#ifndef _IPv4_ROUTE_TABLE_H
#define _IPv4_ROUTE_TABLE_H

#include "ipv4.h"

#include <stdio.h>

/* Esta estructura almacena la información básica sobre la ruta a una subred. 
 * Incluye la dirección y máscara de la subred destino, el nombre del interfaz
 * de salida, y la dirección IP del siguiente salto.
 *
 * Utilice los métodos 'ipv4_route_create()' e 'ipv4_route_free()' para crear
 * y liberar esta estrucutra. Adicionalmente debe completar la implementación
 * del método 'ipv4_route_lookup()'.
 * 
 * Probablemente para construir una tabla de rutas de un protocolo de
 * encaminamiento sea necesario añadir más campos a esta estructura, así como
 * modificar las funciones asociadas.
 */
typedef struct ipv4_route {
  ipv4_addr_t subnet_addr;
  ipv4_addr_t subnet_mask;
  char iface[IFACE_NAME_MAX_LENGTH];
  ipv4_addr_t gateway_addr;
} ipv4_route_t;


/* ipv4_route_t * ipv4_route_create
 * ( ipv4_addr_t subnet, ipv4_addr_t mask, char* iface, ipv4_addr_t gw );
 * 
 * DESCRIPCIÓN: 
 *   Esta función crea una ruta IPv4 con los parámetros especificados:
 *   dirección de subred, máscara, nombre de interfaz y dirección de siguiente
 *   salto.
 *
 *   Esta función reserva memoria para la estructura creada. Debe utilizar la
 *   función 'ipv4_route_free()' para liberar dicha memoria.
 *
 * PARÁMETROS:
 *   'subnet': Dirección IPv4 de la subred destino de la nueva ruta.
 *     'mask': Máscara de la subred destino de la nueva ruta.
 *    'iface': Nombre del interfaz empleado para llegar a la subred destino de
 *             la nueva  ruta. Debe tener una longitud máxima de 
 *             'IFACE_NAME_MAX_LENGTH' caracteres.
 *       'gw': Dirección IPv4 del encaminador empleado para llegar a la subred
 *             destino de la nueva ruta.
 *
 * VALOR DEVUELTO:
 *   La función devuelve un puntero a la ruta creada.
 * 
 * ERRORES:
 *   La función devuelve 'NULL' si no ha sido posible reservar memoria para
 *   crear la ruta.
 */
ipv4_route_t * ipv4_route_create
( ipv4_addr_t subnet, ipv4_addr_t mask, char* iface, ipv4_addr_t gw );


/* int ipv4_route_lookup ( ipv4_route_t * route, ipv4_addr_t addr );
 *
 * DESCRIPCIÓN:
 *   Esta función indica si la dirección IPv4 especificada pertence a la
 *   subred indicada. En ese caso devuelve la longitud de la máscara de la
 *   subred.
 *
 * ************************************************************************
 * * Esta función NO está implementada, debe implementarla usted para que *
 * * funcione correctamente la función 'ipv4_route_table_lookup()'.       *
 * ************************************************************************
 * 
 * PARÁMETROS:
 *   'route': Ruta a la subred que se quiere comprobar.
 *    'addr': Dirección IPv4 destino.
 *
 * VALOR DEVUELTO:
 *   Si la dirección IPv4 pertenece a la subred de la ruta especificada, debe
 *   devolver un número positivo que indica la longitud del prefijo de
 *   subred. Esto es, el número de bits a uno de la máscara de subred.
 *   La función devuelve '-1' si la dirección IPv4 no pertenece a la subred
 *   apuntada por la ruta especificada.
 */
int ipv4_route_lookup ( ipv4_route_t * route, ipv4_addr_t addr );


/* void ipv4_route_print ( ipv4_route_t * route );
 *
 * DESCRIPCIÓN:
 *   Esta función imprime la ruta especificada por la salida estándar.
 *
 * PARÁMETROS:
 *   'route': Ruta que se desea imprimir.
 */
void ipv4_route_print ( ipv4_route_t * route );


/* void ipv4_route_free ( ipv4_route_t * route );
 *
 * DESCRIPCIÓN:
 *   Esta función libera la memoria reservada para la ruta especificada, que
 *   ha sido creada con 'ipv4_route_create()'.
 *
 * PARÁMETROS:
 *   'route': Ruta cuya memoria se desea liberar.
 */
void ipv4_route_free ( ipv4_route_t * route );



/* Número de entradas máximo de la tabla de rutas IPv4 */
#define IPv4_ROUTE_TABLE_SIZE 256


/* Definción de la estructura opaca que modela una tabla de rutas IPv4.
 * Las entradas de la tabla de rutas están indexadas, y dicho índice puede
 * tener un valor entre 0 y 'IPv4_ROUTE_TABLE_SIZE - 1'. Esta
 * implementación no permite rutas duplicadas (e.g. la misma ruta con
 * diferentes distancias administrativas), así que antes de añadir una
 * nueva ruta debe comprobar que no existe previamente.
 *
 * Esta estructura nunca debe crearse directamente. En su lugar debe emplear
 * las funciones 'ipv4_route_table_create()' e 'ipv4_route_table_free()' para
 * crear y liberar dicha estructura, respectivamente.
 *
 * Una vez creada la tabla de rutas, utilice 'ipv4_route_table_get()' para
 * acceder a la ruta en una posición determinada. Además es posible añadir
 * ['ipv4_route_table_add()'] y borrar rutas ['ipv4_route_table_remove()'],
 * así como buscar una subred en particular ['ipv4_route_table_find()'].
 * 'ipv4_route_table_lookup()' es la función más importante de la tabla de
 * rutas ya que devuelve la ruta para llegar a la dirección IPv4 destino
 * especificada.
 * 
 * Adicionalmente, las funciones 'ipv4_route_table_read()',
 * 'ipv4_route_table_write()' y 'ipv4_route_table_print()' permiten,
 * respectivamente, leer/escribir la tabla de rutas de/a un fichero, e
 * imprimirla por la salida estándar.
 */
typedef struct ipv4_route_table ipv4_route_table_t;


/* ipv4_route_table_t * ipv4_route_table_create();
 * 
 * DESCRIPCIÓN: 
 *   Esta función crea una tabla de rutas IPv4 vacía.
 *
 *   Esta función reserva memoria para la tabla de rutas creada, para
 *   liberarla es necesario llamar a la función 'ipv4_route_table_free()'.
 *
 * VALOR DEVUELTO:
 *   La función devuelve un puntero a la tabla de rutas creada.
 *
 * ERRORES:
 *   La función devuelve 'NULL' si no ha sido posible reservar memoria para
 *   crear la tabla de rutas.
 */
ipv4_route_table_t * ipv4_route_table_create();


/* int ipv4_route_table_add ( ipv4_route_table_t * table, 
 *                            ipv4_route_t * route );
 * DESCRIPCIÓN: 
 *   Esta función añade la ruta especificada en la primera posición libre de
 *   la tabla de rutas.
 *
 * PARÁMETROS:
 *   'table': Tabla donde añadir la ruta especificada.
 *   'route': Ruta a añadir en la tabla de rutas.
 * 
 * VALOR DEVUELTO:
 *   La función devuelve el indice de la posición [0, IPv4_ROUTE_TABLE_SIZE-1]
 *   donde se ha añadido la ruta especificada.
 * 
 * ERRORES:
 *   La función devuelve '-1' si no ha sido posible añadir la ruta
 *   especificada.
 */
int ipv4_route_table_add ( ipv4_route_table_t * table, ipv4_route_t * route );


/* ipv4_route_t * ipv4_route_table_remove ( ipv4_route_table_t * table, 
 *                                          int index );
 *
 * DESCRIPCIÓN:
 *   Esta función borra la ruta almacenada en la posición de la tabla de rutas
 *   especificada.
 *   
 *   Esta función NO libera la memoria reservada para la ruta borrada. Para
 *   ello es necesario utilizar la función 'ipv4_route_free()' con la ruta
 *   devuelta.
 *
 * PARÁMETROS:
 *   'table': Tabla de rutas de la que se desea borrar una ruta.
 *   'index': Índice de la ruta a borrar. Debe tener un valor comprendido
 *            entre [0, IPv4_ROUTE_TABLE_SIZE-1].
 * 
 * VALOR DEVUELTO:
 *   Esta función devuelve la ruta que estaba almacenada en la posición
 *   indicada antes de ser borrada.
 *
 * ERRORES:
 *   Esta función devuelve 'NULL' si la ruta no ha podido ser borrada, o no
 *   existía ninguna ruta en dicha posición.
 */
ipv4_route_t * ipv4_route_table_remove ( ipv4_route_table_t * table, int index );


/* ipv4_route_t * ipv4_route_table_lookup ( ipv4_route_table_t * table, 
 *                                          ipv4_addr_t addr );
 * 
 * DESCRIPCIÓN:
 *   Esta función devuelve la mejor ruta almacenada en la tabla de rutas para
 *   alcanzar la dirección IPv4 destino especificada.
 *
 *   Esta función recorre toda la tabla de rutas buscando rutas que contengan
 *   a la dirección IPv4 indicada. Para ello emplea la función
 *   'ipv4_route_lookup()'. De todas las rutas posibles se devuelve aquella
 *   con el prefijo más específico, esto es, aquella con la máscara de subred
 *   mayor.
 * 
 * PARÁMETROS:
 *   'table': Tabla de rutas en la que buscar la dirección IPv4 destino.
 *    'addr': Dirección IPv4 destino a buscar.
 *
 * VALOR DEVUELTO:
 *   Esta función devuelve la ruta más específica para llegar a la dirección
 *   IPv4 indicada.
 *
 * ERRORES:
 *   Esta función devuelve 'NULL' si no no existe ninguna ruta para alcanzar
 *   la dirección indicada, o si no ha sido posible realizar la búsqueda.
 */
ipv4_route_t * ipv4_route_table_lookup ( ipv4_route_table_t * table, 
                                         ipv4_addr_t addr );


/* ipv4_route_t * ipv4_route_table_get ( ipv4_route_table_t * table, int index );
 * 
 * DESCRIPCIÓN:
 *   Esta función devuelve la ruta almacenada en la posición de la tabla de
 *   rutas especificada.
 *
 * PARÁMETROS:
 *   'table': Tabla de rutas de la que se desea obtener una ruta.
 *   'index': Índice de la ruta consultada. Debe tener un valor comprendido
 *            entre [0, IPv4_ROUTE_TABLE_SIZE-1].
 * 
 * VALOR DEVUELTO:
 *   Esta función devuelve la ruta almacenada en la posición de la tabla de
 *   rutas indicada.
 *
 * ERRORES:
 *   Esta función devuelve 'NULL' no existe ninguna ruta en dicha posición, o
 *   si no ha sido posible consultar la tabla de rutas.
 */
ipv4_route_t * ipv4_route_table_get ( ipv4_route_table_t * table, int index );


/* int ipv4_route_table_find ( ipv4_route_table_t * table, ipv4_addr_t subnet, 
 *                                                         ipv4_addr_t mask );
 *
 * DESCRIPCIÓN:
 *   Esta función devuelve el índice de la ruta para llegar a la subred
 *   especificada.
 *
 * PARÁMETROS:
 *    'table': Tabla de rutas en la que buscar la subred.
 *   'subnet': Dirección de la subred a buscar.
 *     'mask': Máscara de la subred a buscar.
 * 
 * VALOR DEVUELTO:
 *   Esta función devuelve la posición de la tabla de rutas donde se encuentra
 *   la ruta que apunta a la subred especificada.
 *
 * ERRORES:
 *   La función devuelve '-1' si no se ha encontrado la ruta especificada o
 *   '-2' si no ha sido posible realizar la búsqueda.
 */
int ipv4_route_table_find
( ipv4_route_table_t * table, ipv4_addr_t subnet, ipv4_addr_t mask );


/* void ipv4_route_table_free ( ipv4_route_table_t * table );
 *
 * DESCRIPCIÓN:
 *   Esta función libera la memoria reservada para la tabla de rutas
 *   especificada, incluyendo todas las rutas almacenadas en la misma,
 *   mediante la función 'ipv4_route_free()'.
 *
 * PARÁMETROS:
 *   'table': Tabla de rutas a borrar.
 */
void ipv4_route_table_free ( ipv4_route_table_t * table );


/* int ipv4_route_table_read ( char * filename, ipv4_route_table_t * table );
 *
 * DESCRIPCIÓN:
 *   Esta función lee el fichero especificado y añade las rutas IPv4
 *   estáticas leídas en la tabla de rutas indicada.
 *
 * PARÁMETROS:
 *   'filename': Nombre del fichero con rutas IPv4 que se desea leer.
 *      'table': Tabla de rutas donde añadir las rutas leidas.
 *
 * VALOR DEVUELTO:
 *   La función devuelve el número de rutas leidas y añadidas en la tabla, o
 *   '0' si no se ha leido ninguna ruta.
 *
 * ERRORES:
 *   La función devuelve '-1' si se ha producido algún error al leer el
 *   fichero de rutas.
 */
int ipv4_route_table_read ( char * filename, ipv4_route_table_t * table );


/* void ipv4_route_table_print ( ipv4_route_table_t * table );
 *
 * DESCRIPCIÓN:
 *   Esta función imprime por la salida estándar la tabla de rutas IPv4
 *   especificada.
 *
 * PARÁMETROS:
 *      'table': Tabla de rutas a imprimir.
 */
void ipv4_route_table_print ( ipv4_route_table_t * table );


/* int ipv4_route_table_write ( ipv4_route_table_t * table, char * filename );
 *
 * DESCRIPCIÓN:
 *   Esta función almacena en el fichero especificado la tabla de rutas IPv4
 *   indicada.
 *
 * PARÁMETROS:
 *      'table': Tabla de rutas a almacenar.
 *   'filename': Nombre del fichero donde se desea almacenar la tabla de
 *               rutas.
 *
 * VALOR DEVUELTO:
 *   La función devuelve el número de rutas almacenadas en el fichero de
 *   rutas, o '0' si la tabla de rutas estaba vacia.
 *
 * ERRORES:
 *   La función devuelve '-1' si se ha producido algún error al escribir el
 *   fichero de rutas.
 */
int ipv4_route_table_write ( ipv4_route_table_t * table, char * filename );


#endif /* _IPv4_ROUTE_TABLE_H */
