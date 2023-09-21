#include "ipv4_config.h"

#include <stdio.h>
#include <errno.h>
#include <string.h>

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
( char* filename, char ifname[], ipv4_addr_t addr, ipv4_addr_t netmask )
{
  int err = 0;

  /* Open IPv4 Configuration file */
  FILE* conf_file = fopen(filename, "r");
  if (conf_file == NULL) {
    fprintf(stderr, "Error opening IPv4 Configuration file '%s': %s.\n",
            filename, strerror(errno));
    return -1;
  }
  
  int ifname_read = 0;
  int addr_read = 0;
  int netmask_read = 0;

  /* Init output parameters, just in case */
  ifname[0] = '\0';
  memset(addr, 0x00, IPv4_ADDR_SIZE);
  memset(netmask, 0x00, IPv4_ADDR_SIZE);

  int linenum = 0;
  char line_buf[1024];
  char name_str[256];
  char value_str[256];

  while ((! feof(conf_file)) && (err==0)) {

    linenum++;

    /* Read next line of config file */
    char* line = fgets(line_buf, 1024, conf_file);
    if (line == NULL) {
      break;
    }

    /* If this line is empty or a comment, just ignore it */
    if ((line_buf[0] == '\n') || (line_buf[0] == '#')) {
      err = 0;
      continue;
    }

    /* Parse line: Format "<var> <value>\n" */
    err = sscanf(line, "%s %s\n", name_str, value_str);
    if (err != 2) {
      fprintf(stderr, "%s:%d: Invalid IPv4 Configuration file format.\n", 
              filename, linenum);
      fprintf(stderr, "%s:%d: Format must be: <var> <value>\n",
              filename, linenum);              
      err = -1;

    } else {

      /* Parse read name/value pair */
      if (strcasecmp(name_str, "Interface") == 0) {
        strcpy(ifname, value_str);
        ifname_read = 1;
        err = 0;
      } else if (strcasecmp(name_str, "IPv4Address") == 0) {
        err = ipv4_str_addr(value_str, addr);
        if (err != 0) {
          fprintf(stderr, "%s:%d: Invalid 'IPv4Address' value: '%s'\n", 
                  filename, linenum, value_str);
        } else {
          addr_read = 1;
        }
      } else if (strcasecmp(name_str, "SubnetMask") == 0) {
        err = ipv4_str_addr(value_str, netmask);
        if (err != 0) {
          fprintf(stderr, "%s:%d: Invalid 'SubnetMask' value: '%s'\n",
                  filename, linenum, value_str);
        } else {
          netmask_read = 1;
        }
      } else {
        fprintf(stderr, "%s:%d: Unknown variable: '%s'\n", 
                filename, linenum, name_str);
        err = -1;
      }
    }  
  }

  if (err == 0) {
    if (ifname_read == 0) {
      fprintf(stderr, "%s: Missing 'Interface' value\n", filename);
      err = -1;
    }
    if (addr_read == 0) {
      fprintf(stderr, "%s: Missing 'IPv4Address' value\n", filename);
      err = -1;
    }
    if (netmask_read == 0) {
      fprintf(stderr, "%s: Missing 'SubnetMask' value\n", filename);
      err = -1;
    }
  }

  /* Close IPv4 Configuration file */
  fclose(conf_file);

  return err;
}
