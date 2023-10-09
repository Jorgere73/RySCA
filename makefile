ipv4_client: ipv4.c ipv4.h arp.c arp.h ipv4_config.c ipv4_config.h ipv4_route_table.c ipv4_route_table.h ipv4_client.c eth.c eth.h header.h log.c log.h
	rawnetcc ipv4_client ipv4.c eth.c arp.c ipv4_config.c ipv4_route_table.c log.c ipv4_client.c
ipv4_server: ipv4_server