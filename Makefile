CC = gcc
CFLAGS = -Wall -std=c11 -pthread

all: proyecto

proyecto: main.o procesos_ventas.o hilos_ventas.o
	$(CC) $(CFLAGS) -o proyecto main.o procesos_ventas.o hilos_ventas.o

main.o: main.c procesos_ventas.h hilos_ventas.h configuracion.h
	$(CC) $(CFLAGS) -c main.c

procesos_ventas.o: procesos_ventas.c procesos_ventas.h configuracion.h
	$(CC) $(CFLAGS) -c procesos_ventas.c

hilos_ventas.o: hilos_ventas.c hilos_ventas.h configuracion.h
	$(CC) $(CFLAGS) -c hilos_ventas.c

clean:
	rm -f *.o proyecto reporte_*.txt
