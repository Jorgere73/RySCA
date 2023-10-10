#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <libgen.h>
#include <rawnet.h>
#include <timerms.h>
#include <arpa/inet.h>

#include "udp.h"
#include "ipv4.h" 

//Un servidor su funcion es primero recibir datos de un cliente y mas adelante enviar datos de vuelta.
int main(int argc, char* argv[]){
    if(argc!=2){
        printf("ERROR\n");
        printf("    USO\n");
        printf("udp_server.c <Puerto elegido entrada> <Archivo de configuracion> <Archivo con la ruta>");
    } 
    //Procesamos los parametros
    uint16_t puerto_in = atoi(argv[1]);
    char* archivo_config = argv[2];
    char* archivo_route = argv[3];

    //inicializamos el layer con el udp_open
    udp_layer_t * udp_layer = udp_open(puerto_in,archivo_config,archivo_route);
    if(udp_layer ==NULL){
        fprintf(stderr, "udp_server--ERROR en udp_open()");
    }

    unsigned char buffer[UDP_MTU]; 
    ipv4_addr_t sender;
    long int timeout = 2000;
    printf("udp_server--Esperando datagrama UDP...\n");
    uint16_t puerto;
    while(1){
    int len= udp_recv(udp_layer,buffer,sender,UDP_MTU,&puerto,timeout);
    printf("udp_server--%d bytes recibidos desde el puerto %d\n", len,&puerto);
    //Reenviamos el mismo datagrama? NO se muy bien cual es la funcion del servdior
    //De momento vamos a hacer que reenvie el mismo para asegurarnos que funciona
    //Mas adelante le implementaremos una funcionalidad si es que la tiene

    printf("udp_server--Enviando datagrama desde servidor\n");
    udp_send(udp_layer,sender,puerto,buffer, len); //PUERTO???????????
}
    //Cerramos el layer
    ipv4_close(layer);
} 