#include <stdio.h>
#include "Tarea1Constantes.h"
#include "paquete1.h"

int main() {
    printf("prueba paquete 1\n");

    //cargamos el mapa inicial
    Mapa mapa = cargar_mapa("DimensionesMapa.txt");
    printf("Mapa inicial (%dx%d):\n", mapa.filas, mapa.columnas);
    imprimir_mapa(&mapa);

    // modificamos una casilla
    if (mapa.filas > 1 && mapa.columnas > 1) {
        mapa.matriz[1][1] = planta_basica;
        printf("\nSe coloco una planta en la posicion (1,1):\n");
        imprimir_mapa(&mapa);
    }

    // guardamos el estado actual en un archivo de partida guardad
    guardar_mapa("partida_guardada.txt", &mapa);

    // redimensionamos la grilla
    printf("\nRedimensionando el jardin a 6x10...\n");
    redimensionar_mapa(&mapa, 6, 10);
    imprimir_mapa(&mapa);

    // cargar la partida guardada previamente
    printf("\nRestaurando el estado desde 'partida_guardada.txt'...\n");
    liberar_mapa(&mapa); // liberamos la redimensionada
    mapa = cargar_mapa("partida_guardada.txt");
    imprimir_mapa(&mapa);

    // liberacion final
    liberar_mapa(&mapa);
    printf("\nPrueba de Paquete 1 completada con exito.\n");

    return 0;
}