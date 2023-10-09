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
   uint8_t protocol = (uint8_t) atoi(argv[1]);

    //Inicializamos el layer con los archivos de configuracion(IPv4_open)
    ipv4_layer_t* layer = ipv4_open("ipv4_config_server.txt","ipv4_route_table_server.txt"); //Esto se puede psaasar por parametro
    if(layer ==NULL){
        fprintf(stderr, "ipv4_server--ERROR en ipv4_open()");
        return 0;
    }
    unsigned char buffer[IP_MTU]; 
    ipv4_addr_t sender;
    long int timeout = 5000;
    int len_recibido = 0;
    while(1){
        printf("ipv4_server--Esperando datagrama IP...Protocolo: %d\n", protocol);
        len_recibido = ipv4_recv(layer,protocol,buffer,sender,IP_MTU,timeout);
        if(len_recibido<0){
            printf("ipv4_server--ERROR EN EL RECV CLIENTE. Esperando de nuevo...\n");
        }else if(len_recibido == 0){
            printf("ipv4_server-- Se han recibido 0 bytes. Esperando de nuevo...\n");
        }else{
            printf("ipv4_server--Paquete recibido correctamente del cliente\n");
            printf("ipv4_server--Valor de len %d\n", len_recibido);
            printf("ipv4_server--Vamos a imprimir el paquete recibido\n");
            print_pkt(buffer, len_recibido, 0);
        }
        
        //Reenviamos el mismo datagrama? NO se muy bien cual es la funcion del servdior
        //De momento vamos a hacer que reenvie el mismo para asegurarnos que funciona
        //Mas adelante le implementaremos una funcionalidad si es que la tiene
        break;
    }
    printf("ipv4_server--Enviando datagrama desde servidor\n");
    ipv4_send(layer, sender, protocol, buffer, len_recibido); //devolvemos el mismo datagrama con la longitud del que nos han enviado
        
    printf("ipv4_server--Cerrando interfaz IP\n");
    ipv4_close(layer);
    return 0;
} 
