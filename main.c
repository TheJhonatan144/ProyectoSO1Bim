#include <stdio.h>
#include <stdlib.h>
#include "procesos_libros.h"
#include "hilos_libros.h"

int main(void) {
    int opc;
    do {
        printf("=== Simulación de Ventas de Libros ===\n");
        printf("1) Ejecutar con PROCESOS (fork)\n");
        printf("2) Ejecutar con HILOS (pthread)\n");
        printf("0) Salir\n");
        printf("Seleccione una opción: ");
        if (scanf("%d", &opc) != 1) break;

        switch (opc) {
            case 1: run_procesos();   break;
            case 2: run_threads();    break;
            case 0: printf("Saliendo...\n"); break;
            default: printf("Opción inválida.\n"); break;
        }
    } while (opc != 0);

    return 0;
}
