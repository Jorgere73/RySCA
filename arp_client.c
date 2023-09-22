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
    eth_iface_t * iface= eth_open(argv[1]);
    ipv4_addr_t ip_addr;
    ipv4_str_addr(argv[2],ip_addr);
    mac_addr_t mac;//aqui es donde vamos a almacenar la MAC DEST(que buscamos)
    arp_resolve(iface, ip_addr, mac);
    
    
    
}
