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
}
