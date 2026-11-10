#include <stdio.h>
#include <stdlib.h>
#include "Tarea1Constantes.h"
#include "paquete1.h"

// se guarda la matriz actual en un archivo con el formato estandar
int guardar_mapa(const char *nombre_archivo, const Mapa *mapa) {
    if (mapa == NULL || mapa->matriz == NULL) {
        printf("Error: el mapa no esta iniciado\n");
        return 0;
    }

    // abrimos el archivo en modo escritura
    FILE *fp = fopen(nombre_archivo, "w");
    if (fp == NULL) {
        printf("Error: no se pudo crear el archivo %s\n", nombre_archivo);
        return 0;
    }

    // escribimos las dimensiones actuales
    fprintf(fp, "%d %d\n", mapa->filas, mapa->columnas);

    // escribimos el valor de cada casilla
    for (int i = 0; i < mapa->filas; i++) {
        for (int j = 0; j < mapa->columnas; j++) {
            fprintf(fp, "%d ", mapa->matriz[i][j]);
        }
        fprintf(fp, "\n");
    }

    fclose(fp);
    printf("Estado del juego guardado exitosamente en '%s'\n", nombre_archivo);
    return 1;
}

// redimensionamos el jardin a un nuevo tamaño
int redimensionar_mapa(Mapa *mapa, int nuevas_filas, int nuevas_columnas) {
    if (mapa == NULL || nuevas_filas <= 0 || nuevas_columnas <= 0) {
        printf("Error: dimensiones invalidas para redimensionar\n");
        return 0;
    }

    // reservamos espacio para la nueva matriz
    int **nueva_matriz = (int **)malloc(nuevas_filas * sizeof(int *));
    if (nueva_matriz == NULL) {
        printf("Error en malloc al redimensionar filas\n");
        return 0;
    }

    for (int i = 0; i < nuevas_filas; i++) {
        nueva_matriz[i] = (int *)malloc(nuevas_columnas * sizeof(int));
        if (nueva_matriz[i] == NULL) {
            printf("Error en malloc al redimensionar columnas\n");
            for (int k = 0; k < i; k++) free(nueva_matriz[k]);
            free(nueva_matriz);
            return 0;
        }

        // copiamos lo que habia antes o ponemos casillas vacias si la matriz crecio
        for (int j = 0; j < nuevas_columnas; j++) {
            if (i < mapa->filas && j < mapa->columnas && mapa->matriz != NULL) {
                nueva_matriz[i][j] = mapa->matriz[i][j]; // se mantiene lo que habia
            } else {
                nueva_matriz[i][j] = casilla_vacia; // casilla limpia en lo nuevo
            }
        }
    }

    // liberamos la matriz antigua
    if (mapa->matriz != NULL) {
        for (int i = 0; i < mapa->filas; i++) {
            free(mapa->matriz[i]);
        }
        free(mapa->matriz);
    }

    // actualizamos los datos del mapa con la nueva matriz
    mapa->matriz = nueva_matriz;
    mapa->filas = nuevas_filas;
    mapa->columnas = nuevas_columnas;

    printf("Mapa redimensionado a %dx%d con exito\n", nuevas_filas, nuevas_columnas);
    return 1;
}

