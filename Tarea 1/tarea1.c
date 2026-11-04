#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tarea1.h"

Configuracion cargar_configuracion(const char *nombre_archivo) {
    Configuracion config = {0, 0, 0, 0, 0};
  
    FILE *fp = fopen(nombre_archivo, "r");
    if (fp == NULL) {
        printf("Error: No se pudo abrir el archivo de configuracion '%s'\n", nombre_archivo);
        exit(EXIT_FAILURE);
    }

    char clave[50];
    int valor;

    while(fscanf(fp, "%49s %d", clave, &valor) == 2) {
        if (strcmp(clave, "cantidad_oleadas") == 0) config.max_oleadas = valor;
        else if (strcmp(clave, "resistencia_zombie") == 0) config.resistencia_zombie = valor;
        else if (strcmp(clave, "resistencia_casa") == 0) config.resistencia_casa = valor;
        else if (strcmp(clave, "frecuencia_sol") == 0) config.frecuencia_sol = valor;
        else if (strcmp(clave, "frecuencia_disparo") == 0) config.frecuencia_disparo = valor;
    }

    fclose(fp);
    return config;
}

Mapa cargar_mapa(const char *nombre_archivo) {
    Mapa mapa;
    mapa.filas = 0;
    mapa.columnas = 0;
    mapa.matriz = NULL;

    FILE *fp = fopen(nombre_archivo, "r");
    if (fp == NULL) {
        printf("Error: No se puedo abrir el archivo del mapa '%s'\n", nombre_archivo);
        exit(EXIT_FAILURE);
    }

    if (fscanf(fp, "%d %d", &mapa.filas, &mapa.columnas) != 2) {
        printf("Error: Formato de dimisiones invalido en el archivo de mapa. \n");
        fclose(fp);
        exit(EXIT_FAILURE);
    }

    mapa.matriz = (int **)malloc(mapa.filas *sizeof(int *));
    if (mapa.matriz == NULL) {
        printf("Error de memoria al asignar las filas del mapa. \n");
        fclose(fp);
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < mapa.filas; i++) {
        mapa.matriz[i] = (int *)malloc(mapa.columnas * sizeof(int));
        if (mapa.matriz[i] == NULL) {
            printf("Error de memoria al asignar la columna %d. \n", i);
            for (int k = 0; k < i; k++) free(mapa.matriz[k]);
            free(mapa.matriz);
            fclose(fp);
            exit(EXIT_FAILURE);
        }

        for (int j = 0; j < mapa.columnas; j++) {
            fscanf(fp, "%d", &mapa.matriz[i][j]);
        }
    }

    fclose(fp);
    return mapa;
}

void liberar_mapa(Mapa *mapa) {
    if (mapa != NULL && mapa->matriz != NULL) {
        for (int i = 0; i < mapa->filas; i++) {
            free(mapa->matriz[i]);
        }
        free(mapa->matriz);
        mapa->matriz = NULL;
        mapa->filas = 0;
        mapa->columnas = 0;
    }
}

void imprimir_mapa(const Mapa *mapa) {
    if (mapa == NULL || mapa->matriz == NULL) return;
    for (int i = 0; i < mapa->filas; i++) {
        for (int j = 0; j < mapa->columnas; j++) {
            printf("%d ", mapa->matriz[i][j]);
        }
        printf("\n");
    }
}