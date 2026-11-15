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
#define cometumbas 12
#define zombie_cono 13
#define zombie_cubo 14

// fase del sistema de rondas
typedef enum {
    fase_preparacion   = 0,     // tiempo inicial: se puede plantar y no salen zombies
    fase_oleada        = 1,     // salen zombies y no se puede plantar
    fase_descanso      = 2,     // todos los zombies de la ronda murieron, se puede volver a plantar
    fase_victoria      = 3      // se superaron todas las oleadas
} FaseJuego;
// estructura para los datos de config.txt
typedef struct {
    int max_oleadas;            // cantidad maxima de oleadas
    int resistencia_zombie;     // puntos de vida de los zombies
    int resistencia_casa;       // vida de la casa antes de perder
    int frecuencia_sol;         // cada cuantos turnos se generan soles
    int frecuencia_disparo;     // frecuencia a la que disparan las plantas
    int tiempo_preparacion;     // duracion de la fase inicial para plantar
    int tiempo_descanso;        // duracion del descanso entre oleadas
    int soles_iniciales;        // soles con los que empieza el jugador
} Configuracion;

// estructura para controlar el estado entre rondas sin alterar el Mapa
typedef struct {
    FaseJuego fase_actual;       
    int ronda_actual;            
    int tiempo_restante_fase;   // contador regresivo para preparacion/descanso
    int zombies_por_spawnear;   // zombies pendientes por aparecer en la oleada actual
} EstadoRondas;
// estructura para la matriz del mapa
typedef struct {
    int filas;
    int columnas;
    int **matriz;               // puntero doble para la matriz dinamica
} Mapa;

// funciones
Configuracion cargar_configuracion(const char *nombre_archivo);
Mapa cargar_mapa(const char *nombre_archivo, int *soles_disponibles); 
void liberar_mapa(Mapa *mapa);
void imprimir_mapa(const Mapa *mapa);

#define costo_girasol 50
#define costo_lanzaguisantes 100
#define costo_nuez 50
#define costo_patatapum 25
#define costo_cometumbas 75

int colocar_planta(Mapa *mapa, int **matriz_vida, int fila, int columna, int tipo_planta, int *soles_disponibles);

// guardamos el estado actual de la matriz en un archivo de texto
int guardar_mapa(const char *nombre_archivo, const Mapa *mapa, int soles_disponibles);

// redimensionamos la matriz manteniendo las casillas existentes
int redimensionar_mapa(Mapa *mapa, int nuevas_filas, int nuevas_columnas);

// funcion para generar un mapa aleatorio con las dimensiones dadas
// ejemplo: Mapa mapa = generar_mapa_aleatorio(5, 10) genera un mapa de 5 filas y 10 columnas
// para guardar el mapa generado en un archivo de texto se puede usar la funcion guardar_mapa("Logica.txt", &mapa)
Mapa generar_mapa_aleatorio(int filas, int columnas);

EstadoRondas iniciar_estado_rondas(Configuracion config);
int puede_plantar(const EstadoRondas *estado);
int hay_zombies_en_mapa(const Mapa *mapa);
void actualizar_estado_rondas(EstadoRondas *estado, const Mapa *mapa, Configuracion config);
int colocar_planta_en_ronda(Mapa *mapa, int **matriz_vida, int fila, int columna, int tipo_planta, int *soles_disponibles, const EstadoRondas *estado);
