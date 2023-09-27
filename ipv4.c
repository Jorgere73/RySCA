
#include "arp.h"
#include "eth.h"
#include <unistd.h>
#include <libgen.h>
#include <string.h>
#include <rawnet.h>
#include <timerms.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include "ipv4_config.h" 
#include "arp.h" 

#define HEADER_LEN_IP 20
#define VERSION_HEADERLEN 0x45
#define FLAGS_FO 0x40


struct ipv4_layer {
eth_iface_t * iface;  /*iface=eth_open("eth1"); */
ipv4_addr_t addr; /* 192.168.1.1 */
ipv4_addr_t netmask; /* 255.255.255.0 */
ipv4_route_table_t * routing_table;
}; 

struct ipv4_frame
{
  uint8_t version_headerLen;
  uint8_t dscp;
  uint16_t total_length;
  uint16_t identification;
  uint8_t flags_fragmentOffset;
  uint8_t ttl;
  uint8_t protocol;
  uint16_t checksum;
  ipv4_addr_t src_ip;
  ipv4_addr_t dst_ip;

};

ipv4_layer_t* ipv4_open(char * file_conf, char * file_conf_route) {
  
    ipv4_layer_t * layer = malloc(sizeof(ipv4_layer_t));
    
    char ifname[4];
    ipv4_addr_t addr;
    ipv4_addr_t netmask;
    ipv4_config_read( file_conf, ifname , addr,netmask);/* 2. Leer direcciones y subred de file_conf */

    memcpy(layer->addr, addr, IPv4_ADDR_SIZE);
    memcpy(layer->netmask, netmask, IPv4_ADDR_SIZE);


    //ipv4_route_table_t * routing_table;
    layer->routing_table=ipv4_route_table_create(); //HAY QUE LIBERAR!!!!!!!! ipv4_route_table_free()

    int numRutasLeidas = ipv4_route_table_read(file_conf_route, layer->routing_table);/* 3. Leer tabla de reenvío IP de file_conf_route */
    if(numRutasLeidas ==0){
      printf("No se ha leido ninguna ruta\n");
    }else if(numRutasLeidas ==-1){
      printf("Se ha producido algún error al leer el fichero de rutas.\n");
    }

    //memcpy(layer->routing_table, routing_table, sizeof(ipv4_route_table_t *));

    
    layer->iface=eth_open ( ifname );/* 4. Inicializar capa Ethernet con eth_open() */
    return layer;

}

int ipv4_close (ipv4_layer_t * layer) {

/* 1. Liberar table de rutas (layer -> routing_table) */
  ipv4_route_table_free(layer->routing_table);
/* 2. Cerrar capa Ethernet con eth_close() */
  eth_close ( layer->iface );

free(layer);
return 0;
}
int ipv4_send (ipv4_layer_t * layer, ipv4_addr_t dst, uint8_t protocol,unsigned char * payload, int payload_len) {

//Metodo para enviar una trama ip
struct ipv4_frame pkt_ip_send;
memset(&pkt_ip_send, 0, sizeof(struct ipv4_frame)); //Limpiamos la estructura para que no haya basura

//INICIALIZAMOS LA ESTRUCTURA
pkt_ip_send.version_headerLen = VERSION_HEADERLEN;
pkt_ip_send.total_length = HEADER_LEN_IP+payload_len;
pkt_ip_send.identification = 0x2816;
pkt_ip_send.flags_fragmentOffset = FLAGS_FO; 
pkt_ip_send.ttl = 64; //Hay que ver que numero ponemos de ttl(Puede que sea 64)
pkt_ip_send.protocol = protocol;
pkt_ip_send.checksum = ipv4_checksum(payload,payload_len);
memcpy(pkt_ip_send.src_ip, layer->addr, IPv4_ADDR_SIZE);
memcpy(pkt_ip_send.dest_addr, dst, IPv4_ADDR_SIZE);

//Ahora hacemos el lookup 

int route = ipv4_route_lookup(,dst); //que ruta hay que poner?

//Si la ip es el siguiente salto (0.0.0.0), directamente la enviamos con eth
eth_send(,,TYPE_IP,payload,payload_len)//interfaz?? //MAC
//Si lookup nos da un sig salto entonces arp para la mac y luego eth
mac_addr_t macdest;
arp_resolve(,,macdest)//que interfaz ponemos?
eth_send(,macdest,TYPE_IP,payload,payload_len)//interfaz??

}

/*int ipv4_recv(ipv4_layer_t * layer, uint8_t protocol,unsigned char buffer [], ipv4_addr_t sender, int buf_len,long int timeout) {

//Metodo para recibir una trama ip
  
}
QUITAD COMENTARIO CUANDO VAYAIS A EDITAR
*/
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


