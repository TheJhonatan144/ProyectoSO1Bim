#include "procesos_libros.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <time.h>

#define GRUPOS 8
#define VENTAS_POR_GRUPO 1000

// Función interna para imprimir el timestamp
static void mostrar_tiempo_actual(const char* mensaje) {
    time_t t = time(NULL);
    printf("%s: %s", mensaje, ctime(&t));
}

// Función interna que simula las ventas de un grupo
static void simular_ventas_libros(const char* grupo, int write_fd) {
    srand(time(NULL) ^ getpid());
    double total = 0;
    for (int i = 0; i < VENTAS_POR_GRUPO; i++) {
        int precio = rand() % 91 + 10;
        total += precio;
        printf("🟢 Proceso %d - Grupo %s: Venta %d = $%d\n",
               getpid(), grupo, i+1, precio);
    }
    write(write_fd, &total, sizeof(total));
}

// ¡Esta debe ser NO-STATIC y coincidir con el header!
void run_procesos(void) {
    const char* grupos[GRUPOS] = {"A","B","C","D","E","F","G","H"};
    int pipes_fd[GRUPOS][2];
    double totales[GRUPOS];
    struct timespec inicio, fin;

    for (int i = 0; i < GRUPOS; i++)
        if (pipe(pipes_fd[i]) < 0) { perror("pipe"); exit(EXIT_FAILURE); }

    mostrar_tiempo_actual("⏱ Inicio (procesos)");
    clock_gettime(CLOCK_MONOTONIC, &inicio);

    for (int i = 0; i < GRUPOS; i++) {
        pid_t pid = fork();
        if (pid < 0) { perror("fork"); exit(EXIT_FAILURE); }
        if (pid == 0) {
            close(pipes_fd[i][0]);
            simular_ventas_libros(grupos[i], pipes_fd[i][1]);
            close(pipes_fd[i][1]);
            exit(EXIT_SUCCESS);
        }
        close(pipes_fd[i][1]);
    }

    for (int i = 0; i < GRUPOS; i++) {
        wait(NULL);
        read(pipes_fd[i][0], &totales[i], sizeof(double));
        close(pipes_fd[i][0]);
    }

    clock_gettime(CLOCK_MONOTONIC, &fin);
    mostrar_tiempo_actual("⏱ Fin (procesos)");

    double dur = (fin.tv_sec - inicio.tv_sec)
               + (fin.tv_nsec - inicio.tv_nsec) / 1e9;
    printf("\n⏳ Duración total (procesos): %.3f segundos\n\n", dur);

    printf("--- Resumen de Ventas por Grupo (Procesos) ---\n");
    for (int i = 0; i < GRUPOS; i++)
        printf("Grupo %s: $%.2f\n", grupos[i], totales[i]);
    printf("\n");
}
