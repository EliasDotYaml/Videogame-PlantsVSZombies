#ifndef PAQUETE1_H
#define PAQUETE1_H

#include "Tarea1Constantes.h"

// guardamos el estado actual de la matriz en un archivo de texto
int guardar_mapa(const char *nombre_archivo, const Mapa *mapa);

// redimensionamos la matriz manteniendo las casillas existentes
int redimensionar_mapa(Mapa *mapa, int nuevas_filas, int nuevas_columnas);

#endif // PAQUETE1_H