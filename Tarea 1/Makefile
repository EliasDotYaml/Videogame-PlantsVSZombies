# compilador y banderas de advertencia
CC = gcc
CFLAGS = -Wall -Wextra -g

# regla principal: compila el programa de prueba y el generador
all: prueba generador

# compila el ejecutable 'prueba'
prueba: main.o tarea1.o
	$(CC) $(CFLAGS) -o prueba main.o tarea1.o

# compila el ejecutable 'generador'
generador: generador.o tarea1.o 
	$(CC) $(CFLAGS) -o generador generador.o tarea1.o 

# compilacion de los archivos objetos(.o)
main.o: main.c tarea1.h
	$(CC) $(CFLAGS) -c main.c 

tarea1.o: tarea1.c tarea1.h
	$(CC) $(CFLAGS) -c tarea1.c 
	
generador.o: generador.c tarea1.h
	$(CC) $(CFLAGS) -c generador.c

clean:
	rm -f *.o *.gch prueba generador mapa_aleatorio.txt