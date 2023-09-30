#define UDP_HEADER 8
#define TYPE_UDP 0x11
#define UDP_MTU 1472 //1500-20-8 
#include <stdint.h>
#include "ipv4.h"
#define DEFAULT_PAYLOAD_LEN 200

typedef struct udp_layer udp_layer_t;

udp_layer_t *udp_open(uint16_t puerto, char *ip_config, char *ip_route_config);

int udp_send(udp_layer_t *udp_layer,ipv4_addr_t ip_dest,uint16_t dst_port,unsigned char * payload, int payload_len);
int udp_recv(udp_layer_t* udp_layer,unsigned char buffer [], ipv4_addr_t sender, int buffer_len, long int timeout);
int udp_close(udp_layer_t * udp_layer);