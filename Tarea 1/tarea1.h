#ifndef TAREA1_H
#define TAREA1_H

#define casilla_vacia 0
#define zombie_basico 1
#define xombie_flag   2
#define planta_basica 3
#define obstaculo     4

typedef struct {
    int max_oleadas;
    int resistencia_zombie;
    int resistencia_casa;
    int frecuencia_sol;
    int frecuencia_disparo;
} Configuracion;

typedef struct {
    int filas;
    int columnas;
    int **matriz;
} Mapa;

Configuracion cargar_configuracion(const char *nombre_archivo);
Mapa cargar_mapa(const char *nombre_archivo);
void liberar_mapa(Mapa *mapa);
void imprimir_mapa(const Mapa *mapa);

#endif
