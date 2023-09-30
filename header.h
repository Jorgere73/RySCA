#ifndef HEADER_H
#define HEADER_H
#define IPv4_ADDR_SIZE 4
#define IFACE_NAME_MAX_LENGTH 32
#define IPv4_STR_MAX_LENGTH 16
#define IP_MTU 1480 //1500 -20 cabecera
#define HEADER_LEN_IP 20
#define VERSION_HEADERLEN 0x45
#define FLAGS_FO 0x0040
/* Logitud máxima del nombre de un interfaz de red */


typedef unsigned char ipv4_addr_t [IPv4_ADDR_SIZE];
typedef struct ipv4_layer ipv4_layer_t;
typedef struct ipv4_route ipv4_route_t;
typedef struct ipv4_route_table ipv4_route_table_t;


#endif 