CC = gcc
CFLAGS = -Wall -std=c11 -pthread

all: proyecto

proyecto: main.o procesos_libros.o hilos_libros.o
	$(CC) $(CFLAGS) -o proyecto main.o procesos_libros.o hilos_libros.o

main.o: main.c procesos_libros.h hilos_libros.h
	$(CC) $(CFLAGS) -c main.c

procesos_libros.o: procesos_libros.c procesos_libros.h
	$(CC) $(CFLAGS) -c procesos_libros.c

hilos_libros.o: hilos_libros.c hilos_libros.h
	$(CC) $(CFLAGS) -c hilos_libros.c

clean:
	rm -f *.o proyecto
