CC       := gcc
CFLAGS   := -std=c11 -Wall -pthread
HDRS     := configuracion.h

all: procesos hilos

procesos: procesos.c configuracion.h
	gcc -std=c11 -Wall -pthread procesos.c -o procesos

hilos: hilos.c configuracion.h
	gcc -std=c11 -Wall -pthread hilos.c   -o hilos

clean:
	rm -f procesos hilos reporte_*.txt
