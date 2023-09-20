#include "arp.h"
#include "eth.h"
#include "ipv4.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <libgen.h>
#include <string.h>
#include <rawnet.h>
#include <timerms.h>
#include <arpa/inet.h>

#define ARP_HEADER_SIZE 28
#define TYPE_IP 0x0806

struct arp_frame
{

    uint16_t hardware_type;
    uint16_t type;
    uint8_t hardware_size;
    uint8_t protocol_size;
    uint16_t opcode; // 00 01 para request y 00 02 para reply
    mac_addr_t src_addr; //
    ipv4_addr_t src_ip;
    mac_addr_t dest_addr;
    ipv4_addr_t dest_ip;
};

int arp_resolve(eth_iface_t *iface, ipv4_addr_t ip_addr, mac_addr_t mac_addr)
{
    struct arp_frame arp;
    // OBTENER MAC E IP PROPIA
    mac_addr_t macPropia;
    eth_getaddr(iface, macPropia);
    char mac_str[MAC_ADDR_SIZE];
    mac_addr_str(macPropia, mac_str);
    printf("MAC propia %s", mac_str);
    printf("\n");

    char* ipPropiaStr = "0.0.0.0";
    ipv4_addr_t ipPropia;
    ipv4_str_addr(ipPropiaStr,ipPropia);

    printf("INTERFAZ %s", eth_getname(iface));
    printf("\n");
    char ip_str[IPv4_STR_MAX_LENGTH];
    ipv4_addr_str(ip_addr, ip_str);
    printf("IP: %s", ip_str);
    printf("\n");

    memcpy(arp.dest_addr, mac_addr, MAC_ADDR_SIZE);
    memcpy(arp.dest_ip, ip_addr, IPv4_ADDR_SIZE);
    memcpy(arp.src_addr, macPropia, MAC_ADDR_SIZE);
    memcpy(arp.src_ip, ipPropia, IPv4_ADDR_SIZE);
    arp.hardware_type = htons(0x0001);
    arp.type = htons(0x0800);
    arp.hardware_size = 0x06;
    arp.protocol_size = 0x04;
    arp.opcode = htons(0x0001);
    printf("Aqui ya hemos inicializado la estructura arp\n");

    mac_addr_t MAC_FF = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};

    // Convertimos la estructura arp a una cadena de caracteres


    // Enviamos arp request
    int a = eth_send(iface, MAC_FF, TYPE_IP, (unsigned char *) &arp, sizeof(arp));
    if (a == -1)
    {
        printf("Ha ocurrido un error\n");
    }
    else
    {
        printf("Número de bytes enviados: %d\n", a);
        printf("Esto es lo que enviamos en str: \n %s", (unsigned char *) &arp);
    }

    // Recibimos el Reply
    unsigned char *arp_reply_str = NULL;
    long int timeout =2000;
    int b = eth_recv(iface, macPropia, TYPE_IP, arp_reply_str, sizeof(arp_reply_str), timeout);
printf("llega\n");
    if (b == -1)
    {
        printf("Ha ocurrido un error\n");
    }
    else
    {
        printf("Número de bytes recibidos: %d\n", b);
        printf("el arp reply str es: %s", arp_reply_str);
    }

    struct arp_frame arp_reply ;
    /*memcpy(&arp_reply, arp_reply_str, sizeof(arp_reply));*/


    mac_addr = arp_reply.src_addr;
printf("llega\n");
    char mac2print_str[MAC_ADDR_SIZE];
    mac_addr_str(mac_addr, mac2print_str);
    printf("MAC encontrada %s\n", mac2print_str);

    return 0;
}
