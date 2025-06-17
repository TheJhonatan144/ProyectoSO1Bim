// procesos.c
// Versión por procesos: fork() + semáforo para simular ventas de libros

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <semaphore.h>
#include <time.h>
#include "config.h"
#include "procesos.h"

// Memoria compartida
static int *saldo_global;
static int *num_transacciones;
static sem_t *sem_mutex;

// Tiempo monotónico de alta resolución en segundos
static double tiempo_actual() {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec + ts.tv_nsec / 1e9;
}

// Lógica que ejecuta cada proceso hijo
static void procesar_por_proceso(int id_grupo) {
    FILE *f = fopen("reporte_procesos.txt","a");
    if (!f) { perror("fopen hijo"); exit(EXIT_FAILURE); }

    int ventas_por_grupo = VENTAS_TOTALES / NUM_GRUPOS;
    for (int i = 0; i < ventas_por_grupo; i++) {
        sem_wait(sem_mutex);
        if (*num_transacciones >= VENTAS_TOTALES) {
            sem_post(sem_mutex);
            break;
        }
        // Simula venta: si es par resta 8, si es impar suma 15
        if ((*num_transacciones % 2) == 0) {
            if (*saldo_global >= 8) *saldo_global -= 8;
        } else {
            *saldo_global += 15;
        }
        (*num_transacciones)++;
        sem_post(sem_mutex);
    }
    fprintf(f, "Grupo %c:\t%d\n", 'A'+id_grupo, *saldo_global);
    fclose(f);
}

void ejecutar_procesos(void) {
    // Reservar memoria compartida
    saldo_global = mmap(NULL,sizeof(int),
                        PROT_READ|PROT_WRITE,
                        MAP_SHARED|MAP_ANONYMOUS,
                        -1,0);
    num_transacciones = mmap(NULL,sizeof(int),
                             PROT_READ|PROT_WRITE,
                             MAP_SHARED|MAP_ANONYMOUS,
                             -1,0);
    sem_mutex = mmap(NULL,sizeof(sem_t),
                     PROT_READ|PROT_WRITE,
                     MAP_SHARED|MAP_ANONYMOUS,
                     -1,0);

    *saldo_global = 1000;
    *num_transacciones = 0;
    sem_init(sem_mutex,1,1);

    // Crear o truncar reporte
    FILE *f = fopen("reporte_procesos.txt","w");
    if (!f) { perror("fopen padre"); exit(EXIT_FAILURE); }
    fprintf(f, "SIMULACIÓN VENTAS DE LIBROS (Procesos)\n");
    fprintf(f, "Grupos A–H | Total: %d ventas\n\n", VENTAS_TOTALES);
    fprintf(f, "Grupo\tSaldo parcial\n");
    fclose(f);

    double t_ini = tiempo_actual();

    // Lanzar procesos hijos
    for (int g = 0; g < NUM_GRUPOS; g++) {
        pid_t pid = fork();
        if (pid < 0) { perror("fork"); exit(EXIT_FAILURE); }
        if (pid == 0) {
            procesar_por_proceso(g);
            munmap(saldo_global,sizeof(int));
            munmap(num_transacciones,sizeof(int));
            munmap(sem_mutex,sizeof(sem_t));
            exit(EXIT_SUCCESS);
        }
    }
    // Esperar hijos
    for (int i = 0; i < NUM_GRUPOS; i++) wait(NULL);

    double t_fin = tiempo_actual();

    // Resumen final
    f = fopen("reporte_procesos.txt","a");
    fprintf(f, "\nNúmero de grupos: %d\n", NUM_GRUPOS);
    fprintf(f, "Saldo final (total vendido): %d\n", *saldo_global);
    fprintf(f, "Total transacciones: %d\n", VENTAS_TOTALES);
    fprintf(f, "Tiempo de ejecución: %.6f segundos\n",
            t_fin - t_ini);
    fclose(f);

    sem_destroy(sem_mutex);
    munmap(saldo_global,sizeof(int));
    munmap(num_transacciones,sizeof(int));
    munmap(sem_mutex,sizeof(sem_t));
}

// Punto de entrada
int main(void) {
    ejecutar_procesos();
    return 0;
}
