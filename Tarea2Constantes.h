#pragma once
#include "Tarea1Constantes.h"

void mover_zombies(Mapa *mapa, int** matriz_vida, int turno);
void ataque_lanzaguisantes(Mapa *mapa, int turno, Configuracion config, int** matriz_vida);
int verificar_victoria(Mapa *mapa);
int spawnear_zombies(Mapa *mapa, int** matriz_vida, Configuracion config);
int** matriz_vida(Mapa *mapa, Configuracion config);