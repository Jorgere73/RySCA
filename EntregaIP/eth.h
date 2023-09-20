#ifndef _ETH_H
#define _ETH_H

#include <stdint.h>

/* Tamaño en bytes de las direcciones MAC (48 bits == 6 bytes) */
#define MAC_ADDR_SIZE 6

/* Definición del tipo para almacenar direcciones MAC */
typedef unsigned char mac_addr_t [MAC_ADDR_SIZE];

/* Dirección MAC de difusión: "FF:FF:FF:FF:FF:FF" */
extern mac_addr_t MAC_BCAST_ADDR;

/* Longitud en bytes de una cadena de texto que representa una dirección MAC */
#define MAC_STR_LENGTH 18

/* Maximum Transmission Unit (MTU) de la tramas Ethernet. */
#define ETH_MTU 1500

/* Manejador de un interfaz ethernet. Esta es una estructura opaca que no debe
   ser accedida directamente, sino a través de las funciones de esta librería. */
typedef struct eth_iface eth_iface_t;


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
eth_iface_t * eth_open ( char* ifname );


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
char * eth_getname ( eth_iface_t * iface );


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
void eth_getaddr ( eth_iface_t * iface, mac_addr_t addr );


/* int eth_send 
 * ( eth_iface_t * iface, 
 *   mac_addr_t dst, uint16_t type, unsigned char * data, int data_len );
 *
 * DESCRIPCIÓN:
 *   Esta función permite enviar una trama Ethernet a través de la interfaz
 *   indicada.
 *
 * PARÁMETROS:
 *       'iface': Manejador de la interfaz Ethernet por la que se quiere
 *                enviar el paquete.
 *                La interfaz debe haber sido inicializada con 'eth_open()'
 *                previamente.
 *         'dst': Dirección MAC del equipo destino.
 *        'type': Valor del campo 'Tipo' de la trama Ethernet a enviar.
 *     'payload': Puntero a los datos que se quieren enviar en la trama
 *                Ethernet.
 * 'payload_len': Longitud en bytes de los datos a enviar.
 *
 * VALOR DEVUELTO:
 *   El número de bytes de datos que han podido ser enviados.
 * 
 * ERRORES:
 *   La función devuelve '-1' si se ha producido algún error. 
 */
int eth_send 
( eth_iface_t * iface, 
  mac_addr_t dst, uint16_t type, unsigned char * payload, int payload_len );


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
  int buf_len, long int timeout );


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
( eth_iface_t * ifaces[], int ifnum, long int timeout );


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
int eth_close ( eth_iface_t * iface );


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
void mac_addr_str ( mac_addr_t addr, char str[] );

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
int mac_str_addr ( char* str, mac_addr_t addr );


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
void print_pkt ( unsigned char * packet, int pkt_len, int hdr_len );

#endif /* _ETH_H */
