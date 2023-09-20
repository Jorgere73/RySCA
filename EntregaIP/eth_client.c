#include "eth.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <libgen.h>

#define DEFAULT_PAYLOAD_LENGTH 1500

/* int main ( int argc, char * argv[] );
 * 
 * DESCRIPCIÓN:
 *   Función principal del programa.
 *
 * PARÁMETROS:
 *   'argc': Número de parámetros pasados al programa por la línea de
 *           comandos, incluyendo el propio nombre del programa.
 *   'argv': Array con los parámetros pasado al programa por línea de
 *           comandos. 
 *           El primer parámetro (argv[0]) es el nombre del programa.
 *
 * VALOR DEVUELTO:
 *   Código de salida del programa.
 */
int main ( int argc, char * argv[] )
{
  /* Mostrar mensaje de ayuda si el número de argumentos es incorrecto */
  char * myself = basename(argv[0]);
  if ((argc < 4) || (argc >5)) {
    printf("Uso: %s <iface> <tipo> <mac> [<long>]\n", myself);
    printf("       <iface>: Nombre de la interfaz Ethernet\n");
    printf("        <tipo>: Campo 'Tipo' de las tramas Ethernet\n");
    printf("         <mac>: Dirección MAC del servidor Ethernet\n");
    printf("        <long>: Longitud de los datos enviados al servidor Ethernet\n");
    printf("                (%d bytes por defecto)\n", DEFAULT_PAYLOAD_LENGTH);
    exit(-1);
  }

  /* Procesar los argumentos de la línea de comandos */
  char * iface_name = argv[1];

  char* eth_type_str = argv[2];
  char* endptr;
  /* El Tipo puede indicarse en hexadecimal (0x0800) o en decimal (2048) */
  int eth_type_int = (int) strtol(eth_type_str, &endptr, 0);
  if ((*endptr != '\0') || (eth_type_int < 0) || (eth_type_int > 0x0000FFFF)) {
    fprintf(stderr, "%s: Tipo Ethernet incorrecto: '%s'\n",
            myself, eth_type_str);
    exit(-1);
  }
  uint16_t eth_type = (uint16_t) eth_type_int;

  char* server_addr_str = argv[3];
  mac_addr_t server_addr;
  int err = mac_str_addr(server_addr_str, server_addr);
  if (err != 0) {
    fprintf(stderr, "%s: Dirección MAC del servidor incorrecta: '%s'\n",
            myself, argv[2]);
    exit(-1);
  }

  int payload_len = DEFAULT_PAYLOAD_LENGTH;
  if (argc == 5) {
    char * payload_len_str = argv[4];
    payload_len = atoi(payload_len_str);
    if ((payload_len < 0) || (payload_len > 1500)) {
      fprintf(stderr, "%s: Longitud de payload incorrecta: '%s'\n",
              myself, payload_len_str);
      exit(-1);
    }
  }

  /* Abrir la interfaz Ethernet */
  eth_iface_t * eth_iface = eth_open(iface_name);
  if (eth_iface == NULL) {
    fprintf(stderr, "%s: ERROR en eth_open(\"%s\")\n", myself, iface_name);
    exit(-1);
  }

  mac_addr_t client_addr;
  eth_getaddr(eth_iface, client_addr);  
  char client_addr_str[MAC_STR_LENGTH];
  mac_addr_str(client_addr, client_addr_str);
  
  printf("Abriendo interfaz Ethernet %s. Dirección MAC: %s\n", 
         iface_name, client_addr_str);

  /* Generar payload */
  unsigned char payload[payload_len];
  int i;
  for (i=0; i<payload_len; i++) {
    payload[i] = (unsigned char) i;
  }

  int len;
  mac_addr_t src_addr;
  unsigned char buffer[ETH_MTU];

  /* Enviar trama Ethernet al Servidor */
  printf("Enviando %d bytes al Servidor Ethernet (%s):\n",
         payload_len, server_addr_str);
  print_pkt(payload, payload_len, 0);
  
  err = eth_send(eth_iface, server_addr, eth_type, payload, payload_len);
  if (err == -1) {
    fprintf(stderr, "%s: ERROR en eth_send()\n", myself);
    exit(-1);
  }   
  
  /* Recibir trama Ethernet del Servidor y procesar errores */
  long int timeout = 2000;
  len = eth_recv(eth_iface, src_addr, eth_type, buffer, ETH_MTU, timeout);
  if (len == -1) {
    fprintf(stderr, "%s: ERROR en eth_recv()\n", myself);
  } else if (len == 0) {
    fprintf(stderr, "%s: ERROR: No hay respuesta del Servidor Ethernet\n",
            myself);
  }

  if (len > 0) {
    char src_addr_str[MAC_STR_LENGTH];
    mac_addr_str(src_addr, src_addr_str);    

    printf("Recibidos %d bytes del Servidor Ethernet (%s)\n", 
           len, src_addr_str);
    print_pkt(buffer, len, 0);
  }

  /* Cerrar interfaz Ethernet */
  printf("Cerrando interfaz Ethernet %s.\n", iface_name);

  eth_close(eth_iface);

  return 0;
}
