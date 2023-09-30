#include "udp.h"
#include "arp.h"
#include "eth.h"
#include "ipv4.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <libgen.h>




int main ( int argc, char * argv[] )
{
  /* Mostrar mensaje de ayuda si el número de argumentos es incorrecto */
 if(argc!=5){
		printf("ERROR\n");
		printf("					                                          USO\n");
		printf("udp_client.c <Puerto origen> <Archivo de configuracion> <Archivo con la ruta> <IP destino> <Puerto destinp\n>");
        return 1;
	}
 
  //Procesamos los parametros
  uint16_t puerto_in = atoi(argv[1]);
  char* archivo_config = argv[2];
  char* archivo_route = argv[3];
  char* ip_str = argv[4];
  ipv4_addr_t ip_addr;
  ipv4_str_addr(ip_str,ip_addr);
  uint16_t puerto_dst = atoi(argv[5]);    

  //inicializamos el layer con el udp_open
  udp_layer_t * udp_layer = udp_open(puerto_in,archivo_config,archivo_route);

  //Generamos payload
  unsigned char payload[DEFAULT_PAYLOAD_LEN];
    int i;
    for (i = 0; i < DEFAULT_PAYLOAD_LEN; i++) {
        payload[i] = (unsigned char) i;
    }; 
    unsigned char buffer[UDP_MTU]; //
    ipv4_addr_t sender;
    long int timeout = 2000;
//Enviamos paquete UDP
  printf("ENVIANDO PAQUETE UDP...");
  udp_send(udp_layer,ip_addr,puerto_dst,payload, DEFAULT_PAYLOAD_LEN);

//Recibimos paquete UDP

  printf("RECIBIENDO PAQUETE UDP...");
  udp_recv(udp_layer,buffer,sender,UDP_MTU,timeout);

//Liberamos la memoria

udp_close(udp_layer);


}