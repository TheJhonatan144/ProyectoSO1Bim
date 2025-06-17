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
static int *saldo_global;       // Saldo total de la tienda
static int *num_transacciones;  // Número total de transacciones realizadas
static sem_t *sem_mutex;        // Semáforo para controlar acceso a saldo_global y num_transacciones

// Tiempo monotónico de alta resolución en segundos
static double tiempo_actual() { 
    struct timespec ts;                  // Estructura para almacenar el tiempo
    clock_gettime(CLOCK_MONOTONIC, &ts); // Obtener el tiempo actual en segundos y nanosegundos
    return ts.tv_sec + ts.tv_nsec / 1e9; // Convertir a segundos
}

// Lógica que ejecuta cada proceso hijo
// Función que ejecuta cada proceso hijo para simular ventas de libros
static void procesar_por_proceso(int id_grupo) {
    // Abrir el archivo de reporte en modo “append” para añadir líneas
    FILE *f = fopen("reporte_procesos.txt", "a");
    if (!f) {
        perror("Error al abrir reporte en el proceso hijo");
        exit(EXIT_FAILURE);
    }

    // Inicializar la semilla de rand() usando el PID para asegurar variación entre procesos
    srand(time(NULL) ^ getpid());

    // Cuántas ventas debe atender este grupo
    int ventas_por_grupo = VENTAS_TOTALES / NUM_GRUPOS;
    // Acumulador local de lo vendido por este grupo
    int subtotal_local = 0;

    // Bucle: cada iteración simula una venta
    for (int i = 0; i < ventas_por_grupo; i++) {
        // Solicitar acceso exclusivo al saldo y al contador
        sem_wait(sem_mutex);

        // Si ya se alcanzó el total global de ventas, liberar y salir
        if (*num_transacciones >= VENTAS_TOTALES) {
            sem_post(sem_mutex);
            break;
        }

        // Generar un precio aleatorio entre $10 y $100
        int precio = rand() % 91 + 10;
        // Sumar al acumulado local
        subtotal_local += precio;
        // Aumentar el saldo global con el precio de esta venta
        *saldo_global += precio;

        // Incrementar el contador global de transacciones
        (*num_transacciones)++;
        // Liberar el semáforo para que otro proceso pueda acceder
        sem_post(sem_mutex);
    }

    // Escribir en el reporte el subtotal que vendió este grupo
    fprintf(f, "Grupo %c:\tSubtotal = $%d\n", 'A' + id_grupo, subtotal_local);

    // Cerrar el archivo de reporte
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
    sem_init(sem_mutex,1,1); // Inicializar semáforo para acceso exclusivo

    // Crear o truncar reporte de procesos
    // Abrir archivo de reporte en modo escritura
    FILE *f = fopen("reporte_procesos.txt","w");
    if (!f) { perror("fopen padre"); exit(EXIT_FAILURE); } // Manejo de errores al abrir el archivo
    fprintf(f, "SIMULACIÓN VENTAS DE LIBROS (Procesos)\n");
    fprintf(f, "Grupos A–H | Total: %d ventas\n\n", VENTAS_TOTALES); 
    fprintf(f, "Grupo\tSaldo parcial\n"); 
    fclose(f);

    double t_ini = tiempo_actual(); // Inicio de medición de tiempo

    // Lanzar procesos hijos
    // Crear un proceso por grupo
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
   
    // El padre espera a que todos los procesos hijos terminen
    for (int i = 0; i < NUM_GRUPOS; i++) wait(NULL); 

    double t_fin = tiempo_actual(); // Fin de medición de tiempo

    // Resumen final
    f = fopen("reporte_procesos.txt","a");
    fprintf(f, "\nNúmero de grupos: %d\n", NUM_GRUPOS);
    fprintf(f, "Saldo final (total vendido): %d\n", *saldo_global);
    fprintf(f, "Total transacciones: %d\n", VENTAS_TOTALES);
    fprintf(f, "Tiempo de ejecución: %.6f segundos\n",
            t_fin - t_ini);
    fclose(f);

    sem_destroy(sem_mutex); // Liberar recursos del semáforo
    munmap(saldo_global,sizeof(int)); 
    munmap(num_transacciones,sizeof(int));
    munmap(sem_mutex,sizeof(sem_t));
}

// Punto de entrada
int main(void) {
    ejecutar_procesos(); 
    return 0;
}
