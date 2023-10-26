#include "eth.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <libgen.h>

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
  if (argc != 3) {
    printf("Uso: %s <iface> <tipo>\n", myself);
    printf("       <iface>: Nombre de la interfaz Ethernet\n");
    printf("        <tipo>: Campo 'Tipo' de las tramas Ethernet\n");
    exit(-1);
  }


  /* Procesar los argumentos de la línea de comandos */
  char * iface_name = argv[1];

  /* El Tipo puede indicarse en hexadecimal (0x0800) o en decimal (2048) */
  char* eth_type_str = argv[2];
  char* endptr;
  int eth_type_int = (int) strtol(eth_type_str, &endptr, 0);
  if ((*endptr != '\0') || (eth_type_int < 0) || (eth_type_int > 0x0000FFFF)) {
    fprintf(stderr, "%s: Tipo Ethernet incorrecto: '%s'\n",
            myself, eth_type_str);
    exit(-1);
  }
  uint16_t eth_type = (uint16_t) eth_type_int;

  /* Abrir la interfaz Ethernet */
  eth_iface_t * eth_iface = eth_open(iface_name);
  if (eth_iface == NULL) {
    fprintf(stderr, "%s: ERROR en eth_open(\"%s\")\n", myself, iface_name);
    exit(-1);
  }

  mac_addr_t server_addr;
  eth_getaddr(eth_iface, server_addr);  
  char server_addr_str[MAC_STR_LENGTH];
  mac_addr_str(server_addr, server_addr_str);
    
  printf("Abriendo interfaz Ethernet %s. Direccion MAC: %s\n",
         iface_name, server_addr_str);

  while(1) {

    /* Recibir trama Ethernet del Cliente */
    unsigned char buffer[ETH_MTU];
    mac_addr_t src_addr;
    long int timeout = -1;

    printf("Escuchando tramas Ethernet (tipo=0x%04x) ...\n", eth_type);
    
    int payload_len = eth_recv(eth_iface, src_addr, eth_type, buffer, 
			       ETH_MTU, timeout);
    if (payload_len == -1) {
      fprintf(stderr, "%s: ERROR en eth_recv()\n", myself);
      exit(-1);
    }

    char src_addr_str[MAC_STR_LENGTH];
    mac_addr_str(src_addr, src_addr_str);
    
    printf("Recibidos %d bytes del Cliente Ethernet (%s):\n", 
           payload_len, src_addr_str);
    print_pkt(buffer, payload_len, 0);

    /* Enviar la misma trama Ethernet de vuelta al Cliente */
    printf("Enviando %d bytes al Cliente Ethernet (%s):\n",
           payload_len, src_addr_str);
    print_pkt(buffer, payload_len, 0);

    int len = eth_send(eth_iface, src_addr, eth_type, buffer, payload_len);
    if (len == -1) {
      fprintf(stderr, "%s: ERROR en eth_send()\n", myself);
    }
  }

  /* Cerrar interfaz Ethernet */
  printf("Cerrando interfaz Ethernet %s.\n", iface_name);

  eth_close(eth_iface);

  return 0;
}
