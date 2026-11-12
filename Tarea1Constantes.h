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
#define patatapum_abajo 10
#define patatapum_arriba 11

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

#define costo_girasol 50
#define costo_lanzaguisantes 100
#define costo_nuez 50
#define costo_patatapum 150

int colocar_planta(Mapa *mapa, int **matriz_vida, int fila, int columna, int tipo_planta, int *soles_disponibles);

// guardamos el estado actual de la matriz en un archivo de texto
int guardar_mapa(const char *nombre_archivo, const Mapa *mapa);

// redimensionamos la matriz manteniendo las casillas existentes
int redimensionar_mapa(Mapa *mapa, int nuevas_filas, int nuevas_columnas);

// funcion para generar un mapa aleatorio con las dimensiones dadas
// ejemplo: Mapa mapa = generar_mapa_aleatorio(5, 10) genera un mapa de 5 filas y 10 columnas
// para guardar el mapa generado en un archivo de texto se puede usar la funcion guardar_mapa("DimensionesMapa.txt", &mapa)
Mapa generar_mapa_aleatorio(int filas, int columnas);