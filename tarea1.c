#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Tarea1Constantes.h"
#include <time.h>

// busca el archivo Logica.txt y guarda los valores
Configuracion cargar_configuracion(const char *nombre_archivo) {
    Configuracion config = {5, 10, 100, 3, 2, 30, 20, 200};
  
    // se intenta abrir el archivo en modo lectura "r"
    FILE *fp = fopen(nombre_archivo, "r");
    if (fp == NULL) {
        printf("Error: No se pudo abrir el archivo de configuracion '%s'\n", nombre_archivo);
        exit(EXIT_FAILURE); // si no existe el archivo se finaliza el programa
    }

    char clave[50]; // buffer para la palabra clave
    int valor;      // variable para guardar el numero del parametro

    // lee el nombre del parametro y su valor
    while(fscanf(fp, "%49s %d", clave, &valor) == 2) {
        // strcasecmp compara el texto sin importar si es mayuscula o minuscula
        if (strcasecmp(clave, "cantidad_oleadas") == 0) config.max_oleadas = valor;
        else if (strcasecmp(clave, "resistencia_zombie") == 0) config.resistencia_zombie = valor;
        else if (strcasecmp(clave, "resistencia_casa") == 0) config.resistencia_casa = valor;
        else if (strcasecmp(clave, "frecuencia_sol") == 0) config.frecuencia_sol = valor;
        else if (strcasecmp(clave, "frecuencia_disparo") == 0) config.frecuencia_disparo = valor;
        else if (strcasecmp(clave, "tiempo_preparacion") == 0) config.tiempo_preparacion = valor;
        else if (strcasecmp(clave, "tiempo_descanso") == 0) config.tiempo_descanso = valor;
        else if (strcasecmp(clave, "soles_iniciales") == 0) config.soles_iniciales = valor;
    }

    // se cierra el archivo
    fclose(fp);
    return config;
}

// funcion que lee el mapa y pide la memoria dinamica
Mapa cargar_mapa(const char *nombre_archivo, int *soles_disponibles) {
    Mapa mapa;
    mapa.filas = 0;
    mapa.columnas = 0;
    mapa.matriz = NULL;

    // se abre el archivo de texto del mapa
    FILE *fp = fopen(nombre_archivo, "r");
    if (fp == NULL) {
        printf("Error: No se puedo abrir el archivo del mapa '%s'\n", nombre_archivo);
        return mapa; // retorna matriz = NULL para usar el respaldo
    }

    // busca la palabra "MAPA" en el archivo para asegurarse de que el formato es correcto
    char palabra[50];
    int encontrado = 0; // bandera para saber si se encontro la palabra "mapa"
    while (fscanf(fp, "%49s", palabra) == 1) {
        if (strcasecmp(palabra, "SOLES") == 0) {
            fscanf(fp, "%d", soles_disponibles);
        } else if (strcasecmp(palabra, "MAPA") == 0) {
            encontrado = 1;
            break;
        }
    }

    // lee las dimensiones del mapa y verifica si se pudo leer correctamente
    if (!encontrado || fscanf(fp, "%d %d", &mapa.filas, &mapa.columnas) != 2) { // si no se encontro la palabra "mapa" o no se pudieron leer las dimensiones, se muestra un error
        printf("Error: Formato de mapa invalido en el archivo. \n");
        fclose(fp);
        exit(EXIT_FAILURE);
    }

    // reserva memoria para el arreglo de filas
    mapa.matriz = (int **)malloc(mapa.filas *sizeof(int *));
    if (mapa.matriz == NULL) {
        printf("Error de memoria al asignar las filas del mapa. \n");
        fclose(fp);
        exit(EXIT_FAILURE);
    }

    // reserva memoria para las columnas de cada fila
    for (int i = 0; i < mapa.filas; i++) {
        mapa.matriz[i] = (int *)malloc(mapa.columnas * sizeof(int));
        // si malloc falla, se libera la memoria para evitar fugas
        if (mapa.matriz[i] == NULL) {
            printf("Error de memoria al asignar la columna %d. \n", i);
            for (int k = 0; k < i; k++) free(mapa.matriz[k]);
            free(mapa.matriz);
            fclose(fp);
            exit(EXIT_FAILURE);
        }

        // se leen los numeros de la fila actual y se guarda en la matriz
        for (int j = 0; j < mapa.columnas; j++) {
            fscanf(fp, "%d", &mapa.matriz[i][j]);
        }
    }

    fclose(fp);
    return mapa;
}

// libera la memoria dinamica del mapa
void liberar_mapa(Mapa *mapa) {
    if (mapa != NULL && mapa->matriz != NULL) {
        for (int i = 0; i < mapa->filas; i++) {
            free(mapa->matriz[i]);
        }
        free(mapa->matriz);
        mapa->matriz = NULL;
        mapa->filas = 0;
        mapa->columnas = 0;
    }
}

// imprime el mapa en la consola
void imprimir_mapa(const Mapa *mapa) {
    if (mapa == NULL || mapa->matriz == NULL) return;
    for (int i = 0; i < mapa->filas; i++) {
        for (int j = 0; j < mapa->columnas; j++) {
            printf("%d ", mapa->matriz[i][j]);
        }
        printf("\n");
    }
}

// colocamos una planta en el mapa y restamos los soles disponibles
int colocar_planta(Mapa *mapa, int **matriz_vida, int fila, int columna, int tipo_planta, int *soles_disponibles) {
    if (mapa == NULL || mapa->matriz == NULL) return 0; // error si el mapa o la matriz de vida es NULL
    if (fila < 0 || fila >= mapa->filas || columna < 0 || columna >= mapa->columnas) return 0; // error si la posición está fuera del mapa
    if (mapa->matriz[fila][columna] != casilla_vacia) return 0; // error si la casilla no está vacía
    int costo = 0;
    int vida_inicial = 0;
    switch (tipo_planta) {
        case girasol:
            costo = costo_girasol;
            vida_inicial = 5; // vida inicial del girasol
            break;
        case lanzaguisantes:
            costo = costo_lanzaguisantes;
            vida_inicial = 5; // vida inicial del lanzaguisantes
            break;
        case nuez:
            costo = costo_nuez;
            vida_inicial = 50; // vida inicial de la nuez
            break;
        case patatapum_abajo:
        case patatapum_arriba:
            costo = costo_patatapum;
            vida_inicial = 8; // vida inicial del patatapum
            break;
        case cometumbas:
            costo = costo_cometumbas;
            vida_inicial = 8;
            break;
        default:
            return 0; // error si el tipo de planta no es válido
    }
    if (soles_disponibles != NULL) {
        if (*soles_disponibles < costo) {
            return 0; // error si no hay suficientes soles
        }
        *soles_disponibles -= costo; // restamos el costo de la planta a los soles disponibles
    }
    mapa->matriz[fila][columna] = tipo_planta; // colocamos la planta en el mapa
    if (matriz_vida != NULL) {
        matriz_vida[fila][columna] = vida_inicial; // asignamos la vida inicial a la planta
    }
    return 1; // éxito al colocar la planta
}

// se guarda la matriz actual en un archivo con el formato estandar
int guardar_mapa(const char *nombre_archivo, const Mapa *mapa, int soles_disponibles) {
    if (mapa == NULL || mapa->matriz == NULL) {
        printf("Error: el mapa no esta iniciado\n");
        return 0;
    }

    // abrimos el archivo en modo escritura
    FILE *fp = fopen(nombre_archivo, "w");
    if (fp == NULL) {
        printf("Error: no se pudo crear el archivo %s\n", nombre_archivo);
        return 0;
    }

    fprintf(fp, "SOLES%d\n", soles_disponibles); //
    fprintf(fp, "MAPA\n"); // se escribe MAPA al inicio del archivo
    fprintf(fp, "%d %d\n", mapa->filas, mapa->columnas); // escribimos las dimensiones actuales

    // escribimos el valor de cada casilla
    for (int i = 0; i < mapa->filas; i++) {
        for (int j = 0; j < mapa->columnas; j++) {
            fprintf(fp, "%d ", mapa->matriz[i][j]);
        }
        fprintf(fp, "\n");
    }

    fclose(fp);
    printf("Estado del juego guardado exitosamente en '%s'\n", nombre_archivo);
    return 1;
}

// redimensionamos el jardin a un nuevo tamaño
int redimensionar_mapa(Mapa *mapa, int nuevas_filas, int nuevas_columnas) {
    if (mapa == NULL || nuevas_filas <= 0 || nuevas_columnas <= 0) {
        printf("Error: dimensiones invalidas para redimensionar\n");
        return 0;
    }

    // reservamos espacio para la nueva matriz
    int **nueva_matriz = (int **)malloc(nuevas_filas * sizeof(int *));
    if (nueva_matriz == NULL) {
        printf("Error en malloc al redimensionar filas\n");
        return 0;
    }

    for (int i = 0; i < nuevas_filas; i++) {
        nueva_matriz[i] = (int *)malloc(nuevas_columnas * sizeof(int));
        if (nueva_matriz[i] == NULL) {
            printf("Error en malloc al redimensionar columnas\n");
            for (int k = 0; k < i; k++) free(nueva_matriz[k]);
            free(nueva_matriz);
            return 0;
        }

        // copiamos lo que habia antes o ponemos casillas vacias si la matriz crecio
        for (int j = 0; j < nuevas_columnas; j++) {
            if (i < mapa->filas && j < mapa->columnas && mapa->matriz != NULL) {
                nueva_matriz[i][j] = mapa->matriz[i][j]; // se mantiene lo que habia
            } else {
                nueva_matriz[i][j] = casilla_vacia; // casilla limpia en lo nuevo
            }
        }
    }

    // liberamos la matriz antigua
    if (mapa->matriz != NULL) {
        for (int i = 0; i < mapa->filas; i++) {
            free(mapa->matriz[i]);
        }
        free(mapa->matriz);
    }

    // actualizamos los datos del mapa con la nueva matriz
    mapa->matriz = nueva_matriz;
    mapa->filas = nuevas_filas;
    mapa->columnas = nuevas_columnas;

    printf("Mapa redimensionado a %dx%d con exito\n", nuevas_filas, nuevas_columnas);
    return 1;
}

// funcion para generar un mapa aleatorio con las dimensiones dadas
Mapa generar_mapa_aleatorio(int filas, int columnas) {   
    Mapa mapa;
    mapa.filas = filas;
    mapa.columnas = columnas;

    // reserva memoria para el arreglo de filas
    mapa.matriz = (int **)malloc(filas * sizeof(int *));
    if (mapa.matriz == NULL) {
        exit(EXIT_FAILURE);
    }

    // reserva memoria para las columnas de cada fila
    for (int i = 0; i < filas; i++) {
        mapa.matriz[i] = (int *)malloc(columnas * sizeof(int));
        if (mapa.matriz[i] == NULL) {
            for (int k = 0; k < i; k++) free(mapa.matriz[k]);
            free(mapa.matriz);
            exit(EXIT_FAILURE);
        }
    
        // llena la matriz con valores aleatorios entre 0 y 4
        for (int j = 0; j < columnas; j++) {
            int elemento = casilla_vacia; // valor por defecto para la casilla
            int probabilidad = rand() % 100; // genera un número aleatorio entre 0 y 99

            if (j < columnas-1 && probabilidad < 10) {
                elemento = obstaculo; // 10% de probabilidad de colocar un obstáculo en las demás columnas
            }

            mapa.matriz[i][j] = elemento; // asigna el valor a la casilla correspondiente
        }
    }

    return mapa;
} 

// logica de fases: preparacion -> oleadas -> descanso

EstadoRondas iniciar_estado_rondas(Configuracion config) {
    EstadoRondas estado;
    estado.fase_actual = fase_preparacion;
    estado.ronda_actual = 1;
    estado.tiempo_restante_fase = config.tiempo_preparacion;
    estado.zombies_por_spawnear = 0;
    return estado;
}

// devuelve 1 si el jugador tiene permitido plantar, 0 si esta en oleada
int puede_plantar(const EstadoRondas *estado) {
    if (estado == NULL) return 1;
    return (estado ->fase_actual == fase_preparacion || estado->fase_actual == fase_descanso);
}

int hay_zombies_en_mapa(const Mapa *mapa) {
    if (mapa == NULL || mapa->matriz == NULL) return 0;
    for (int y = 0; y < mapa->matriz; y++) {
        for (int x = 0; x < mapa->columnas; x++) {
            int celda = mapa->matriz[y][x];
            if (celda == zombie_basico || celda == zombie_flag || celda == zombie_cono || celda == zombie_cubo) {
                return 1;
            }
        }
    }
    return 0;   
}

// Debe llamarse una vez por turno (o cada segundo en SDL) para avanzar las fases
void actualizar_estado_rondas(EstadoRondas *estado, const Mapa *mapa, Configuracion config) {
    if (estado == NULL || mapa == NULL) return;

    if (estado->fase_actual == FASE_PREPARACION) {
        estado->tiempo_restante_fase--;
        if (estado->tiempo_restante_fase <= 0) {
            // Termina la preparacion: empieza la Oleada 1
            estado->fase_actual = FASE_OLEADA;
            estado->zombies_por_spawnear = 10 + (estado->ronda_actual - 1) * 5;
        }
    } 
    else if (estado->fase_actual == FASE_OLEADA) {
        // La oleada termina SOLO cuando ya no quedan zombies por salir Y todos en el mapa murieron
        if (estado->zombies_por_spawnear <= 0 && !hay_zombies_en_mapa(mapa)) {
            if (estado->ronda_actual >= config.max_oleadas) {
                estado->fase_actual = FASE_VICTORIA;
            } else {
                estado->fase_actual = FASE_DESCANSO;
                estado->tiempo_restante_fase = config.tiempo_descanso;
            }
        }
    } 
    else if (estado->fase_actual == FASE_DESCANSO) {
        estado->tiempo_restante_fase--;
        if (estado->tiempo_restante_fase <= 0) {
            // Termina el descanso: pasamos a la siguiente ronda y comienza la nueva oleada
            estado->ronda_actual++;
            estado->fase_actual = FASE_OLEADA;
            estado->zombies_por_spawnear = 10 + (estado->ronda_actual - 1) * 5;
        }
    }
}

// Wrapper seguro que respeta la regla de no plantar durante las oleadas
int colocar_planta_en_ronda(Mapa *mapa, int **matriz_vida, int fila, int columna, int tipo_planta, int *soles_disponibles, const EstadoRondas *estado) {
    if (!puede_plantar(estado)) {
        return 0; // Bloqueado: estamos en medio de una oleada
    }
    return colocar_planta(mapa, matriz_vida, fila, columna, tipo_planta, soles_disponibles);
}
