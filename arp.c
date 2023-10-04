#include "arp.h"
#include "eth.h"
#include "log.h"
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

int arp_resolve(eth_iface_t *iface, ipv4_addr_t ip_addr, mac_addr_t* mac_addr)
{
    struct arp_frame arp;
    memset(&arp, 0, sizeof(struct arp_frame ));
    // OBTENER MAC E IP PROPIA
    mac_addr_t macPropia;
    eth_getaddr(iface, macPropia);
    char mac_str[MAC_STR_LENGTH];
    mac_addr_str(macPropia, mac_str);
    log_trace("MAC propia %s", mac_str);

    //char* ipPropiaStr = "0.0.0.0";
    //ipv4_addr_t ipPropia;
    //ipv4_str_addr(ipPropiaStr,ipPropia);

    log_trace("INTERFAZ %s", eth_getname(iface));
    char ip_str[IPv4_STR_MAX_LENGTH];
    ipv4_addr_str(ip_addr, ip_str);
    log_trace("IP: %s", ip_str);
    memcpy(arp.dest_addr, *mac_addr, MAC_ADDR_SIZE);
    memcpy(arp.dest_ip, ip_addr, IPv4_ADDR_SIZE);
    memcpy(arp.src_addr, macPropia, MAC_ADDR_SIZE);
    arp.hardware_type = htons(HARDWARE_TYPE);
    arp.type = htons(TYPE_IP);
    arp.hardware_size = HARDWARE_SIZE;
    arp.protocol_size = PROTOCOL_SIZE;
    arp.opcode = htons(OPCODE_REQUEST);

    mac_addr_t MAC_FF = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};

    // Convertimos la estructura arp a una cadena de caracteres


    // Enviamos arp request
    int a = eth_send(iface, MAC_FF, TYPE_ARP, (unsigned char *) &arp, sizeof(struct arp_frame));
    if (a < 0)
    {
        log_trace("Ha ocurrido un error, compruebe la MAC de origen\n");
    }
    else if (a > 0)
    {
        log_trace("Número de bytes enviados: %d\n", a);
        log_trace("Esto es lo que enviamos en str: \n %s", (unsigned char *) &arp);
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
    int count = 2;
    do{
        log_trace("%d", count);
        //long int time_left = timerms_left(&timer);//Vemos cuanto tiempo le queda al timer para expirar
        int b = eth_recv(iface, macPropia, TYPE_ARP, (unsigned char*) &arp_reply, sizeof(struct arp_frame), timeout);
   
        if (b == -1)
        {
            log_trace("Ha ocurrido un error");
            return -1;
        }
        else
        {
            log_trace("Número de bytes recibidos: %d", b);
            log_trace("ARP Reply: ");
            for (int i = 0; i < sizeof(struct arp_frame); i++) {
                log_trace("%02x ", ((unsigned char *)&arp_reply)[i]);
            }
            printf("\n");
            //continue -> sobra
        }
        isIP = (memcmp(arp.dest_ip,arp_reply.src_ip,IPv4_ADDR_SIZE)==0); //Miramos si la ip que nos pasan por parametro es igual a la que nos llega en el reply
        isReply = (ntohs(arp_reply.opcode) == OPCODE_REPLY);//Miramos si el opcode que nos llega en el reply es realmente de reply y no de otra cosa
        count--;

    }while(!(isIP && isReply) && count > 0); 

    if(count <= 0)
    {
        log_trace("No se ha encontrado la MAC destino");
        return -1;
    }
    
    //mac_addr = arp_reply.src_addr;
    memcpy(mac_addr, arp_reply.src_addr, sizeof(mac_addr_t));

    char mac_received_str[MAC_STR_LENGTH];
    mac_addr_str(arp_reply.src_addr, mac_received_str);
    log_trace("MAC encontrada en la respuesta ARP: %s", mac_received_str);
    return 0;
}
