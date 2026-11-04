#include <stdio.h>
#include "tarea1.h"

int main() {
    printf("=== Probando lectura de configuracion ===\n");
    Configuracion config = cargar_configuracion("config.txt");
    printf("Oleadas: %d | Vida Zombie: %d | Vida Casa: %d\n\n", config.max_oleadas, config.resistencia_zombie, config.resistencia_casa);

    printf("=== probando lectura y memoria de mapa ===\n");
    Mapa mapa = cargar_mapa("mapa.txt");
    printf("Dimensiones del jardin: %d filas x %d columnas\n", mapa.filas, mapa.columnas);
    printf("matriz cargada en ram:\n");
    imprimir_mapa(&mapa);

    liberar_mapa(&mapa);
    printf("\nmemoria liberada correctamente.\n");

    return 0;
}