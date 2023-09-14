#include "arp.h"
#include "eth.h"
#include "ipv4.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <libgen.h>
#include <string.h>
#include <rawnet.h>
#include "timerms.h"


#define ARP_HEADER_SIZE 16


struct arp_frame{

    uint16_t hardware_type;
    uint16_t type;
    uint8_t hardware_size;
    uint8_t protocol_size;
    uint16_t opcode;//00 01 para request y 00 02 para reply
    mac_addr_t dest_addr;
    ipv4_addr_t dest_ip;
    mac_addr_t src_addr;//
    ipv4_addr_t src_ip;
};



int arp_resolve(eth_iface_t * iface,ipv4_addr_t ip_addr, mac_addr_t mac_addr){
    struct arp_frame arp;
    //OBTENER MAC E IP PROPIA
    mac_addr_t macPropia ;
    eth_getaddr(iface, macPropia);
    char mac_str[MAC_ADDR_SIZE];
    mac_addr_str(macPropia,mac_str);
    printf("MAC propia %s", mac_str);
    printf("\n");

    printf("INTERFAZ %s",eth_getname(iface) );
    printf("\n");
    char ip_str[IPv4_STR_MAX_LENGTH];
    ipv4_addr_str(ip_addr,ip_str);
    printf("IP: %s",  ip_str);
    printf("\n");


    memcpy(arp.dest_addr,mac_addr, MAC_ADDR_SIZE);
    memcpy(arp.dest_ip ,ip_addr, IPv4_ADDR_SIZE);
    memcpy(arp.src_addr,macPropia, MAC_ADDR_SIZE);
    memcpy(arp.src_ip,macPropia, IPv4_ADDR_SIZE);
    arp.hardware_type = 0x0001;
    arp.type = 0x0800;
    arp.hardware_size = 0x06;
    arp.protocol_size = 0x04;
    arp.opcode = 0x0001;
    



    


    return 0;


    
}



