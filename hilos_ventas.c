/*
  Hilo informático:
    • Unidad de ejecución dentro de un proceso.
    • Permiten ejecutar tareas concurrentes en el mismo programa.
    • Son la base de la multitarea en sistemas operativos.
*/

#include "hilos_ventas.h"
#include "configuracion.h"
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <unistd.h>

// Mutex para proteger la variable 'total'
static pthread_mutex_t cerrojo;
static double total = 0.0;

// ---------------------------------------------
// Función de ayuda: diff de reloj
// ---------------------------------------------
static void calcularTiempo(struct timespec *ini, struct timespec *fin, struct timespec *delta) {
    delta->tv_sec  = fin->tv_sec  - ini->tv_sec;
    delta->tv_nsec = fin->tv_nsec - ini->tv_nsec;
    if (delta->tv_nsec < 0) {
        delta->tv_sec--;
        delta->tv_nsec += 1000000000L;
    }
}

// ---------------------------------------------
// Rutina que ejecuta cada hilo: calcula su parte
// de ventas y lo suma al total protegido por mutex.
// ---------------------------------------------
static void* trabajador(void* arg) {
    int indice = *(int*)arg;
    int base = VENTAS_TOTALES / NUM_HILOS;
    int extra = (indice < VENTAS_TOTALES % NUM_HILOS) ? 1 : 0;
    double suma = 0;
    srand(time(NULL) ^ (unsigned long)pthread_self());
    for (int i = 0; i < base + extra; i++) {
        int precio = rand() % 91 + 10;
        suma += precio;
    }
    // Sumar al total global de forma segura
    pthread_mutex_lock(&cerrojo);
      total += suma;
    pthread_mutex_unlock(&cerrojo);
    return NULL;
}

// ---------------------------------------------
// Función principal de hilos, llamada desde main
// ---------------------------------------------
void ejecutarHilos(void) {
    pthread_t hilos[NUM_HILOS];
    int indices[NUM_HILOS];
    struct timespec t_inicio, t_fin, t_delta;
    FILE *f = fopen("reporte_hilos.txt", "w");
    if (!f) {
        perror("Error abriendo reporte_hilos.txt");
        exit(EXIT_FAILURE);
    }

    pthread_mutex_init(&cerrojo, NULL);

    // Iniciar cronómetro
    clock_gettime(CLOCK_MONOTONIC, &t_inicio);

    // Lanzar hilos
    for (int i = 0; i < NUM_HILOS; i++) {
        indices[i] = i;
        if (pthread_create(&hilos[i], NULL, trabajador, &indices[i]) != 0) {
            perror("pthread_create");
            exit(EXIT_FAILURE);
        }
    }

    // Esperar a todos
    for (int i = 0; i < NUM_HILOS; i++) {
        pthread_join(hilos[i], NULL);
    }

    // Medir fin
    clock_gettime(CLOCK_MONOTONIC, &t_fin);
    calcularTiempo(&t_inicio, &t_fin, &t_delta);

    // Escribir reporte
    fprintf(f, "===== REPORTE DE HILOS =====\n");
    fprintf(f, "Hilos usados    : %d\n", NUM_HILOS);
    fprintf(f, "Ventas totales  : %.2f\n", total);
    fprintf(f, "Tiempo ejec.    : %ld.%09lds seg\n",
            t_delta.tv_sec, t_delta.tv_nsec);
    fclose(f);

    pthread_mutex_destroy(&cerrojo);

    // Mostrar en pantalla
    printf("Hilos   -> Total: %.2f | Tiempo: %ld.%09lds s\n",
           total, t_delta.tv_sec, t_delta.tv_nsec);
}
