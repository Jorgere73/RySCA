#ifndef HEADER_H
#define HEADER_H

#include <stdint.h>
#include "eth.h"

#define IPv4_ADDR_SIZE 4
#define IFACE_NAME_MAX_LENGTH 32
#define IPv4_ROUTE_TABLE_SIZE 216
#define IPv4_STR_MAX_LENGTH 16
#define MTU_IP 1480 //1500 -20 cabecera
#define HEADER_LEN_IP 20
#define VERSION_HEADERLEN 0x45
#define FLAGS_FO 0x0040
/* Logitud máxima del nombre de un interfaz de red */


typedef unsigned char ipv4_addr_t [IPv4_ADDR_SIZE];
typedef struct ipv4_layer ipv4_layer_t;
typedef struct ipv4_route ipv4_route_t;
typedef struct ipv4_route_table ipv4_route_table_t;
typedef struct ipv4_frame ipv4_frame;


struct ipv4_route {//es una fila de la tabla
  ipv4_addr_t subnet_addr;
  ipv4_addr_t subnet_mask;
  char iface[IFACE_NAME_MAX_LENGTH];
  ipv4_addr_t gateway_addr;
};

struct ipv4_route_table {
  ipv4_route_t * routes[IPv4_ROUTE_TABLE_SIZE];
};

struct ipv4_layer {
eth_iface_t * iface;  /*iface=eth_open("eth1"); */
ipv4_addr_t addr; /* 192.168.1.1 */
ipv4_addr_t netmask; /* 255.255.255.0 */
ipv4_route_table_t * routing_table;
}; 

struct ipv4_frame
{
  uint8_t version_headerLen;
  uint8_t dscp;
  uint16_t total_length;
  uint16_t identification;
  uint16_t flags_fragmentOffset;
  uint8_t ttl;
  uint8_t protocol;
  uint16_t checksum;
  ipv4_addr_t src_ip;
  ipv4_addr_t dst_ip;
  unsigned char payload[200];
};


#endif 