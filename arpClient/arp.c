#include "arp.h"
#include <rawnet.h>
#include "ipv4.h"

struct arp_frame
{
    uint16_t hardware_type;
    uint16_t type;
    uint8_t hardware_size;
    uint8_t protocol_size;
    uint16_t opcode;
    mac_addr_t dest_addr;
    ipv4_addr_t dest_ip;
    mac_addr_t src_addr;
    ipv4_addr_t src_ip;
};


int arp_resolve ( eth_iface_t * iface, ipv4_addr_t dest, mac_addr_t mac )
{
    struct arp_frame envio;
    mac_addr_t macpropia;
    eth_getaddr(iface, macpropia);
}


