#include "hilos_libros.h"
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <unistd.h>

#define GRUPOS 8
#define VENTAS_POR_GRUPO 1000

double totales[GRUPOS];
double total_general = 0;
static pthread_mutex t lock;

// Muestra un tiemestap legible 
static void mostrar_tiempo_actual(cosnt char* mensaje){
    time_t t = time(NULL);
    printf("%s: %s", mensaje, ctime(&t));
}

// Funcion de hilo que simula ventas

static void* simular_ventas(void* arg){
    int idx = *(int*)arg;
    cosnt char* grupos[GRUPOS] = {"A", "B", "C", "D", "E", "F", "G", "H"};
    srand(time(NULL) ^ pthread_self()); // Semilla aleatoria por hilo
    double subtotal = 0;

    for(int i = 0; i <VENTAS_POR_GRUPO; i++){
        int precio = rand() % 91 + 10; // Precio entre 10 y 100
        subtotal += precio;
        pthread_mutex_lock(&lock);
        printf("Hilo %ld - Grupo %s: Venta %s = $%d\n", pthread_self(), grupos[idx], i + 1, precio);
        pthread_mutex_unlock(&lock);
    }

    pthread_mutex_lock(&lock);
    totales[idx] = subtotal;
    total_general += subtotal;
    pthread_mutex_unlock(&lock);

    pthread_exit(NULL);
}

int main(){
    struct timespec inicio, fin;
    pthread_t hilos[GRUPOS];
    int indices[GRUPOS];
    for(int i = 0; i < GRUPOS; i++)
        indices[i] = i;
    mostrar_tiempo_actual("Inicio(hilos - ventas de libros)");
    clock_gettime(CLOCK_MONOTONIC, &inicio);

    pthread_mutex_init(&lock, NULL);
    for(int i = 0; i < GRUPOS; i++){
        if(pthread_create(&hilos[i], NULL, simular_ventas_libros, &indices[i]) != 0){
            perror("Error al crear el hilo");
            exit(EXIT_FAILURE);
        }
    }

    for (int i = 0; i < GRUPOS; i++){
        pthread_join(hilos[i], NULL);
    }

    pthread_mutex_destroy(&lock);
    clock_gettime(CLOCK_MONOTONIC, &fin);

    mostrar_tiempo_actual("Fin(hilos - ventas de libros)");
    double duracion = (fin.tv_sec - inicio.tv_sec) + (fin.tv_nsec - inicio.tv_nsec) / 1e9;
    printf("Duracion total: %.3f segundos\n", duracion);
    //Resumen de ventas

    printf("\nResumen de ventas de Libros por Grupo:\n");
    for(int i = 0; i < GRUPOS; i++){
        printf("Grupo %c: Total ventas = $%.2f\n", 'A' + i, totales[i]);
    }
    printf("Total general: $%.2f\n", total_general);
    return 0;
}
