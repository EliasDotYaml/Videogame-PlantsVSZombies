#include <stdio.h>
#include "tarea1.h"

int main() {
    printf("Probando lectura de configuracion \n");
    // se usa la funcion para cargar Logica.txt
    Configuracion Logica = cargar_configuracion("Logica.txt");
    // se imprimen los valores leidos
    printf("Oleadas: %d Vida Zombie: %d Vida Casa: %d\n\n", Logica.max_oleadas, Logica.resistencia_zombie, Logica.resistencia_casa);

    printf("=== probando lectura y memoria de mapa ===\n");
    // se carga la matriz del mapa de DimensionesMapa.txt
    Mapa mapa = cargar_mapa("DimensionesMapa.txt");
    printf("Dimensiones del jardin: %d filas x %d columnas\n", mapa.filas, mapa.columnas);
    printf("matriz cargada en ram:\n");
    // se muestra el mapa en pantalla
    imprimir_mapa(&mapa);

    // se libera la memoria al finalizar la prueba
    liberar_mapa(&mapa);
    printf("\nmemoria liberada correctamente.\n");
    return 0;
}