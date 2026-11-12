# compilador y banderas de advertencia
CC = gcc
CFLAGS = -Wall -Wextra -Werror -g

# regla principal: compila el programa y genera el ejecutable "prueba"
all: prueba


prueba: main.o tarea1.o tarea2.o
	$(CC) $(CFLAGS) -o prueba main.o tarea1.o tarea2.o

main.o: main.c Tarea1Constantes.h 
	$(CC) $(CFLAGS) -c main.c

tarea1.o: tarea1.c Tarea1Constantes.h
	$(CC) $(CFLAGS) -c tarea1.c

tarea2.o: tarea2.c Tarea1Constantes.h
	$(CC) $(CFLAGS) -c tarea2.c

clean:
	rm -f *.o *.gch prueba partida_guradada.txt