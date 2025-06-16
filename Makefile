CC       := gcc
CFLAGS   := -std=c11 -Wall -pthread
HDR      := config.h

all: procesos hilos

procesos: procesos.c procesos.h $(HDR)
	$(CC) $(CFLAGS) procesos.c -o procesos

hilos: hilos.c hilos.h $(HDR)
	$(CC) $(CFLAGS) hilos.c -o hilos

clean:
	rm -f procesos hilos reporte_*.txt
