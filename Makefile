# Compilador y banderas
CC = gcc
CFLAGS = -Wall -Wextra -Werror -g
SDL_FLAGS = -lSDL3 -lSDL3_image -lSDL3_mixer

TARGET = juego$(EXT)

# Compilar todo
all: $(TARGET)

$(TARGET): SalidaVisualizacion.o tarea1.o tarea2.o
	$(CC) $(CFLAGS) -o $(TARGET) SalidaVisualizacion.o tarea1.o tarea2.o $(SDL_FLAGS)

SalidaVisualizacion.o: SalidaVisualizacion.c Tarea1Constantes.h VisualizacionConstantes.h
	$(CC) $(CFLAGS) -c SalidaVisualizacion.c

tarea1.o: tarea1.c Tarea1Constantes.h
	$(CC) $(CFLAGS) -c tarea1.c

tarea2.o: tarea2.c Tarea1Constantes.h
	$(CC) $(CFLAGS) -c tarea2.c

# Limpiar archivos y ejecutable
clean:
	rm -f *.o *.gch juego juego.exe PartidaGuardada.txt
