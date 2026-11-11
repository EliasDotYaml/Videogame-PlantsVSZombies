#pragma once
#include "Tarea1Constantes.h"

//logica general
int** matriz_vida(Mapa *mapa, Configuracion config);
int verificar_victoria (Mapa *mapa);

//logica guisantes
int** matriz_guisantes(Mapa *mapa);
void mover_guisantes(Mapa *mapa, int** matriz_vida, int** matriz_guisantes);
void imprimir_matriz_guisantes(int** matriz_guisantes, Mapa *mapa);
void buscar_zombies_atacar(Mapa *mapa, int turno, Configuracion config, int** matriz_guisantes);

//logica zombies
int spawnear_zombies(Mapa *mapa, int** matriz_vida, Configuracion config);
void mover_zombies(Mapa *mapa, int** matriz_vida, int turno);

//logica soles
int** matriz_soles(Mapa *mapa);
void generar_soles(int** matriz_soles, int turno, Mapa *mapa, Configuracion config);
void imprimir_matriz_soles(int** matriz_soles, Mapa *mapa);