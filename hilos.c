// hilos.c
// Versión multihilo: pthread + mutex para simular ventas de libros

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include "config.h" // Define NUM_GRUPOS y VENTAS_TOTALES
#include "hilos.h" // Expone ejecutar_hilos()

// Subtotales por grupo A–H
static int subtotales[NUM_GRUPOS];
// Mutex para proteger el arreglo de subtotales
static pthread_mutex_t mutex_subtotales;

// Función para obtener el tiempo actual en segundos
// Tiempo monotónico de alta precisión 
static double tiempo_actual() {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts); // Evita problemas con cambios de hora
    return ts.tv_sec + ts.tv_nsec / 1e9;
}

// Rutina que ejecuta cada hilo: simula ventas de libros con precios aleatorios
static void* procesar_por_hilo(void *arg) {
    // 1) Obtener el ID de grupo (0..NUM_GRUPOS-1) desde el argumento
    int id = *(int*)arg;

    // 2) Calcular cuántas ventas procesa este hilo
    int ventas_por_grupo = VENTAS_TOTALES / NUM_GRUPOS;

    // 3) Inicializar la semilla del generador de números aleatorios
    //    mezclando el tiempo y el ID del hilo para mayor aleatoriedad
    srand((unsigned)time(NULL) ^ (unsigned long)pthread_self());

    int subtotal = 0;  // Acumulador local de precios

    // 4) Bucle de simulación de ventas
    for (int i = 0; i < ventas_por_grupo; i++) {
        // Generar un precio aleatorio entre 10 y 100
        int precio = rand() % 91 + 10;
        subtotal += precio;
    }

    // 5) Sección crítica: guardar el subtotal en el arreglo compartido
    pthread_mutex_lock(&mutex_subtotales);
      subtotales[id] = subtotal;
    pthread_mutex_unlock(&mutex_subtotales);

    return NULL;
}

// Función principal que gestiona la creación y sincronización de los hilos
void ejecutar_hilos(void) {
    pthread_t hilos[NUM_GRUPOS]; // Arreglo de identificadores de hilos
    int indices[NUM_GRUPOS]; // IDs para pasar a cada hilo
    pthread_mutex_init(&mutex_subtotales, NULL); // Inicializar mutex

     // Crear y preparar el archivo de reporte
    FILE *f = fopen("reporte_hilos.txt","w");
    if (!f) { perror("fopen hilos"); exit(EXIT_FAILURE); } // Manejo de errores al abrir el archivo
    fprintf(f, "SIMULACIÓN VENTAS DE LIBROS (Hilos)\n");
    fprintf(f, "Grupos A–H | Total: %d ventas\n\n", VENTAS_TOTALES);
    fprintf(f, "Grupo\tSubtotal\n");
    fclose(f); 

    double t_ini = tiempo_actual(); // Inicio de medición de tiempo

    // Crear un hilo por grupo
    for (int g = 0; g < NUM_GRUPOS; g++) {
        indices[g] = g; // Asignar ID de grupo a cada hilo
        if (pthread_create(&hilos[g], NULL,
                           procesar_por_hilo, &indices[g]) != 0)
        {
            perror("pthread_create");
            exit(EXIT_FAILURE); // Si falla la creación del hilo, termina el programa
        }
    }
   // Esperar a que todos los hilos terminen
    for (int g = 0; g < NUM_GRUPOS; g++)
        pthread_join(hilos[g], NULL);

    double t_fin = tiempo_actual(); // Fin de medición de tiempo

    // Guardar resultados en archivo
    f = fopen("reporte_hilos.txt","a");
    for (int g = 0; g < NUM_GRUPOS; g++) {
        fprintf(f, " %c\t%d\n", 'A'+g, subtotales[g]); // Escribir subtotales de cada grupo
    }
    // Calcular y guardar resumen
    fprintf(f, "\nNúmero de grupos: %d\n", NUM_GRUPOS);
    int total_vendido = 0;
    for (int g = 0; g < NUM_GRUPOS; g++)
        total_vendido += subtotales[g]; // Sumar subtotales para obtener total vendido
    fprintf(f, "Total vendido: %d\n", total_vendido);
    fprintf(f, "Total transacciones: %d\n", VENTAS_TOTALES);
    fprintf(f, "Tiempo de ejecución: %.6f segundos\n",
            t_fin - t_ini);
    fclose(f);

    pthread_mutex_destroy(&mutex_subtotales); // Liberar recursos del mutex
}

// Punto de entrada del programa
int main(void) {
    ejecutar_hilos(); // Llama a la función principal que gestiona los hilos
    return 0; 
}
