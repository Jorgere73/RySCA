#ifndef _IPv4_CONFIG_H
#define _IPv4_CONFIG_H

#include "ipv4.h"
#include <stdio.h>

/* int ipv4_config_read
 * ( char* filename, char ifname[], ipv4_addr_t addr, ipv4_addr_t netmask );
 *
 * DESCRIPCIÓN: 
 *   Esta función lee el fichero de configuración IPv4 especificado y devuelve
 *   el nombre del interfaz, la direccion IPv4 del mismo, y la máscara de
 *   subred.
 *
 *   La memoria del nombre del interfaz y de las direcciones IPv4 debe haber
 *   sido reservada previamente. Deben reservarse al menos 'IFACE_NAME_MAX_LENGTH'
 *   bytes para almacenar el nombre del interfaz.
 *
 * PARÁMETROS:
 *    'filename': Nombre del fichero de configuración que se desea leer.
 *      'ifname': Variable donde se copiará el nombre de la interfaz leida del
 *                fichero de configuración.
 *        'addr': Variable donde se copiará la dirección IPv4 del interfaz
 *                leida del fichero de configuración.
 *     'netmask': Variable donde se copiará la máscara de subred leida del
 *                fichero de configuración.
 *
 * VALOR DEVUELTO:
 *   La función devuelve '0' si el fichero de configuración se ha leido
 *   correctamente.
 *
 * ERRORES:
 *   La función devuelve '-1' si se ha producido algún error al leer el
 *   fichero de configuración.
 */
int ipv4_config_read
( char* filename, char ifname[], ipv4_addr_t addr, ipv4_addr_t netmask );


#endif /* _IPv4_CONFIG_H*/
