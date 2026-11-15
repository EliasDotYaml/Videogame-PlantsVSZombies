# Videogame-PlantsVSZombies

tarea1.h: cabecera con las estructuras (Configuracion, Mapa) y constantes
tarea1.c: lectura de los archivos y gestion de la memoria dinamica con malloc

DimensionesMapa.txt: matriz con el mapa inicial del jardin
GeneradorMapa.c: programa que crea tableros aleatorios en "DimensionesMapa.txt"

Makefile: automatizacion de la compilacion

Logica.txt: parametros iniciales de la partida

main.c: programa para probar que la lectura funciona

- Para cambiar parametros del juego: 
    editar "config.txt"

- En tarea 2 puede modificar las siguientes variables de la siguiente forma

    linea 75 = velocidad de los zombies
    linea 21-39= la vida de todas las weas

- Para cambiar las dimensiones del mapa:    
    en "generador.c", cambiar las variables "filas" y "columnas", tambien se pueden alterar las probabilidades de lo que salga en la casilla
    tambien se puede manualmente en "mapa.txt" la primera fila tiene las dimensiones y las siguientes son las casillas

- Para compilar:
    primero se compila todo con: "make"
    luego se genera un mapa aleatorio: "./generador"
    se prueba la lectura del mapa: "./prueba"
    se limpia los archivos compilados: "make clean"

- Para añadirlo al codigo:
    escribir la cabecera:  
        #include "tarea1.h"
    cargar los datos al inicio de la partida:
        Configuracion config = cargar_configuracion("config.txt")
        Mapa mapa = cargar_mapa("mapa.txt")
    liberar memoria al finalizar el juego:
        liberar_mapa(&mapa)



- Para compilar main.c en powershell:
    gcc main.c tarea1.c -o main.exe
    .\main.exe

- Para compilar la visualizacion en powershell:
    gcc SalidaVisualizacion.c tarea1.c tarea2.c -o juego.exe -lSDL3 -lSDL3_image -lSDL3_mixer
    ./juego.exe

- Para compilar la tarea2 en ubuntu:
    make 
    ./prueba