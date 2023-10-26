#include "arp.h"
#include "eth.h" 
#include "udp.h"
#include "ipv4.h"  

#include <unistd.h>
#include <libgen.h>
#include <string.h>
#include <rawnet.h>
#include <timerms.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
//¿DIFRENCIAS ENTRE udp_layer Y udp_frame?
typedef struct udp_layer {//lo q necesita ip para poder funcionar contiene todas las capas de abajo

    uint16_t src_port;
    ipv4_layer_t *ipv4_layer;//la capa Ipv4 que esta por debajo

} udp_layer_t;
struct udp_frame{//cabeceraUDP(src_port,dst_port,length_udp,checksum) + data
    uint16_t src_port; 
    uint16_t dst_port; 
    uint16_t length_udp;//sizeof(cabecera + data) 
    uint16_t checksum;//lo calculamos en la capa UDP y se verifica en el destino
    unsigned char payload[UDP_MTU];
} ;
//En el open inicializamos el layer que va a contener el puerto y un layer de ipv4 para abrir la capa IP
udp_layer_t *udp_open(uint16_t puerto, char *ip_config, char *ip_route_config){ 
    udp_layer_t *udp_layer = malloc(sizeof(udp_layer_t));
    udp_layer->ipv4_layer = ipv4_open(ip_config,ip_route_config);
    udp_layer->src_port = puerto;
    return udp_layer;
} 
int udp_close(udp_layer_t * udp_layer){
    //Cerramos la capa Ipv4
    ipv4_close(udp_layer->ipv4_layer);
    free(udp_layer);
    return 0;
    
} 

int udp_send(udp_layer_t *udp_layer,ipv4_addr_t ip_dest,uint16_t dst_port,unsigned char * payload, int payload_len){//logica de los puertos?hay q pensar hacia abajo---------------------------------------------------------------------------------

    struct udp_frame pkt_udp_send;
    memset(&pkt_udp_send, 0, sizeof(struct udp_frame)); //Limpiamos la estructura para que no haya basura

//INICIALIZAMOS LA ESTRUCTURA
    pkt_udp_send.src_port = htons(udp_layer->src_port);
    pkt_udp_send.dst_port= htons(dst_port);
    pkt_udp_send.length_udp=htons((payload_len+UDP_HEADER));
    pkt_udp_send.checksum =ipv4_checksum((unsigned char*) &pkt_udp_send,UDP_HEADER); //PQ SOLO HACEMOS EL CHECKSUM DE LA CABECERA?--------------------------------------------------------------------------------------------------------------------
    memcpy(pkt_udp_send.payload, payload, payload_len);

//Lanzar ip_send
    int bytes_udp = ipv4_send(udp_layer->ipv4_layer,ip_dest,TYPE_UDP,(unsigned char*)&pkt_udp_send, (payload_len+UDP_HEADER));
    if (bytes_udp < 0)
    {
        printf("Ha ocurrido un error en UDP\n");
        return -1;
        }
    else if (bytes_udp > 0)
    {
        printf("Número de bytes enviados en UDP: %d\n", bytes_udp);
        printf("Esto es lo que enviamos en str: \n %s", (unsigned char *) &pkt_udp_send);
        return 0;
    }


return 0;
} 


int udp_recv(udp_layer_t* udp_layer,unsigned char buffer [], ipv4_addr_t sender, int buffer_len, uint16_t *port, long int timeout){//logica de los puertos?


struct udp_frame pkt_udp_recv;

// Inicializar temporizador para mantener timeout si se reciben tramas con tipo incorrecto.
  timerms_t timer;
  timerms_reset(&timer, timeout);

  do{//para comprobar si recibimos lo q nosotros queremos
    long int time_left = timerms_left(&timer);
    int bytes_recibidos = ipv4_recv(udp_layer->ipv4_layer,TYPE_UDP,buffer,sender,buffer_len,time_left);
    if (bytes_recibidos == -1) {
            printf("Error al recibir el datagrama.\n");
            return -1;
    }
    pkt_udp_recv = (struct udp_frame*)buffer;//Ver si esto tiene sentido------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- 

  }while(!(pkt_udp_recv.dst_port== udp_layer->src_port));

  uint16_t puerto_recibido = ntohs(pkt_udp_recv.dst_port);
  memcpy(port,&puerto_recibido,sizeof(uint16_t));
  int payload_len = bytes_recibidos - UDP_HEADER;
  memcpy(buffer,pkt_ip_recv.payload,buffer_len);
  return payload_len;



  //Y ahora????
return 0;

}
