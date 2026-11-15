#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "Tarea1Constantes.h"

//declaro mis funciones de la tarea 2



//logica general
int** matriz_vida(Mapa *mapa, Configuracion config);
int verificar_victoria (Mapa *mapa);

//logica guisantes
int** matriz_guisantes(Mapa *mapa);
void mover_guisantes(Mapa *mapa, int** matriz_vida, int** matriz_guisantes);
void imprimir_matriz_guisantes(int** matriz_guisantes, Mapa *mapa);
void buscar_zombies_atacar(Mapa *mapa, int turno, Configuracion config, int** matriz_guisantes);

//logica zombies
int spawnear_zombies(Mapa *mapa, int** matriz_vida, Configuracion config, int ronda);
void mover_zombies(Mapa *mapa, int** matriz_vida, int turno);

//logica soles
int** matriz_soles(Mapa *mapa);
void generar_soles(int** matriz_soles, int turno, Mapa *mapa, Configuracion config);
void imprimir_matriz_soles(int** matriz_soles, Mapa *mapa);

//logica patatapum
void patatapum_comportamiento(Mapa *mapa,int turno);

//logica tumbas y cometumbas
int** matriz_tumbas(Mapa *mapa) ;
void cometumbas_comportamiento(Mapa *mapa, int** matriz_tumbas,int turno);
void imprimir_matriz_tumbas(int** matriz_tumbas, Mapa *mapa);

void ronda_jugable(Mapa *mapa, int turno, Configuracion config, int** matriz_guisantes, int** matriz_soles, int** matriz_vida, int ronda, int** matriz_tumbas){

    printf("\n bucle de juego \n");

    //si esta variable es 1 el juego se detiene
    int continua = 0; 

    //la cantidad de zombies que spawnean por ronda
    int zombies_ronda = 20+(ronda-1)*10;


    //mientras aun no se gana o se pierde se sigue jugando
    while(continua != 1 && continua != 2){
        printf("\n--- TURNO %d ---\n", turno);

        //genero los soles
        generar_soles(matriz_soles,turno, mapa, config);
        
        //compruebo las cometumbas
        cometumbas_comportamiento(mapa, matriz_tumbas ,turno);

        //muevo los zombies o atacan
        mover_zombies(mapa, matriz_vida, turno);

        //los lanzaguisantes revisan si hay un zombie al cual atacar en su fila y si es así atacan dependiendo de su cooldown de ataque
        buscar_zombies_atacar(mapa, turno, config, matriz_guisantes);

        //mueve los guisantes o impactan si tocan un zombie
        mover_guisantes(mapa, matriz_vida, matriz_guisantes);

        //sube a las patatapum necesarias
        patatapum_comportamiento(mapa, turno);

        //imprime el mapa principal
        imprimir_mapa(mapa);

        //imprime el mapa de los guisantes
        imprimir_matriz_guisantes(matriz_guisantes, mapa);

        //imprime el mapa de los soles
        imprimir_matriz_soles(matriz_soles,mapa);

        //imprime el mapa de las tumba
        imprimir_matriz_tumbas(matriz_tumbas, mapa);

        
        
        int estado = verificar_victoria(mapa);

        if (estado == 2) {
            
            continua = 2;
            printf("DERROTA");
        } 
        else if (estado == 1 && zombies_ronda <= 0) {
        
            continua = 1;
            printf("VICTORIA");
        }

        if (turno % 3 == 0 && zombies_ronda > 0) {
            int generados = spawnear_zombies(mapa, matriz_vida, config, ronda);
            zombies_ronda -= generados;
        }

        turno++;
        sleep(1);
    }



    liberar_mapa(mapa);

    //libero las matrices dinamicas creadas
    for (int y = 0; y < mapa->filas; y++) {
        free(matriz_vida[y]);
        free(matriz_guisantes[y]);
    }
    free(matriz_vida);
    free(matriz_guisantes);

    printf("\n memoria liberada correctamente \n");
}

int main() {
    printf("Probando lectura de configuracion \n");
    // se usa la funcion para cargar config.txt
    Configuracion config = cargar_configuracion("Logica.txt");
    // se imprimen los valores leidos
    printf("Oleadas: %d Vida Zombie: %d Vida Casa: %d\n\n", config.max_oleadas, config.resistencia_zombie, config.resistencia_casa);

    printf("=== probando lectura y memoria de mapa ===\n");
    // se carga la matriz del mapa de mapa.txt
    Mapa mapa = cargar_mapa("Logica.txt");
    printf("Dimensiones del jardin: %d filas x %d columnas\n", mapa.filas, mapa.columnas);
    printf("matriz cargada en ram:\n");
    // se muestra el mapa en pantalla
    imprimir_mapa(&mapa);


    //=================================================
    //logica de juego jejejeje (tarea 2)
    //===============================================

    //declaro una matriz de vida
    int **matriz_vidas = matriz_vida(&mapa, config);
    //declaro una matriz de guisantes
    int **matriz_guis = matriz_guisantes(&mapa);
    //declaro una matriz de soles
    int** matriz_sol = matriz_soles(&mapa);
    //declaro la matriz de las tumbas
    int** matriz_tumba = matriz_tumbas(&mapa);

    // para ir aumentando el numero de turnos cada turno;
    int turno = 1;

    //este deben colocar en su codigo para jugar una ronda
    ronda_jugable(&mapa, turno, config, matriz_guis, matriz_sol, matriz_vidas, 1, matriz_tumba);

    return 0;
}