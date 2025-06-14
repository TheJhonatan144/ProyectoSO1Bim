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

}