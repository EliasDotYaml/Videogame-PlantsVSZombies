#pragma once

// valores para saber que hay en cada casilla del mapa
#define casilla_vacia 0
#define zombie_basico 1
#define zombie_flag   2
#define planta_basica 3
#define obstaculo     4
#define girasol 5
#define lanzaguisantes 6
#define nuez 7
#define sol 8
#define guisante 9

// estructura para los datos de config.txt
typedef struct {
    int max_oleadas;            // cantidad maxima de oleadas
    int resistencia_zombie;     // puntos de vida de los zombies
    int resistencia_casa;       // vida de la casa antes de perder
    int frecuencia_sol;         // cada cuantos turnos se generan soles
    int frecuencia_disparo;     // frecuencia a la que disparan las plantas

} Configuracion;

// estructura para la matriz del mapa
typedef struct {
    int filas;
    int columnas;
    int **matriz;               // puntero doble para la matriz dinamica
} Mapa;

// funciones
Configuracion cargar_configuracion(const char *nombre_archivo);
Mapa cargar_mapa(const char *nombre_archivo);
void liberar_mapa(Mapa *mapa);
void imprimir_mapa(const Mapa *mapa);
