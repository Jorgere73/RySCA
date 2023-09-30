#ifndef _ARP_H
#define _ARP_H
#include "eth.h"
#include "ipv4.h"

#define ARP_HEADER_SIZE 28
#define TYPE_ARP 0x0806
#define HARDWARE_TYPE 0x0001
#define TYPE_IP 0x0800
#define PROTOCOL_SIZE 0x04
#define OPCODE_REQUEST 0x0001
#define OPCODE_REPLY 0x0002
#define HARDWARE_SIZE 0x06

// void ipv4_addr_str(ipv4_addr_t addr, char str[]);
// int ipv4_str_addr(char* str, ipv4_addr_t addr);
int arp_resolve(eth_iface_t *iface, ipv4_addr_t dest, mac_addr_t* mac);



#endif