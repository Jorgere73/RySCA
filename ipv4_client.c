#include "arp.h"
#include "eth.h"
#include "ipv4.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <libgen.h>


int main(int argc, char* argv[]){

	if(argc!=3){
		printf("ERROR\n");
		printf("					USO\n");
		printf("	arp_client <nombre_interfaz> <IP_destino>");
        return 1;
	}
    ipv4_layer_t* layer;
    layer = ipv4_open("ipv4_config_client.txt","ipv4_route_table_client.txt");

    ipv4_addr_t dst; // = argv[x] 
    uint8_t protocol; //0x800?
    /* Generar payload */
    unsigned char payload[payload_len];
    int i;
    for (i=0; i<payload_len; i++) {
        payload[i] = (unsigned char) i;
    }  
    int payload_len = DEFAULT_PAYLOAD_LENGTH;
    ipv4_send(layer,dst,protocol, payload, payload_len); //probablemente haya cosas mal pero es un inicio de cliente
    ipv4_recv();
    ipv4_close();
    
    
    
}
