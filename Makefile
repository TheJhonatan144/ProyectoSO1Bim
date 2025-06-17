CC      := gcc
CFLAGS  := -std=c11 -Wall -pthread

all: procesos hilos

procesos: procesos.c procesos.h config.h
	$(CC) $(CFLAGS) procesos.c -o procesos

hilos: hilos.c hilos.h config.h
	$(CC) $(CFLAGS) hilos.c -o hilos

clean:
	rm -f procesos hilos reporte_*.txt
