// procesos.c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <time.h>
#include "config.h"  

static const char *grupos[NUM_GRUPOS] = {"A","B","C","D","E","F","G","H"};

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

int main(void) {
    int tuberias[NUM_GRUPOS][2];
    struct timespec t0, t1, dt;
    double parcial, total = 0.0;
    FILE *f = fopen("reporte_procesos.txt","w");
    if (!f) { perror("fopen"); exit(EXIT_FAILURE); }

    // ● Crear un pipe por cada proceso
    for (int i = 0; i < NUM_GRUPOS; i++)
        if (pipe(tuberias[i]) < 0) { perror("pipe"); exit(EXIT_FAILURE); }

    // ● Medir tiempo de inicio
    clock_gettime(CLOCK_MONOTONIC, &t0);
    fprintf(f, "=== REPORTE PROCESOS ===\nGrupo\tVentas\n");

    // ● Fork por grupo
    for (int i = 0; i < NUM_GRUPOS; i++) {
        pid_t pid = fork();
        if (pid < 0)       { perror("fork");        exit(EXIT_FAILURE); }
        if (pid == 0) { // código hijo
            close(tuberias[i][0]);
            int ventas = TOTAL_VENTAS / NUM_GRUPOS;
            srand(time(NULL)^getpid());
            double sum = 0;
            for (int j=0;j<ventas;j++)
                sum += rand()%91 + 10;
            write(tuberias[i][1], &sum, sizeof(sum));
            exit(EXIT_SUCCESS);
        }
        close(tuberias[i][1]);  // padre cierra escritura
    }

    // ● Padre recoge subtotales
    for (int i = 0; i < NUM_GRUPOS; i++) {
        wait(NULL);
        read(tuberias[i][0], &parcial, sizeof(parcial));
        fprintf(f, "%s\t%.2f\n", grupos[i], parcial);
        total += parcial;
    }

    // ● Medir fin y duración
    clock_gettime(CLOCK_MONOTONIC, &t1);
    resta_tiempo(&t0, &t1, &dt);
    fprintf(f, "\nTotal vendido: %.2f\nDuración: %ld.%09ld s\n",
            total, dt.tv_sec, dt.tv_nsec);
    printf("Procesos -> Total: %.2f | Tiempo: %ld.%09ld s\n",
           total, dt.tv_sec, dt.tv_nsec);

    fclose(f);
    return 0;
}
