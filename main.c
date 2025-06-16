#include <stdio.h>
#include <stdlib.h>
#include "procesos_ventas.h"
#include "hilos_ventas.h"
#include "configuracion.h"

int main(void) {
    int opcion;
    do {
        printf("\n=== SIMULACIÓN DE VENTAS DE LIBROS ===\n");
        printf("1) Procesos  (%d procesos)\n", NUM_PROCESOS);
        printf("2) Hilos     (%d hilos)\n",   NUM_HILOS);
        printf("0) Salir\n");
        printf("Seleccione una opción: ");
        if (scanf("%d", &opcion) != 1) {
            fprintf(stderr, "Entrada inválida\n");
            exit(EXIT_FAILURE);
        }
        switch (opcion) {
            case 1:
                ejecutarProcesos();
                break;
            case 2:
                ejecutarHilos();
                break;
            case 0:
                printf("Saliendo...\n");
                break;
            default:
                printf("Opción inválida. Intente de nuevo.\n");
        }
    } while (opcion != 0);

    return 0;
}
