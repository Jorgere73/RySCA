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
    printf("ipv4_server--Esperando datagrama IP...Protocolo: %d\n", protocol);
    //Inicializamos el layer con los archivos de configuracion(IPv4_open)
    ipv4_layer_t* layer = ipv4_open("ipv4_config_server.txt","ipv4_route_table_server.txt"); //Esto se puede psaasar por parametro
    if(layer ==NULL){
        fprintf(stderr, "ipv4_server--ERROR en ipv4_open()");
        return 0;
    }
    unsigned char buffer[IP_MTU]; 
    ipv4_addr_t sender;
    long int timeout = -1; //Infinitos ms de timer
    int len_recibido = 0;
    while(1){
        
        len_recibido = ipv4_recv(layer,protocol,buffer,sender,IP_MTU,timeout);
        if(len_recibido<0){
            printf("ipv4_server--ERROR EN EL RECV CLIENTE. Esperando de nuevo...\n");
            return -1;
        }else if(len_recibido == 0){
            printf("ipv4_server-- Se han recibido 0 bytes. Esperando de nuevo...\n");
            exit(0);
        }
            printf("ipv4_server--Paquete recibido correctamente del cliente\n");

            char sender_str[IPv4_STR_MAX_LENGTH];
            ipv4_addr_str(sender, sender_str);
            printf("ipv4_server--%d bytes recibidos de: %s\n", len_recibido, sender_str);
            //------------------FALLO DE QUE RECIBE DE 0.0.0.
            

            printf("ipv4_server--Vamos a imprimir el paquete recibido\n");
            print_pkt(buffer, len_recibido, 0);

            char ipACapon[IPv4_STR_MAX_LENGTH] = "192.100.100.101";
            ipv4_addr_t ipACapon_addr;
            ipv4_str_addr(ipACapon,ipACapon_addr);
            char ACapon[IPv4_STR_MAX_LENGTH];
            ipv4_addr_str(ipACapon_addr,ACapon);
            printf("ipv4_server--Enviando datagrama desde servidor a %s\n", ACapon);
            int len_send = ipv4_send(layer, ipACapon_addr, protocol, buffer, len_recibido); 
            printf("ipv4_server--%d bytes enviados.\n", len_send);
            if(len_send == -1){
                return -1;
            }
            if(len_send == 0){
                exit(0);
            }
        
        
        
        //Reenviamos el mismo datagrama? NO se muy bien cual es la funcion del servdior
        //De momento vamos a hacer que reenvie el mismo para asegurarnos que funciona
        //Mas adelante le implementaremos una funcionalidad si es que la tiene
        
    }
    
        
    printf("ipv4_server--Cerrando interfaz IP\n");
    int len_close = ipv4_close(layer);
    if(len_close){
        return -1;
    }
    return 0;
} 
