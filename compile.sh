#!/bin/bash

rawnetcc /tmp/ipv4_client arp.c eth.c ipv4.c ipv4_route_table.c ipv4_config.c ipv4_client.c log.c


rawnetcc /tmp/ipv4_server arp.c eth.c ipv4.c ipv4_route_table.c ipv4_config.c ipv4_server.c log.c



#rawnetcc udp_client arp.c eth.c ipv4.c ipv4_route_table.c ipv4_config.c log.c udp.c udp_client.c



#scp -r ~/Uni/RySCA/entregasGrupo/ pi@192.168.1.96:~/Uni/RySCA/
