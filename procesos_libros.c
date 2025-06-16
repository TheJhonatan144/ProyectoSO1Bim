
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <time.h>

#define GRUPOS 8
#define VENTAS_POR_GRUPO 1000

// Función para mostrar timestamp legible
void mostrar_tiempo_actual(const char *mensaje)
{
    time_t t = time(NULL);
    printf("%s: %s", mensaje, ctime(&t));
}

// Simula ventas de libros para un grupo y envía el total al padre
void simular_ventas_libros(const char *grupo, int write_fd)
{
    srand(time(NULL) ^ getpid());
    double total = 0;
    for (int i = 0; i < VENTAS_POR_GRUPO; i++)
    {
        // Precio aleatorio de cada libro entre $10 y $100
        int precio = rand() % 91 + 10;
        total += precio;
        // Opcional: comenta la línea siguiente
        printf("Proceso %d - Grupo %s: Venta %d = $%d\n",
               getpid(), grupo, i + 1, precio);
    }
    write(write_fd, &total, sizeof(total));

int main() {
    struct timespec inicio, fin;
    const char* grupos[] = { "A","B","C","D","E","F","G","H" };
    int pipes_fd[GRUPOS][2];
    double totales[GRUPOS];

    // Crear pipes para cada grupo
    for (int i = 0; i < GRUPOS; i++) {
        if (pipe(pipes_fd[i]) < 0) {
            perror("pipe");
            exit(EXIT_FAILURE);
        }
    }

    mostrar_tiempo_actual("⏱ Inicio (procesos - ventas de libros)");
    clock_gettime(CLOCK_MONOTONIC, &inicio);

    for (int i = 0; i < GRUPOS; i++) {
        pid_t pid = fork();
        if (pid < 0) {
            perror("fork");
            exit(EXIT_FAILURE);
        }
        if (pid == 0) {
            // En el hijo: cerrar lectura, simular ventas y enviar total
            close(pipes_fd[i][0]);
            simular_ventas_libros(grupos[i], pipes_fd[i][1]);
            close(pipes_fd[i][1]);
            exit(EXIT_SUCCESS);
        }
        // En el padre: cerrar escritura para este pipe
        close(pipes_fd[i][1]);
    }

    // Padre espera y lee totales de cada grupo
    for (int i = 0; i < GRUPOS; i++) {
        wait(NULL);
        read(pipes_fd[i][0], &totales[i], sizeof(double));
        close(pipes_fd[i][0]);
    }

    clock_gettime(CLOCK_MONOTONIC, &fin);
    mostrar_tiempo_actual("⏱ Fin (procesos - ventas de libros)");

    double duracion = (fin.tv_sec - inicio.tv_sec) +
                      (fin.tv_nsec - inicio.tv_nsec) / 1e9;
    printf("\n⏳ Duración total (procesos): %.3f segundos\n", duracion);

    // Resumen de ventas por grupo
    printf("\n--- Resumen de Ventas de Libros por Grupo ---\n");
    for (int i = 0; i < GRUPOS; i++) {
        printf("Grupo %s: Total ventas = $%.2f\n", grupos[i], totales[i]);
    }

    return 0;
}
