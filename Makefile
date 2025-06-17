# Variables: No repita el compilador y opciones
CC      := gcc
CFLAGS  := -std=c11 -Wall -pthread

#Regla: contruye los ejecutables de hilos y procesos
all: procesos hilos

# Reglas específcas
procesos: procesos.c procesos.h config.h
	$(CC) $(CFLAGS) procesos.c -o procesos

hilos: hilos.c hilos.h config.h
	$(CC) $(CFLAGS) hilos.c -o hilos

clean:   #Borra todos los archivos de ojetos, ejecutables y reportes para empezar limpio
	rm -f procesos hilos reporte_*.txt
