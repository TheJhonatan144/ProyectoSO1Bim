#include "hilos_libros.h"
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <unistd.h>

#define GRUPOS 8
#define VENTAS_POR_GRUPO 1000

static double totales[GRUPOS];
static double total_general;
static pthread_mutex_t lock;

// Muestra un timestamp legible
static void mostrar_tiempo_actual(const char* mensaje) {
    time_t t = time(NULL);
    printf("%s: %s", mensaje, ctime(&t));
}

// Rutina de cada hilo: simula ventas para un grupo
static void* simular_ventas_libros(void* arg) {
    int idx = *(int*)arg;
    const char* grupos[GRUPOS] = {"A","B","C","D","E","F","G","H"};
    // Semilla mixta con el ID de hilo
    srand(time(NULL) ^ (unsigned long)pthread_self());
    double subtotal = 0;

    for (int i = 0; i < VENTAS_POR_GRUPO; i++) {
        int precio = rand() % 91 + 10;  // $10–$100
        subtotal += precio;

        // Protejo la salida por pantalla
        pthread_mutex_lock(&lock);
        printf("🔵 Hilo %lu - Grupo %s: Venta %d = $%d\n",
               (unsigned long)pthread_self(), grupos[idx], i+1, precio);
        pthread_mutex_unlock(&lock);
    }

    // Acumulo el subtotal en el total general
    pthread_mutex_lock(&lock);
    totales[idx] = subtotal;
    total_general += subtotal;
    pthread_mutex_unlock(&lock);

    return NULL;
}

// Función que main.c invoca para ejecutar la versión de hilos
void run_threads(void) {
    pthread_t hilos[GRUPOS];
    int indices[GRUPOS];
    struct timespec inicio, fin;

    // Inicializa el mutex
    pthread_mutex_init(&lock, NULL);

    // Marca inicio
    mostrar_tiempo_actual("⏱ Inicio (hilos)");
    clock_gettime(CLOCK_MONOTONIC, &inicio);

    // Crea todos los hilos
    for (int i = 0; i < GRUPOS; i++) {
        indices[i] = i;
        if (pthread_create(&hilos[i], NULL, simular_ventas_libros, &indices[i]) != 0) {
            perror("pthread_create");
            exit(EXIT_FAILURE);
        }
    }

    // Espera a que terminen
    for (int i = 0; i < GRUPOS; i++) {
        pthread_join(hilos[i], NULL);
    }

    // Marca fin
    clock_gettime(CLOCK_MONOTONIC, &fin);
    mostrar_tiempo_actual("⏱ Fin (hilos)");

    // Duración
    double dur = (fin.tv_sec - inicio.tv_sec)
               + (fin.tv_nsec - inicio.tv_nsec) / 1e9;
    printf("\n⏳ Duración total (hilos): %.3f segundos\n\n", dur);

    // Resumen por grupo
    printf("--- Resumen de Ventas por Grupo (Hilos) ---\n");
    for (int i = 0; i < GRUPOS; i++) {
        printf("Grupo %c: $%.2f\n", 'A' + i, totales[i]);
    }
    printf("Total general: $%.2f\n\n", total_general);

    // Limpia el mutex
    pthread_mutex_destroy(&lock);
}
