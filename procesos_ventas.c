#include "procesos_ventas.h"
#include "configuracion.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <time.h>

// ---------------------------------------------
// Función interna: muestra la diferencia de tiempo
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
// Ejecuta la simulación de ventas repartiendo
// VENTAS_TOTALES entre NUM_PROCESOS procesos.
// Cada proceso genera su subtotal y el padre
// los suma; al final vuelca un informe a disco.
// ---------------------------------------------
void ejecutarProcesos(void) {
    int tuberias[NUM_PROCESOS][2];
    double subtotal, total = 0.0;
    struct timespec t_inicio, t_fin, t_delta;
    FILE *f = fopen("reporte_procesos.txt", "w");
    if (!f) {
        perror("Error abriendo reporte_procesos.txt");
        exit(EXIT_FAILURE);
    }

    // 1) Crear una tubería por cada proceso
    for (int i = 0; i < NUM_PROCESOS; i++) {
        if (pipe(tuberias[i]) < 0) {
            perror("pipe");
            exit(EXIT_FAILURE);
        }
    }

    // 2) Medir tiempo de inicio
    clock_gettime(CLOCK_MONOTONIC, &t_inicio);

    // 3) Crear procesos
    for (int i = 0; i < NUM_PROCESOS; i++) {
        pid_t pid = fork();
        if (pid < 0) {
            perror("fork");
            exit(EXIT_FAILURE);
        }
        if (pid == 0) {
            // Código del hijo:
            close(tuberias[i][0]);  // cerrar lectura
            // Calcular cuántas ventas le tocan
            int base = VENTAS_TOTALES / NUM_PROCESOS;
            int extra = (i < VENTAS_TOTALES % NUM_PROCESOS) ? 1 : 0;
            double suma = 0;
            srand(time(NULL) ^ getpid());
            for (int j = 0; j < base + extra; j++) {
                int precio = rand() % 91 + 10;  // precio aleatorio $10–$100
                suma += precio;
            }
            // Enviar subtotal al padre
            write(tuberias[i][1], &suma, sizeof(suma));
            close(tuberias[i][1]);
            exit(EXIT_SUCCESS);
        }
        // Código del padre: cerrar escritura
        close(tuberias[i][1]);
    }

    // 4) Recoger subtotales y sumar
    for (int i = 0; i < NUM_PROCESOS; i++) {
        wait(NULL);
        read(tuberias[i][0], &subtotal, sizeof(subtotal));
        total += subtotal;
        close(tuberias[i][0]);
    }

    // 5) Medir tiempo de fin
    clock_gettime(CLOCK_MONOTONIC, &t_fin);
    calcularTiempo(&t_inicio, &t_fin, &t_delta);

    // 6) Escribir reporte
    fprintf(f, "===== REPORTE DE PROCESOS =====\n");
    fprintf(f, "Procesos usados : %d\n", NUM_PROCESOS);
    fprintf(f, "Ventas totales  : %.2f\n", total);
    fprintf(f, "Tiempo ejec.    : %ld.%09lds seg\n",
            t_delta.tv_sec, t_delta.tv_nsec);
    fclose(f);

    // También lo mostramos por pantalla
    printf("Procesos -> Total: %.2f | Tiempo: %ld.%09lds s\n",
           total, t_delta.tv_sec, t_delta.tv_nsec);
}
