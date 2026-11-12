#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Tarea1Constantes.h"

// busca el archivo Logica.txt y guarda los valores
Configuracion cargar_configuracion(const char *nombre_archivo) {
    Configuracion config = {0, 0, 0, 0, 0};
  
    // se intenta abrir el archivo en modo lectura "r"
    FILE *fp = fopen(nombre_archivo, "r");
    if (fp == NULL) {
        printf("Error: No se pudo abrir el archivo de configuracion '%s'\n", nombre_archivo);
        exit(EXIT_FAILURE); // si no existe el archivo se finaliza el programa
    }

    char clave[50]; // buffer para la palabra clave
    int valor;      // variable para guardar el numero del parametro

    // lee el nombre del parametro y su valor
    while(fscanf(fp, "%49s %d", clave, &valor) == 2) {
        // strcasecmp compara el texto sin importar si es mayuscula o minuscula
        if (strcasecmp(clave, "cantidad_oleadas") == 0) config.max_oleadas = valor;
        else if (strcasecmp(clave, "resistencia_zombie") == 0) config.resistencia_zombie = valor;
        else if (strcasecmp(clave, "resistencia_casa") == 0) config.resistencia_casa = valor;
        else if (strcasecmp(clave, "frecuencia_sol") == 0) config.frecuencia_sol = valor;
        else if (strcasecmp(clave, "frecuencia_disparo") == 0) config.frecuencia_disparo = valor;
    }

    // se cierra el archivo
    fclose(fp);
    return config;
}

// funcion que lee el mapa y pide la memoria dinamica
Mapa cargar_mapa(const char *nombre_archivo) {
    Mapa mapa;
    mapa.filas = 0;
    mapa.columnas = 0;
    mapa.matriz = NULL;

    // se abre el archivo de texto del mapa
    FILE *fp = fopen(nombre_archivo, "r");
    if (fp == NULL) {
        printf("Error: No se puedo abrir el archivo del mapa '%s'\n", nombre_archivo);
        exit(EXIT_FAILURE);
    }

    // lee la primera linea donde vienen las dimensiones de las filas y columna
    if (fscanf(fp, "%d %d", &mapa.filas, &mapa.columnas) != 2) {
        printf("Error: Formato de dimisiones invalido en el archivo de mapa. \n");
        fclose(fp);
        exit(EXIT_FAILURE);
    }

    // reserva memoria para el arreglo de filas
    mapa.matriz = (int **)malloc(mapa.filas *sizeof(int *));
    if (mapa.matriz == NULL) {
        printf("Error de memoria al asignar las filas del mapa. \n");
        fclose(fp);
        exit(EXIT_FAILURE);
    }

    // reserva memoria para las columnas de cada fila
    for (int i = 0; i < mapa.filas; i++) {
        mapa.matriz[i] = (int *)malloc(mapa.columnas * sizeof(int));
        // si malloc falla, se libera la memoria para evitar fugas
        if (mapa.matriz[i] == NULL) {
            printf("Error de memoria al asignar la columna %d. \n", i);
            for (int k = 0; k < i; k++) free(mapa.matriz[k]);
            free(mapa.matriz);
            fclose(fp);
            exit(EXIT_FAILURE);
        }

        // se leen los numeros de la fila actual y se guarda en la matriz
        for (int j = 0; j < mapa.columnas; j++) {
            fscanf(fp, "%d", &mapa.matriz[i][j]);
        }
    }

    fclose(fp);
    return mapa;
}

// libera la memoria dinamica del mapa
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

// imprime el mapa en la consola
void imprimir_mapa(const Mapa *mapa) {
    if (mapa == NULL || mapa->matriz == NULL) return;
    for (int i = 0; i < mapa->filas; i++) {
        for (int j = 0; j < mapa->columnas; j++) {
            printf("%d ", mapa->matriz[i][j]);
        }
        printf("\n");
    }
}

// colocamos una planta en el mapa y restamos los soles disponibles
int colocar_planta(Mapa *mapa, int **matriz_vida, int fila, int columna, int tipo_planta, int *soles_disponibles) {
    if (mapa == NULL || mapa->matriz == NULL) return 0; // error si el mapa o la matriz de vida es NULL
    if (fila < 0 || fila >= mapa->filas || columna < 0 || columna >= mapa->columnas) return 0; // error si la posición está fuera del mapa
    if (mapa->matriz[fila][columna] != casilla_vacia) return 0; // error si la casilla no está vacía
    int costo = 0;
    int vida_inicial = 0;
    switch (tipo_planta) {
        case girasol:
            costo = costo_girasol;
            vida_inicial = 5; // vida inicial del girasol
            break;
        case lanzaguisantes:
            costo = costo_lanzaguisantes;
            vida_inicial = 5; // vida inicial del lanzaguisantes
            break;
        case nuez:
            costo = costo_nuez;
            vida_inicial = 5; // vida inicial de la nuez
            break;
        case patatapum_abajo:
        case patatapum_arriba:
            costo = costo_patatapum;
            vida_inicial = 8; // vida inicial del patatapum
            break;
        default:
            return 0; // error si el tipo de planta no es válido
    }
    if (soles_disponibles != NULL) {
        if (*soles_disponibles < costo) {
            return 0; // error si no hay suficientes soles
        }
        *soles_disponibles -= costo; // restamos el costo de la planta a los soles disponibles
    }
    mapa->matriz[fila][columna] = tipo_planta; // colocamos la planta en el mapa
    if (matriz_vida != NULL) {
        matriz_vida[fila][columna] = vida_inicial; // asignamos la vida inicial a la planta
    }
    return 1; // éxito al colocar la planta
}