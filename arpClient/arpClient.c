#include "arp.h"
#include "eth.h"
#include "ipv4.h"
#include <stdlib.h>
//sigue dando error

int main(int argc, char* argv[])
{
    eth_iface_t * interfaz = eth_open(argv[0]);
    mac_addr_t macpropio;
    ipv4_addr_t ipdestino;
    int a = ipv4_str_addr(argv[1], ipdestino);
    int result = arp_resolve(interfaz, argv[1], macpropio);
    return 0;
}