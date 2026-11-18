# Videogame-PlantsVSZombies

## Estructura del Proyecto

 * `tarea1.c`: Lectura de archivos y gestión de memoria dinámica (`malloc`/`free`).
 * `tarea2.c`: Lógica principal del juego (oleadas, movimiento, ataques, soles, tumbas y colisiones).
 * `SalidaVisualizacion.c`: Bucle gráfico principal e interfaz en SDL3.
 * `main.c`: Programa de prueba de lectura y simulación en consola (sin gráficos).
 * `Logica.txt`: Parámetros iniciales de configuración y distribución del mapa de la partida.
 * `Makefile`: Automatización de la compilación para Windows y Linux.

## Compilación y Ejecución

### Requisitos previos
Tener instalado `gcc`, `make` y las librerías de **SDL3**, **SDL3_image** y **SDL3_mixer**.

* Para compilar:
    primero se compila todo con: "make"
    se limpia los archivos compilados: "make clean"

* Para cambiar parametros del juego (como soles etc.): 
    editar "Logica.txt"

* Resumidamente main es el juego sin grafica y SalidaVisualizacion es todo lo grafico con SDL3, por eso tenemos 2 mains

* En tarea 2 puede modificar las siguientes variables de la siguiente forma
    linea 75 = velocidad de los zombies
    linea 21-39= la vida de todas las cosas

* Para compilar main.c en powershell:
    gcc main.c tarea1.c -o main.exe
    .\main.exe

* Para compilar la visualizacion en powershell:
    gcc SalidaVisualizacion.c tarea1.c tarea2.c -o juego.exe -lSDL3 -lSDL3_image -lSDL3_mixer
    ./juego.exe

* Para compilar la tarea2 en ubuntu:
    make 
    ./prueba
