// hilos.c
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include "config.h"

static const char *grupos[NUM_GRUPOS] = {"A","B","C","D","E","F","G","H"};
static double parciales[NUM_GRUPOS];
static pthread_mutex_t mutex;

// Resta de tiempos b – a
static void resta_tiempo(const struct timespec *a,
                         const struct timespec *b,
                         struct timespec *delta) {
    delta->tv_sec  = b->tv_sec  - a->tv_sec;
    delta->tv_nsec = b->tv_nsec - a->tv_nsec;
    if (delta->tv_nsec < 0) {
        delta->tv_sec--;
        delta->tv_nsec += 1000000000L;
    }
}

// Rutina de cada hilo: simula ventas y guarda subtotal
static void* simulador(void *arg) {
    int idx = *(int*)arg;
    int ventas = TOTAL_VENTAS / NUM_GRUPOS;
    srand(time(NULL) ^ (unsigned long)pthread_self());
    double suma = 0;
    for (int j=0; j<ventas; j++)
        suma += rand()%91 + 10;

    // Protege el acceso al arreglo compartido
    pthread_mutex_lock(&mutex);
      parciales[idx] = suma;
    pthread_mutex_unlock(&mutex);
    return NULL;
}

int main(void) {
    pthread_t hilos[NUM_GRUPOS];
    int indices[NUM_GRUPOS];
    struct timespec t0, t1, dt;
    double total = 0.0;
    FILE *f = fopen("reporte_hilos.txt","w");
    if (!f) { perror("fopen"); exit(EXIT_FAILURE); }

    pthread_mutex_init(&mutex, NULL);

    // ● Medir inicio
    clock_gettime(CLOCK_MONOTONIC, &t0);
    fprintf(f, "=== REPORTE HILOS ===\nGrupo\tVentas\n");

    // ● Crear hilos
    for (int i=0; i<NUM_GRUPOS; i++) {
        indices[i] = i;
        if (pthread_create(&hilos[i], NULL, simulador, &indices[i]) != 0) {
            perror("pthread_create");
            exit(EXIT_FAILURE);
        }
    }

    // ● Esperar terminación
    for (int i=0; i<NUM_GRUPOS; i++)
        pthread_join(hilos[i], NULL);

    // ● Recoger subtotales
    for (int i=0; i<NUM_GRUPOS; i++) {
        fprintf(f, "%s\t%.2f\n", grupos[i], parciales[i]);
        total += parciales[i];
    }

    // ● Medir fin y duración
    clock_gettime(CLOCK_MONOTONIC, &t1);
    resta_tiempo(&t0, &t1, &dt);
    fprintf(f, "\nTotal vendido: %.2f\nDuración: %ld.%09ld s\n",
            total, dt.tv_sec, dt.tv_nsec);
    printf("Hilos   -> Total: %.2f | Tiempo: %ld.%09ld s\n",
           total, dt.tv_sec, dt.tv_nsec);

    fclose(f);
    pthread_mutex_destroy(&mutex);
    return 0;
}
