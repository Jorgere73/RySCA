#include "arp.h"
#include "eth.h"
#include "ipv4.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <libgen.h>
#define IP_MTU 1480 //1500 -20 cabecera
 
//int ipv4_send (ipv4_layer_t * layer, ipv4_addr_t dst, uint8_t protocol,unsigned char * payload, int payload_len)

int main(int argc, char* argv[]){

	if(argc!=2){
		printf("ERROR\n");
		printf("					USO\n");
		printf("	ipv4_client.c <IP_destino> <protocolo>");
        return 1;
	}
    ipv4_layer_t* layer;
    layer = ipv4_open("ipv4_config_client.txt","ipv4_route_table_client.txt"); //Esto se puede psaasar por parametro
    if(layer ==NULL){
        fprintf(stderr, "ERROR en ipv4_open()");
    }
    
    ipv4_addr_t dst = argv[1];
    uint8_t protocol = argv[2];

    /* Generar payload */
    unsigned char payload[payload_len];
    int i;
    for (i=0; i<payload_len; i++) {
        payload[i] = (unsigned char) i;
    }  
    int payload_len = DEFAULT_PAYLOAD_LENGTH;

    printf("Enviando %d bytes al Servidor IP:\n", payload_len);
    print_pkt(payload, payload_len, 0);

    //int ipv4_send (ipv4_layer_t * layer, ipv4_addr_t dst, uint8_t protocol,unsigned char * payload, int payload_len)
    err = ipv4_send(layer,dst,protocol, payload, payload_len); 
    if (err == -1) {
        fprintf(stderr, "ERROR en ipv4_send()\n");
        exit(-1);
    }   
//int ipv4_recv(ipv4_layer_t * layer, uint8_t protocol,unsigned char buffer [], ipv4_addr_t sender, int buf_len,long int timeout)
    unsigned char buffer[IP_MTU]; //
    ipv4_addr_t sender;
    long int timeout = 2000;
    int len= ipv4_recv(layer,TYPE_IP,buffer,sender,IP_MTU,timeout);
    if (len == -1) {
        fprintf(stderr, " ERROR en ipv4_recv()\n");
    } else if (len == 0) {
        fprintf(stderr, " ERROR: No hay respuesta del Servidor IPv4\n");
    }
    if(len>0){
        char src_ip_str[IPv4_STR_MAX_LENGTH];
        ipv4_addr_str( sender, src_ip_str ); 
        printf("Recibidos %d bytes del Servidor IP (%s)\n", len, src_ip_str);
        print_pkt(buffer, len, 0);
    }

    printf("Cerrando interfaz IP.\n");

    ipv4_close(layer);
    return 0;
    
    
    
}
