/*
 * Proyecto: Ejecución de Múltiples Tareas
 * Autores: Jhonatan Torres, Sabina Zabala, Sebastían Yánes y Geovanni 
 * Fecha: 17 de junio 2025
 *
 * Versión inicial: stub de main para integración con Git.
 */

#include <stdio.h>
#include <stdlin.h>
#include "procesos_libros.h"
#include "hilos_libros.h"

int main(void) {
    int opc;
    do{
        printf("===Simulacion de Ventas de Libros===\n");
        printf("1. Ejecutar con PROCESOS (fork)\n");
        printf("2. Ejecutar con HILOS (pthread)\n");
        printf("3. Salir\n");
        printf("Seleccione una opcion: ");
        if (scanf("%d", &opc) != 1) break;

        switch(opc){
            case 1:
                run_processes(); // Llamada a la función de procesos
                break;
            case 2:
                run_threads(); // Llamada a la función de hilos
                break;
            case 3:
                printf("Saliendo del programa...\n");
                break;
            default:
                printf("Opción no válida. Intente de nuevo.\n");
        }
    }while(opc != 0);

    return 0;
}
