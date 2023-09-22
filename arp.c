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

/*
Obtener tiempo actual:
    long long int timerms_time();
(Re)iniciar temporizador:
    long long int timerms_reset ( timerms_t * timer,long int timeout );--Empieza el timer de timeout ms
        ‘timeout’ >= 0: ‘timer’ expira en ‘timeout’ ms
        ‘timeout’ < 0: Temporizador “infinito” 

Consultar estado temporizador:
    long int timerms_elapsed ( timerms_t * timer );--Para ver cuanto lleva el timer activado
    long int timerms_left ( timerms_t * timer );--Para ver cuanto tiempo de vida le queda al timer
*/

#define ARP_HEADER_SIZE 28
#define TYPE_ARP 0x0806
#define HARDWARE_TYPE 0x0001
#define TYPE_IP 0x0800
#define PROTOCOL_SIZE 0x04
#define OPCODE_REQUEST 0x0001
#define OPCODE_REPLY 0x0002
#define HARDWARE_SIZE 0x06

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
    memset(&arp, 0, sizeof(struct arp_frame ));
    // OBTENER MAC E IP PROPIA
    mac_addr_t macPropia;
    eth_getaddr(iface, macPropia);
    char mac_str[MAC_STR_LENGTH];
    mac_addr_str(macPropia, mac_str);
    printf("MAC propia %s", mac_str);
    printf("\n");

    //char* ipPropiaStr = "0.0.0.0";
    //ipv4_addr_t ipPropia;
    //ipv4_str_addr(ipPropiaStr,ipPropia);

    printf("INTERFAZ %s", eth_getname(iface));
    printf("\n");
    char ip_str[IPv4_STR_MAX_LENGTH];
    ipv4_addr_str(ip_addr, ip_str);
    printf("IP: %s", ip_str);
    printf("\n");

    memcpy(arp.dest_addr, mac_addr, MAC_ADDR_SIZE);
    memcpy(arp.dest_ip, ip_addr, IPv4_ADDR_SIZE);
    memcpy(arp.src_addr, macPropia, MAC_ADDR_SIZE);
    //memcpy(arp.src_ip, ipPropia, IPv4_ADDR_SIZE);
    arp.hardware_type = htons(HARDWARE_TYPE);
    arp.type = htons(TYPE_IP);
    arp.hardware_size = HARDWARE_SIZE;
    arp.protocol_size = PROTOCOL_SIZE;
    arp.opcode = htons(OPCODE_REQUEST);
    printf("Aqui ya hemos inicializado la estructura arp\n");

    mac_addr_t MAC_FF = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};

    // Convertimos la estructura arp a una cadena de caracteres


    // Enviamos arp request
    int a = eth_send(iface, MAC_FF, TYPE_ARP, (unsigned char *) &arp, sizeof(struct arp_frame));
    if (a < 0)
    {
        printf("Ha ocurrido un error\n");
    }
    else if (a > 0)
    {
        printf("Número de bytes enviados: %d\n", a);
        printf("Esto es lo que enviamos en str: \n %s", (unsigned char *) &arp);
    }

    // Recibimos el Reply
    struct arp_frame arp_reply;
    memset(&arp_reply, 0, sizeof(struct arp_frame ));
    long int timeout =2000;
     /* Inicializar temporizador para mantener timeout si se reciben tramas con
     tipo incorrecto. */
    timerms_t timer;
    timerms_reset(&timer, timeout); //timer expira en timeout ms

    int isIP; 
    //Boolean resultado, sirve para guardar la comparación entre el IP al que envíamos y el que nos reenvia la MAC de vuelta
    int isReply;
    //Boolean que guarda el estado de comparación entre el opcode de la respuesta arp con el código arp reply 
    memset(&isIP, 0, sizeof(int));
    memset(&isReply, 0, sizeof(int));
    //Declaramos e inicializamos las variables a 0

    do{
        //long int time_left = timerms_left(&timer);//Vemos cuanto tiempo le queda al timer para expirar
        int b = eth_recv(iface, macPropia, TYPE_ARP, (unsigned char*) &arp_reply, sizeof(struct arp_frame), timeout);
   
        if (b == -1)
        {
            printf("Ha ocurrido un error\n");
            return -1;
        }
        else
        {
            printf("Número de bytes recibidos: %d\n", b);
            printf("ARP Reply: ");
            for (int i = 0; i < sizeof(struct arp_frame); i++) {
                printf("%02x ", ((unsigned char *)&arp_reply)[i]);
            }
            printf("\n");
            //continue -> sobra
        }
        isIP = (memcmp(arp.dest_ip,arp_reply.src_ip,IPv4_ADDR_SIZE)==0); //Miramos si la ip que nos pasan por parametro es igual a la que nos llega en el reply
        isReply = (ntohs(arp_reply.opcode) == OPCODE_REPLY);//Miramos si el opcode que nos llega en el reply es realmente de reply y no de otra cosa
    }while(!(isIP && isReply)); 

    
    mac_addr = arp_reply.src_addr;

    char mac_received_str[MAC_STR_LENGTH];
    mac_addr_str(arp_reply.src_addr, mac_received_str);
    printf("MAC encontrada en la respuesta ARP: %s\n", mac_received_str);
    return 0;
}
