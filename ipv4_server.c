#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <libgen.h>
#include <rawnet.h>
#include <timerms.h>
#include <arpa/inet.h>
#include "log.h"

#include "ipv4.h"
//Un servidor su funcion es primero recibir datos de un cliente y mas adelante enviar datos de vuelta.
int main(int argc, char* argv[]){
    if(argc!=2){
        printf("ERROR\n");
        printf("    USO\n");
        printf("ipv4_server.c <Protocolo elegido>");
    } 
    //Procesamos los parametros
    uint8_t protocol = atoi(argv[1]); //atoi para pasar de char* a int

    //Inicializamos el layer con los archivos de configuracion
    ipv4_layer_t* layer;
    layer = ipv4_open("ipv4_config_server.txt","ipv4_route_table_server.txt"); //Esto se puede psaasar por parametro
    if(layer ==NULL){
        fprintf(stderr, "ERROR en ipv4_open()");
        return 0;
    }
    unsigned char buffer[IP_MTU]; 
    ipv4_addr_t sender;
    long int timeout = 10000;
    int len = 0;
    do
    {
        printf("Esperando datagrama IP...\n");
        len = ipv4_recv(layer,protocol,buffer,sender,IP_MTU,timeout);
        log_trace("%d", len);
        //Reenviamos el mismo datagrama? NO se muy bien cual es la funcion del servdior
        //De momento vamos a hacer que reenvie el mismo para asegurarnos que funciona
        //Mas adelante le implementaremos una funcionalidad si es que la tiene
    } while (len <= 0);
        printf("Enviando datagrama desde servidor");
        int sendstatus = ipv4_send(layer, sender, protocol, buffer, len); //devolvemos el mismo datagrama con la longitud del que nos han enviado
    
    //ipv4_close(layer);
} 