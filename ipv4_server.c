#include <stdio.h>
#include "header.h"
#include "ipv4.h"
#include "log.h"


int main(int argc, char* argv[])
{
    ipv4_addr_t ipdst;
    ipv4_str_addr("192.168.1.127", ipdst);
    ipv4_layer_t* layer;
    layer = ipv4_open("ipv4_config_server.txt","ipv4_route_table_server.txt");
    ipv4_send(layer, ipdst,6, 0, 0);
}