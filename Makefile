CC       := gcc
CFLAGS   := -std=c11 -Wall -pthread
HDRS     := configuracion.h

all: procesos hilos

procesos: procesos_ventas.c procesos_ventas.h $(HDRS)
	$(CC) $(CFLAGS) $< -o procesos

hilos: hilos_ventas.c hilos_ventas.h $(HDRS)
	$(CC) $(CFLAGS) $< -o hilos

clean:
	rm -f procesos hilos reporte_*.txt
