#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <libgen.h>
#include "header.h"
#include "eth.h"
#include "ipv4.h"
#include "ipv4_route_table.h"
#include "arp.h"
#include "log.h"

#define IP_MTU 1480
#define DEFAULT_PAYLOAD_LEN 200

 
//int ipv4_send (ipv4_layer_t * layer, ipv4_addr_t dst, uint8_t protocol,unsigned char * payload, int DEFAULT_PAYLOAD_LEN)

int main(int argc, char* argv[]){
	if(argc != 4)
    {
		printf("ERROR\n");
		printf("					USO\n");
		printf("	ipv4_client.c <IP_destino> <protocolo> <nivel debug>");
        return 1;
	}
    switch(argv[3][0])
    {//todas las opciones empiezan por letra distinta, solo miro la primera
        case 't': 
            log_set_level(LOG_TRACE);
            //Modo trace (devuelve trazas de resultado del código)
            break;
        case 'd':
            log_set_level(LOG_DEBUG);
            break;
        default:
            log_set_level(LOG_INFO);
            //Modo clásico
            break;
    }

    ipv4_addr_t dst;
    //memset(&dst, 0, sizeof(ipv4_addr_t));
    ipv4_str_addr(argv[1], dst);
    uint8_t protocol = atoi(argv[2]);
    log_trace("protocol: %d", protocol);

    ipv4_layer_t* layer = ipv4_open("ipv4_config_client.txt","ipv4_route_table_client.txt"); //Esto se puede psaasar por parametro
    
    if(layer ==NULL){
        fprintf(stderr, "ERROR en ipv4_open()");
    }
  


    /* Generar payload */
    unsigned char* payload = (unsigned char*) calloc(1, DEFAULT_PAYLOAD_LEN+200);
    int i;
    for (i=0; i<DEFAULT_PAYLOAD_LEN; i++) {
        payload[i] = (unsigned char) i;
    }  

    log_trace("Enviando %d bytes al Servidor IP:", DEFAULT_PAYLOAD_LEN);
    print_pkt(payload, DEFAULT_PAYLOAD_LEN, 0);

    //ipv4_route_table_print(layer->routing_table);
    
//Falla que cuando entra en ipv4_send cambia el valor de las rutas
    int err = ipv4_send(layer,dst,protocol, payload, DEFAULT_PAYLOAD_LEN);
    free(payload); 
    if (err == -1) {
        log_trace("ERROR en ipv4_send(), abortando");
        exit(-1);
    }   

    //int ipv4_recv(ipv4_layer_t * layer, uint8_t protocol,unsigned char buffer [], ipv4_addr_t sender, int buf_len,long int timeout);
    unsigned char buffer[IP_MTU];
    ipv4_addr_t sender;
    long int timeout = 2000;
    int len= ipv4_recv(layer,(uint8_t)TYPE_IP,buffer,sender,IP_MTU,timeout);
    if (len == -1) {
        log_trace("ERROR en ipv4_recv()");
    } else if (len == 0) {
        log_trace("ERROR: No hay respuesta del Servidor IPv4");
    }
    if(len>0){
        char src_ip_str[IPv4_STR_MAX_LENGTH];
        ipv4_addr_str( sender, src_ip_str ); 
        log_trace("Recibidos %d bytes del Servidor IP (%s)", len, src_ip_str);
        print_pkt(buffer, len, 0);
    }

    log_trace("Cerrando interfaz IP.\n");

    ipv4_close(layer);
    return 0;
}

