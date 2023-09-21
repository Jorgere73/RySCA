# RySCA
Entregas de grupo por carpetas

1. Cliente ARP
2. Algo relacionado con IP
----------------------------------------------
Inicializar todas las estructuras usando memset(&<estructura>, <valor a inicializar>, sizeof(<estructura>)); Así evitamos que contenga basura en memoria

Meter do-while al recibir, y crear un timer de 2s, que a medida que pasa el tiempo, va bajando. Cambiar el parámetro timeout del eth_recv() por el tiempo restante del temporizador 

#Problemas solucionados:

Dirección MAC de origen, de tamaño 16 estaba siendo guardada en una estructura de tamaño 6, no daba error y el código funcionaba mal
