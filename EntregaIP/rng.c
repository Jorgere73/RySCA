#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include <libgen.h>

int main(int argc, char* argv[])
{
    if (argc !=2) {
      printf("Usage: %s <num>\n", basename(argv[0]));
      printf("    <num>: Cantidad de números aleatorios a generar\n");
      exit(1);
    }
    int nums = atoi(argv[1]);

    /* Inicializar semilla para rand() */
    unsigned int seed = time(NULL);
    srand(seed);

    int i;
    for (i=0; i<nums; i++) {
        /* Generar número aleatorio entre 0 y RAND_MAX */
	int dice = rand();
	/* Número entero aleatorio entre 1 y 10 */
	dice = 1 + (int) (10.0 * dice / (RAND_MAX));
	printf("%i\n", dice);
    }
    return 0;
}
