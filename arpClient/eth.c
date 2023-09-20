#include "eth.h"
#include <rawnet.h>
#include <timerms.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <netinet/in.h>

/* Dirección MAC de difusión: FF:FF:FF:FF:FF:FF */
mac_addr_t MAC_BCAST_ADDR = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };

/* Estructura del manejador del interfaz ethernet */
struct eth_iface {
  rawiface_t * raw_iface; /* Manejador del interfaz "crudo" */
  mac_addr_t mac_address; /* Dirección MAC del interfaz. Se almacena aquí en
                             lugar de consultar al interfaz "en crudo" para
                             evitar una llamada al sistema adcional cada vez
                             que se quiera enviar una trama. */
};

/* Tamaño de la cabecera Ethernet (sin incluir el campo FCS) */
#define ETH_HEADER_SIZE 14
/* Tamaño máximo de una trama Ethernet (sin incluir el campo FCS) */
#define ETH_FRAME_MAX_LENGTH (ETH_HEADER_SIZE + ETH_MTU)

/* Cabecera de una trama Ethernet */
struct eth_frame {
  mac_addr_t dest_addr; /* Dirección MAC destino*/
  mac_addr_t src_addr;  /* Dirección MAC origen */
  uint16_t type;        /* Campo 'Tipo'. 
                           Identificador de la capa de red superior */ 
  unsigned char payload[ETH_MTU]; /* Campo 'payload'. 
                                          Datos de la capa superior */

  /* NOTA: El campo "Frame Checksum" (FCS) no está incluido en la estructura
     porque lo añade automáticamente la tarjeta de red. */
};


/* eth_iface_t * eth_open ( char* ifname );
 *
 * DESCRIPCIÓN: 
 *   Esta función inicializa la interfaz Ethernet especificada para que pueda
 *   ser utilizada por las restantes funciones de la librería.
 *
 *   La memoria del manejador de interfaz devuelto debe ser liberada con la
 *   función 'eth_close()'.
 *
 * PARÁMETROS:
 *   'ifname': Cadena de texto con el nombre de la interfaz Ethernet que se
 *             desea inicializar.
 *
 * VALOR DEVUELTO:
 *   Manejador de la interfaz Ethernet inicializada.
 *
 *   Dicho manejador es un puntero a una estructura opaca que no debe ser
 *   accedida directamente, en su lugar use las funciones de la librería.
 *
 * ERRORES:
 *   La función devuelve 'NULL' si se ha producido algún error. 
 */
eth_iface_t * eth_open ( char* ifname )
{
  struct eth_iface * eth_iface;

  /* Reservar memoria para el manejador del interfaz Ethernet */
  eth_iface = malloc(sizeof(struct eth_iface));
  if (eth_iface == NULL) {
    fprintf(stderr, "eth_open(): ERROR en malloc()\n");
    return NULL;
  }
  
  /* Abrir el interfaz "en crudo" subyacente */
  rawiface_t * raw_iface = rawiface_open(ifname);
  if (raw_iface == NULL) {
    fprintf(stderr, "eth_open(): ERROR en rawiface_open(): %s\n",
            rawnet_strerror());
    return NULL;
  }  
  eth_iface->raw_iface = raw_iface;

  /* Copiar la dirección MAC en el manejador */
  rawiface_getaddr(raw_iface, eth_iface->mac_address);

  return eth_iface;
}


/* char * eth_getname ( eth_iface_t * iface );
 * 
 * DESCRIPCIÓN:
 *   Esta función devuelve el nombre de la interfaz Ethernet especificada.
 *  
 * PARÁMETROS:
 *   'iface': Manejador de la interfaz ethernet de la que se quiere obtener su
 *            nombre. 
 *            La interfaz debe haber sido inicializada con 'eth_open()'
 *            previamente.
 *
 * VALOR DEVUELTO:
 *   Cadena de texto con el nombre de la interfaz.
 *
 * ERRORES:
 *   La función devuelve 'NULL' si la interfaz no ha sido inicializada
 *   correctamente.
 */
char * eth_getname ( eth_iface_t * iface )
{
  char* iface_name = NULL;
  
  if (iface != NULL) {
    iface_name = rawiface_getname(iface->raw_iface);
  }

  return iface_name;
}

/* void eth_getaddr ( eth_iface_t * iface, mac_addr_t addr );
 *
 * DESCRIPCIÓN:
 *   Esta función permite obtener la dirección MAC de la interfaz Ethernet
 *   especificada.
 *
 * PARÁMETROS:
 *   'iface': Manejador de la interfaz Ethernet de la que se quiere obtener su
 *            dirección. 
 *            La interfaz debe haber sido inicializada con 'eth_open()'
 *            previamente.
 *    'addr': Array donde se copiará la dirección MAC de la interfaz Ethernet.
 *            Las direcciones MAC ocupan 'MAC_ADDR_SIZE' bytes.
 */
void eth_getaddr ( eth_iface_t * iface, mac_addr_t addr )
{
  if (iface != NULL) {
    memcpy(addr, iface->mac_address, MAC_ADDR_SIZE);
  }
}

/* int eth_send 
 * ( eth_iface_t * iface, 
 *   mac_addr_t dst, uint16_t type, unsigned char * data, int data_len );
 *
 * DESCRIPCIÓN:
 *   Esta función permite enviar una trama Ethernet a través de la interfaz
 *   indicada.
 *
 * PARÁMETROS:
 *    'iface': Manejador de la interfaz Ethernet por la que se quiere enviar
 *             el paquete.
 *             La interfaz debe haber sido inicializada con 'eth_open()'
 *             previamente.
 *      'dst': Dirección MAC del equipo destino.
 *     'type': Valor del campo 'Tipo' de la trama Ethernet a enviar.
 *     'data': Puntero a los datos que se quieren enviar en la trama
 *             Ethernet.
 * 'data_len': Longitud en bytes de los datos a enviar.
 *
 * VALOR DEVUELTO:
 *   El número de bytes que han podido ser enviados.
 * 
 * ERRORES:
 *   La función devuelve '-1' si se ha producido algún error. 
 */
int eth_send 
( eth_iface_t * iface, 
  mac_addr_t dst, uint16_t type, unsigned char * payload, int payload_len )
{
  int bytes_sent;

  /* Comprobar parámetros */
  if (iface == NULL) {
    fprintf(stderr, "eth_send(): ERROR: iface == NULL\n");
    return -1;
  }

  /* Crear la Trama Ethernet y rellenar todos los campos */
  struct eth_frame eth_frame;
  memcpy(eth_frame.dest_addr, dst, MAC_ADDR_SIZE);
  memcpy(eth_frame.src_addr, iface->mac_address, MAC_ADDR_SIZE);
  eth_frame.type = htons(type);  
  memcpy(eth_frame.payload, payload, payload_len);
  int eth_frame_len = ETH_HEADER_SIZE + payload_len;

  /* Imprimir trama Ethernet */
  char* iface_name = eth_getname(iface);
  char mac_str[MAC_STR_LENGTH];
  mac_addr_str(dst, mac_str);
  printf("eth_send(type=0x%04x, payload[%d]) > %s/%s\n",
         type, payload_len, iface_name, mac_str);
  print_pkt((unsigned char *) &eth_frame, eth_frame_len, ETH_HEADER_SIZE);

  /* Enviar la trama Ethernet creada con rawnet_send() y comprobar errores */
  bytes_sent = rawnet_send
    (iface->raw_iface, (unsigned char *) &eth_frame, eth_frame_len);
  if (bytes_sent == -1) {
    fprintf(stderr, "eth_send(): ERROR en rawnet_send(): %s\n", 
            rawnet_strerror());
    return -1;
  }

  /* Devolver el número de bytes de datos recibidos */
  return (bytes_sent - ETH_HEADER_SIZE);
}

/* int eth_recv 
 * ( eth_iface_t * iface, 
 *   mac_addr_t src, uint16_t type, unsigned char buffer[], long int timeout );
 *
 * DESCRIPCIÓN:
 *   Esta función permite obtener el siguiente paquete recibido por la
 *   interfaz Ethernet indicada. La operación puede esperar indefinidamente o
 *   un tiempo limitando dependiento del parámetro 'timeout'.
 *
 *   Esta función sólo permite recibir paquetes de una única interfaz. Si desea
 *   escuchar varias interfaces Ethernet simultaneamente, utilice la función
 *   'eth_poll()'.
 *
 * PARÁMETROS:
 *    'iface': Manejador de la interfaz Ethernet por la que se desea recibir
 *             un paquete. 
 *             La interfaz debe haber sido inicializada con 'eth_open()'
 *             previamente.
 *      'src': Dirección MAC del equipo que envió la trama Ethernet recibida.
 *             Este es un parámetro de salida. La dirección se copiará en la
 *             memoria indicada, que debe estar reservada previamente.
 *     'type': Valor del campo 'Tipo' de la trama Ethernet que se desea
 *             recibir. 
 *             Las tramas con un valor 'type' diferente serán descartadas.
 *   'buffer': Array donde se almacenarán los datos de la trama recibida.
 *  'buf_len': Longitud del 'buffer' dónde se almacenarán los datos de la trama
 *             recibida. Si se reciben más datos de los que caben el en 'buffer'
 *             sólo se copiaran los primeros 'buf_len' bytes.
 *             datos.
 *  'timeout': Tiempo en milisegundos que debe esperarse a recibir una trama
 *             antes de retornar. Un número negativo indicará que debe
 *             esperarse indefinidamente, mientras que con un '0' la función
 *             retornará inmediatamente, se haya recibido o no una trama.
 *
 * VALOR DEVUELTO:
 *   La longitud en bytes de los datos de la trama recibida (que puede
 *   ser mayor que 'buf_len'), o '0' si no se ha recibido ninguna
 *   trama porque ha expirado el temporizador.
 * 
 * ERRORES:
 *   La función devuelve '-1' si se ha producido algún error. 
 */
int eth_recv 
( eth_iface_t * iface, mac_addr_t src, uint16_t type, unsigned char buffer[], 
  int buf_len, long int timeout )
{
  int payload_len;

  /* Comprobar parámetros */
  if (iface == NULL) {
    fprintf(stderr, "eth_recv(): ERROR: iface == NULL\n");
    return -1;
  }

  /* Inicializar temporizador para mantener timeout si se reciben tramas con
     tipo incorrecto. */
  timerms_t timer;
  timerms_reset(&timer, timeout);

  int frame_len;
  int eth_buf_len = ETH_HEADER_SIZE + buf_len;
  unsigned char eth_buffer[eth_buf_len];
  struct eth_frame * eth_frame_ptr = NULL;
  int is_target_type;
  int is_my_mac;

  do {
    long int time_left = timerms_left(&timer);

    /* Recibir trama del interfaz Ethernet y procesar errores */
    frame_len = rawnet_recv (iface->raw_iface, eth_buffer, eth_buf_len,
                             time_left);
    if (frame_len < 0) {
      fprintf(stderr, "eth_recv(): ERROR en rawnet_recv(): %s\n", 
              rawnet_strerror());
      return -1;
    } else if (frame_len == 0) {
      /* Timeout! */
      return 0;
    } else if (frame_len < ETH_HEADER_SIZE) {
      fprintf(stderr, "eth_recv(): Trama de tamaño invalido: %d bytes\n",
              frame_len);
      continue;
    }

    /* Comprobar si es la trama que estamos buscando */
    eth_frame_ptr = (struct eth_frame *) eth_buffer;
    is_my_mac = (memcmp(eth_frame_ptr->dest_addr, 
                        iface->mac_address, MAC_ADDR_SIZE) == 0);
    is_target_type = (ntohs(eth_frame_ptr->type) == type);

  } while ( ! (is_my_mac && is_target_type) );
  
  /* Trama recibida con 'tipo' indicado. Copiar datos y dirección MAC origen */
  memcpy(src, eth_frame_ptr->src_addr, MAC_ADDR_SIZE);
  payload_len = frame_len - ETH_HEADER_SIZE;
  if (buf_len > payload_len) {
    buf_len = payload_len;
  }
  memcpy(buffer, eth_frame_ptr->payload, buf_len);

  return payload_len;
}


/* int eth_poll 
 * ( eth_iface_t * ifaces[], int ifnum, long int timeout );
 *
 * DESCRIPCIÓN:
 *   Esta función permite esperar paquetes en múltiples interfaces Ethernet
 *   simultaneamente. Cuando alguna de las interfaces indicadas reciba una
 *   trama, la función devolverá la primera interfaz que tiene una trama
 *   listo para ser recibida mediante la funcion 'eth_recv()'.
 *
 *   Esta operación puede escuchar de los interfaces Ethernet indefinidamente
 *   o un tiempo limitado dependiento del parámetro 'timeout'.
 *
 * PARÁMETROS:
 *  'ifaces': Array con los manejadores de interfaces Ethernet por los que se
 *            quiere recibir.
 *            Todos los interfaces deben haber sido inicializados con
 *            'eth_open()' previamente.
 *   'ifnum': Número de interfaces que aparecen en el array 'ifaces'.
 * 'timeout': Tiempo en milisegundos que debe esperarse a recibir una trama
 *            antes de retornar. Un número negativo indicará que debe
 *            esperarse indefinidamente.
 *
 * VALOR DEVUELTO:
 *   El índice del primer interfaz [0, ifnum-1] que tiene una trama lista para
 *   ser recibida o '-2' si ha expirado el temporizador.
 * 
 * ERRORES:
 *   La función devuelve '-1' si se ha producido algún error.
 */
int eth_poll 
( eth_iface_t * ifaces[], int ifnum, long int timeout )
{
  int iface_index;
    
  /* Crear lista de interfaces hardware */
  rawiface_t * raw_ifaces[ifnum];
  int i;
  for (i=0; i<ifnum; i++) {
    raw_ifaces[i] = ifaces[i]->raw_iface;
  }

  /* Llamar a rawnet_poll() y procesar errores */
  iface_index = rawnet_poll(raw_ifaces, ifnum, timeout);
  if (iface_index == -1) {
    fprintf(stderr, "eth_poll(): ERROR en rawnet_poll(): %s\n", 
            rawnet_strerror());
    return -1;
  } else if (iface_index == -2) {
    /* Timeout! */
    return -2;
  }

  return iface_index;
}


/* int eth_close ( eth_iface_t * iface );
 * 
 * DESCRIPCIÓN:
 *   Esta función cierra la interfaz Ethernet especificada y libera la memoria
 *   de su manejador.
 *
 * PARÁMETROS:
 *   'iface': Manejador de la interfaz Ethernet que se desea cerrar.
 *
 * VALOR DEVUELTO:
 *   Devuelve 0 si la interfaz Ethernet se ha cerrado correctamente.
 * 
 * ERRORES:
 *   La función devuelve '-1' si se ha producido algún error. 
 */
int eth_close ( eth_iface_t * iface )
{
  int err = -1;

  if (iface != NULL) {
    err = rawiface_close(iface->raw_iface);
    free(iface);
  }

  return err;
}


/* void mac_addr_str ( mac_addr_t addr, char str[] );
 *
 * DESCRIPCIÓN:
 *   Esta función genera una cadena de texto que representa la dirección MAC
 *   indicada.
 *
 * PARÁMETROS:
 *   'addr': La dirección MAC que se quiere representar textualente.
 *    'str': Memoria donde se desea almacenar la cadena de texto generada.
 *           Deben reservarse al menos 'MAC_STR_LENGTH' bytes.
 */
void mac_addr_str ( mac_addr_t addr, char str[] )
{
  if (str != NULL) {
    sprintf(str, "%02X:%02X:%02X:%02X:%02X:%02X",
            addr[0], addr[1], addr[2], addr[3], addr[4], addr[5]);
  }
}

/* int mac_str_addr ( char* str, mac_addr_t addr );
 *
 * DESCRIPCIÓN:
 *   Esta función analiza una cadena de texto en busca de una dirección MAC.
 *
 * PARÁMETROS:
 *    'str': La cadena de texto que se desea procesar.
 *   'addr': Memoria donde se almacena la dirección MAC encontrada.
 *
 * VALOR DEVUELTO:
 *   Se devuelve 0 si la cadena de texto representaba una dirección MAC.
 *
 * ERRORES:
 *   La función devuelve -1 si la cadena de texto no representaba una
 *   dirección MAC.
 */
int mac_str_addr ( char* str, mac_addr_t addr )
{
  int err = -1;

  if (str != NULL) {
    unsigned int addr_int[MAC_ADDR_SIZE];
    int len = sscanf(str, "%02X:%02X:%02X:%02X:%02X:%02X",
                     &addr_int[0], &addr_int[1], &addr_int[2],
                     &addr_int[3], &addr_int[4], &addr_int[5]);

    if (len == MAC_ADDR_SIZE) {
      int i;
      for (i=0; i<MAC_ADDR_SIZE; i++) {
        addr[i] = (unsigned char) addr_int[i];
      }      
      err = 0;
    }
  }
  
  return err;
}


/* void print_pkt ( unsigned char * packet, int pkt_len, int hdr_len );
 * 
 * DESCRIPCIÓN:
 *   Esta función permite imprimir por salida estándar los contenidos del
 *   paquete especificado. Además, los primeros 'hdr_len' bytes del mensaje se
 *   resaltarán con otro color para facilitar su visualización.
 *
 * PARÁMETROS:
 *     'packet': Puntero al contenido del paquete que se quiere imprimir.
 *    'pkt_len': Longitud total en bytes del paquete a imprimir.
 *    'hdr_len': Número de bytes iniciales que se quieren resaltar,
 *               imprimiendolos en otro color. Utilice cualquier valor menor o
 *               igual a cero para no utilizar esta característica.
 */
void print_pkt ( unsigned char * packet, int pkt_len, int hdr_len )
{
  if ((packet == NULL) || (pkt_len <= 0)) {
    return;
  }

  int i;
  for (i=0; i<pkt_len; i++) {

    if ((i % 8) == 0) {
      /* Se ha llegado al final de una línea */
      if (i > 0) {
        /* Insertar salto de línea */
        printf("\n");

        /* Cambiar al color normal para el índice */
        if (i <= hdr_len) {
          printf("\033[0m");
        }
      }

      /* Imprimir un byte hexadecimal de índice al principio de cada línea */
      printf("  0x%04x:", i);
      
      if (i < hdr_len)  {
        /* Imprimir los primeros bytes de la cabecera con un color diferente */
        printf("\033[1;34m");
      }

    } else if ((i % 4) == 0) {
      /* Imprimir separador entre cada pareja de 4 bytes */
      printf(" ");
    } 

    /* Volver al color normal cuando termina 'hdr_len' */
    if (i == hdr_len) {
      printf("\033[0m");
    }

    /* Imprimir cada byte del paquete en hexadecimal */
    printf(" %02x", packet[i]);
  }

  /* Todo el paquete era cabecera, reestablecer el color normal */
  if (pkt_len <= hdr_len) {
    printf("\033[0m");
  }
  
  printf("\n");
}
