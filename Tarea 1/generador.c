#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "tarea1.h"

// programa que genera mapas aleatorios
// asigna probabilidades a cada casilla y genera un archivo de texto

int main() {
    int filas = 5;
    int columnas = 9;
    const char *nombre_salida = "mapa.txt";

    srand((unsigned int)time(NULL));

    FILE *fp = fopen(nombre_salida, "w");
    if (fp == NULL) {
        printf("Error: No se puede crear el archivo %s\n", nombre_salida);
        return 1;
    }

    fprintf(fp, "%d %d\n", filas, columnas);
    // genera la matriz del mapa
    for (int i = 0; i < filas; i++) {
        for (int j = 0; j < columnas; j++) {
            int elemento = casilla_vacia; // por defecto va a ser pasto libre
            int probabilidad = rand() % 100; // numero aleatorio entre 0 y 99

            // en la ultima columna hay un 20 % de generar un zombie
            if (j == columnas - 1 && probabilidad < 20) {
                elemento = zombie_basico;
            }

            // hay un 10 % de que sea obstaculo
            else if (j < columnas - 1 && probabilidad < 10) {
                elemento = obstaculo;
            }

            fprintf(fp, "%d ", elemento);
        }
        fprintf(fp, "\n");
    }
    
    fclose(fp);
    printf("¡Mapa aleatorio de %d%d generado correctamente en '%s'!\n", filas, columnas, nombre_salida);

    return 0;
}