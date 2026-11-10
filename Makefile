# compilador y banderas de advertencia
CC = gcc
CFLAGS = -Wall -Wextra -g

# regla principal: compila el programa de prueba y el generador
all: prueba generador probar_paquete1

# compila el ejecutable 'prueba'
prueba: main.o tarea1.o
	$(CC) $(CFLAGS) -o prueba main.o tarea1.o

# compila el ejecutable 'generador'
generador: GeneradorMapa.o tarea1.o 
	$(CC) $(CFLAGS) -o generador GeneradorMapa.o tarea1.o 

probar_paquete1: main_paquete1.o tarea1.o paquete1.o 
	$(CC) $(CFLAGS) -o probar_paquete1 main_paquete1.o tarea1.o paquete1.o

# compilacion de los archivos objetos(.o)
main.o: main.c Tarea1Constantes.h
	$(CC) $(CFLAGS) -c main.c 

tarea1.o: tarea1.c Tarea1Constantes.h
	$(CC) $(CFLAGS) -c tarea1.c 
	
GeneradorMapa.o: GeneradorMapa.c Tarea1Constantes.h
	$(CC) $(CFLAGS) -c GeneradorMapa.c

paquete1.o: paquete1.c paquete1.h Tarea1Constantes.h
	$(CC) $(CFLAGS) -c paquete1.c 

main_paquete1.o: main_paquete1.c paquete1.h Tarea1Constantes.h
	$(CC) $(CFLAGS) -c main_paquete1.c

clean:
	rm -f *.o *.gch prueba generador probar_paquete1 partida_guardada.txt