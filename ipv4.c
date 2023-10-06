#include <unistd.h>
#include <libgen.h>
#include <string.h>
#include <rawnet.h>
#include <timerms.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include "eth.h"
#include "arp.h"
#include "ipv4_config.h"  
#include "log.h"




ipv4_layer_t* ipv4_open(char * file_conf, char * file_conf_route) {
  
    ipv4_layer_t* layer = (ipv4_layer_t*)calloc(1, sizeof(ipv4_layer_t));
    
    char ifname[16];
    //Leemos el archivo de configuracion y de ahi sacamos la interfaz, la IP y la mascara
    ipv4_config_read( file_conf, ifname , layer->addr,layer->netmask);
    //Imprimimos lo que sale de la funcion config_read
    log_trace("Estamos usando la interfaz: %s",ifname);
    char ip_str[IPv4_STR_MAX_LENGTH]; 
    ipv4_addr_str(layer->addr,ip_str);
    log_trace("Estamos con la IP: %s",ip_str);
    char netmask_str[IPv4_STR_MAX_LENGTH]; 
    ipv4_addr_str(layer->netmask,netmask_str);
    
  
    layer->routing_table=ipv4_route_table_create(); //HAY QUE LIBERAR!!!!!!!! ipv4_route_table_free()

    int numRutasLeidas = ipv4_route_table_read(file_conf_route, layer->routing_table);/* Leer tabla de reenvío IP de file_conf_route */
    log_trace("Se han leído %d rutas", numRutasLeidas);
    if(numRutasLeidas == 0){
      log_trace("No se ha leido ninguna ruta");
    }else if(numRutasLeidas ==-1){
      log_trace("Se ha producido algún error al leer el fichero de rutas.");
    }
    layer->iface=eth_open ( ifname );/* 4. Inicializar capa Ethernet con eth_open() */
    return layer;
}


int ipv4_close (ipv4_layer_t * layer) {
  // 1. Liberar table de rutas (layer -> routing_table)
    ipv4_route_table_free(layer->routing_table);
  // 2. Cerrar capa Ethernet con eth_close() 
    eth_close ( layer->iface );
  //Liberar la memoria reservada en el open para el layer
    free(layer);
    return 0;
}


int ipv4_send (ipv4_layer_t * layer, ipv4_addr_t dst, uint8_t protocol,unsigned char * payload, int payload_len) {
  //Metodo para enviar una trama ip
  ipv4_frame* pkt_ip_send = calloc(1, sizeof(ipv4_frame));
  mac_addr_t macdst;
  memset(&macdst, 0, sizeof(mac_addr_t));
  //Variable donde guardaremos la MAC de destino
  //memset(&pkt_ip_send, 0, sizeof(struct ipv4_frame)); //Limpiamos la estructura para que no haya basura
  
  //INICIALIZAMOS LA ESTRUCTURA
  pkt_ip_send->version_headerLen = VERSION_HEADERLEN;
  //ipv4_route_table_print(layer->routing_table);
  pkt_ip_send->total_length = htons(HEADER_LEN_IP+payload_len);
  pkt_ip_send->identification = htons(0x2816);
  pkt_ip_send->flags_fragmentOffset = FLAGS_FO; 
  pkt_ip_send->ttl = 32; //Hay que ver que numero ponemos de ttl(Puede que sea 64)
  pkt_ip_send->protocol = protocol;
  memcpy(pkt_ip_send->src_ip, layer->addr, IPv4_ADDR_SIZE);
  memcpy(pkt_ip_send->dst_ip, dst, IPv4_ADDR_SIZE);
  pkt_ip_send->checksum = 0;
  pkt_ip_send->checksum = htons(ipv4_checksum((unsigned char*) pkt_ip_send,HEADER_LEN_IP));
  memcpy(pkt_ip_send->payload, payload, payload_len);
  ipv4_route_t* route;
  route = ipv4_route_table_lookup(layer->routing_table, dst);
  //ipv4_route_print(route);
  
  //route es la ruta más rápida encontrada en la tabla de rutas del layer hasta la dirección dst.
  //De no funcionar, devuelve -1
  
  //Si el destino se encuentra en la misma subred que nuestro host, encontramos su MAC y enviamos
  if(memcmp(route->gateway_addr, IPv4_ZERO_ADDR, IPv4_ADDR_SIZE)==0)
  {
    printf("%s", eth_getname(layer->iface));
    int arp = arp_resolve(layer->iface, dst, &macdst);
    
    //Sacamos la dirección MAC de destino
    if (arp < 0)
    {
          log_trace("MAC destino no ubicada\n");
          return -1;
    }
    int a = eth_send(layer->iface, macdst, TYPE_IP,(unsigned char*)pkt_ip_send, HEADER_LEN_IP+payload_len);
    free(pkt_ip_send);
    if (a < 0)
      {
          log_trace("Ha ocurrido un error en eth_send\n");
      }
      else if (a > 0)
      {
          log_trace("Número de bytes enviados: %d\n", a);
      }
    return (a-HEADER_LEN_IP);
  }
  //Si el destino está fuera de la subred (hay salto), tendremos que sacar la MAC del siguiente salto y enviárselo a él
  else
  {
    int arp = arp_resolve(layer->iface, route->gateway_addr, &macdst);
    
    if (arp < 0)
      {
          log_trace("MAC destino no ubicada");
          return -1;
      }
    //Sacamos dirección MAC del salto
    int a = eth_send(layer->iface, macdst, TYPE_IP, (unsigned char*)pkt_ip_send, HEADER_LEN_IP+payload_len); 
    free(pkt_ip_send);
    if (a < 0)
      {
          log_trace("Ha ocurrido un error");
          return -1;
      }
    return (a-HEADER_LEN_IP);
    
  }
}

int ipv4_recv(ipv4_layer_t * layer, uint8_t protocol,unsigned char* buffer, ipv4_addr_t sender, int buf_len,long int timeout) 
{

  //Metodo para recibir una trama ip
  struct ipv4_frame pkt_ip_recv;

  // Inicializar temporizador para mantener timeout si se reciben tramas con tipo incorrecto.
  timerms_t timer;
  timerms_reset(&timer, timeout);
  // OBTENER MAC E IP PROPIA
  mac_addr_t macPropia;
  eth_getaddr(layer->iface, macPropia);
  int isIP;
  int isProtocol;
  char addr_str[IPv4_STR_MAX_LENGTH];
  int eth = 0;
  long int time_left;
  do {
    time_left = timerms_left(&timer);
    eth = eth_recv(layer->iface, macPropia ,TYPE_IP, (unsigned char*) &pkt_ip_recv, buf_len+HEADER_LEN_IP, time_left);
    
    if (eth <= 0)
    {
      log_trace("No se ha recibido nada");
      return -1;
    }
    else
    {
      ipv4_addr_str(pkt_ip_recv.src_ip, addr_str);
     // if(addr_str == NULL) { continue; }
      isIP = (memcmp(layer->addr,pkt_ip_recv.dst_ip,IPv4_ADDR_SIZE)==0); //Miramos si la ip que nos pasan por parametro es igual a la que nos llega
      if(pkt_ip_recv.protocol == protocol)//Comprobamos si es el protocolo que nos pasan por parametro
      {
         isProtocol= 1;
      } else 
      { 
        isProtocol = 0;
      }

      // Recibir trama del interfaz Ethernet y procesar errores 

    log_trace("%d %d", protocol, pkt_ip_recv.protocol );
      if(isProtocol == 1 && isIP == 1)
      {
        //TODO: memcmp
        //buffer = (unsigned char*)&pkt_ip_recv;
       log_trace("Paquete enviado desde: %s", &addr_str);
       sender = pkt_ip_recv.src_ip;
        log_trace("Número de bytes recibidos: %d", eth);
        /*log_trace("IPv4 Recibido: ");
        for (int i = 0; i < sizeof(struct ipv4_frame); i++) {
            log_trace("%02x ", ((unsigned char *)&pkt_ip_recv)[i]);
        */
       log_trace("%d", eth);
        return eth;
      }
    }
    if(time_left <= 0)
    {
      log_trace("No se ha recibido paquete IP");
      return -1;
    }
    //Hacemos las comprobaciones necesarias(Que esta bien) para salir del do while
    
  } while (!(isIP && isProtocol));


return 0;
  
}


/* Dirección IPv4 a cero: "0.0.0.0" */
ipv4_addr_t IPv4_ZERO_ADDR = { 0, 0, 0, 0 };


/* void ipv4_addr_str ( ipv4_addr_t addr, char* str );
 *
 * DESCRIPCIÓN:
 *   Esta función genera una cadena de texto que representa la dirección IPv4
 *   indicada.
 *
 * PARÁMETROS:
 *   'addr': La dirección IP que se quiere representar textualente.
 *    'str': Memoria donde se desea almacenar la cadena de texto generada.
 *           Deben reservarse al menos 'IPv4_STR_MAX_LENGTH' bytes.
 */
void ipv4_addr_str ( ipv4_addr_t addr, char* str )
{
  if (str != NULL) {
    sprintf(str, "%d.%d.%d.%d",
            addr[0], addr[1], addr[2], addr[3]);
  }
}


/* int ipv4_str_addr ( char* str, ipv4_addr_t addr );
 *
 * DESCRIPCIÓN:
 *   Esta función analiza una cadena de texto en busca de una dirección IPv4.
 *
 * PARÁMETROS:
 *    'str': La cadena de texto que se desea procesar.
 *   'addr': Memoria donde se almacena la dirección IPv4 encontrada.
 *
 * VALOR DEVUELTO:
 *   Se devuelve 0 si la cadena de texto representaba una dirección IPv4.
 *
 * ERRORES:
 *   La función devuelve -1 si la cadena de texto no representaba una
 *   dirección IPv4.
 */
int ipv4_str_addr ( char* str, ipv4_addr_t addr )
{
  int err = -1;

  if (str != NULL) {
    unsigned int addr_int[IPv4_ADDR_SIZE];
    int len = sscanf(str, "%d.%d.%d.%d", 
                     &addr_int[0], &addr_int[1], 
                     &addr_int[2], &addr_int[3]);

    if (len == IPv4_ADDR_SIZE) {
      int i;
      for (i=0; i<IPv4_ADDR_SIZE; i++) {
        addr[i] = (unsigned char) addr_int[i];
      }
      
      err = 0;
    }
  }
  
  return err;
}


/*
 * uint16_t ipv4_checksum ( unsigned char * data, int len )
 *
 * DESCRIPCIÓN:
 *   Esta función calcula el checksum IP de los datos especificados.
 *
 * PARÁMETROS:
 *   'data': Puntero a los datos sobre los que se calcula el checksum.
 *    'len': Longitud en bytes de los datos.
 *
 * VALOR DEVUELTO:
 *   El valor del checksum calculado.
 */
uint16_t ipv4_checksum ( unsigned char * data, int len )
{
  int i;
  uint16_t word16;
  unsigned int sum = 0;
    
  /* Make 16 bit words out of every two adjacent 8 bit words in the packet
   * and add them up */
  for (i=0; i<len; i=i+2) {
    word16 = ((data[i] << 8) & 0xFF00) + (data[i+1] & 0x00FF);
    sum = sum + (unsigned int) word16;	
  }

  /* Take only 16 bits out of the 32 bit sum and add up the carries */
  while (sum >> 16) {
    sum = (sum & 0xFFFF) + (sum >> 16);
  }

  /* One's complement the result */
  sum = ~sum;

  return (uint16_t) sum;
}

