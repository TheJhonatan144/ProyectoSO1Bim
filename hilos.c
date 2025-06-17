// hilos.c
// Versión multihilo: pthread + mutex para simular ventas de libros

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include "config.h"
#include "hilos.h"

// Subtotales por grupo A–H
static int subtotales[NUM_GRUPOS];
// Mutex para proteger el arreglo de subtotales
static pthread_mutex_t mutex_subtotales;

// Tiempo monotónico de alta resolución en segundos
static double tiempo_actual() {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec + ts.tv_nsec / 1e9;
}

// Función que ejecuta cada hilo
static void* procesar_por_hilo(void *arg) {
    int id = *(int*)arg;
    int ventas_por_grupo = VENTAS_TOTALES / NUM_GRUPOS;
    int subtotal = 0;
    for (int i = 0; i < ventas_por_grupo; i++) {
        // si el índice es par resta 8, si impar suma 15
        subtotal += (i % 2 == 0) ? -8 : +15;
    }
    pthread_mutex_lock(&mutex_subtotales);
      subtotales[id] = subtotal;
    pthread_mutex_unlock(&mutex_subtotales);
    return NULL;
}

void ejecutar_hilos(void) {
    pthread_t hilos[NUM_GRUPOS];
    int indices[NUM_GRUPOS];
    pthread_mutex_init(&mutex_subtotales, NULL);

    // Iniciar reporte
    FILE *f = fopen("reporte_hilos.txt","w");
    if (!f) { perror("fopen hilos"); exit(EXIT_FAILURE); }
    fprintf(f, "SIMULACIÓN VENTAS DE LIBROS (Hilos)\n");
    fprintf(f, "Grupos A–H | Total: %d ventas\n\n", VENTAS_TOTALES);
    fprintf(f, "Grupo\tSubtotal\n");
    fclose(f);

    double t_ini = tiempo_actual();

    // Crear hilos
    for (int g = 0; g < NUM_GRUPOS; g++) {
        indices[g] = g;
        if (pthread_create(&hilos[g], NULL,
                           procesar_por_hilo, &indices[g]) != 0)
        {
            perror("pthread_create");
            exit(EXIT_FAILURE);
        }
    }
    // Esperar finalización
    for (int g = 0; g < NUM_GRUPOS; g++)
        pthread_join(hilos[g], NULL);

    double t_fin = tiempo_actual();

    // Guardar subtotales y resumen
    f = fopen("reporte_hilos.txt","a");
    for (int g = 0; g < NUM_GRUPOS; g++) {
        fprintf(f, " %c\t%d\n", 'A'+g, subtotales[g]);
    }
    fprintf(f, "\nNúmero de grupos: %d\n", NUM_GRUPOS);
    int total_vendido = 0;
    for (int g = 0; g < NUM_GRUPOS; g++)
        total_vendido += subtotales[g];
    fprintf(f, "Total vendido: %d\n", total_vendido);
    fprintf(f, "Total transacciones: %d\n", VENTAS_TOTALES);
    fprintf(f, "Tiempo de ejecución: %.6f segundos\n",
            t_fin - t_ini);
    fclose(f);

    pthread_mutex_destroy(&mutex_subtotales);
}

// Punto de entrada
int main(void) {
    ejecutar_hilos();
    return 0;
}
