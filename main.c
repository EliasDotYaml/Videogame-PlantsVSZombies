#include <stdio.h>
#include <unistd.h>
#include "Tarea1Constantes.h"

//declaro mis funciones de la tarea 2
void mover_zombies(Mapa *mapa, int** matriz_vida,int turno);
void ataque_lanzaguisantes (Mapa *mapa, int turno, Configuracion config,int** matriz_vida);
int** matriz_vida(Mapa *mapa, Configuracion config);
int verificar_victoria (Mapa *mapa);
int spawnear_zombies(Mapa *mapa, int** matriz_vida, Configuracion config);


int main() {
    printf("Probando lectura de configuracion \n");
    // se usa la funcion para cargar config.txt
    Configuracion config = cargar_configuracion("Logica.txt");
    // se imprimen los valores leidos
    printf("Oleadas: %d Vida Zombie: %d Vida Casa: %d\n\n", config.max_oleadas, config.resistencia_zombie, config.resistencia_casa);

    printf("=== probando lectura y memoria de mapa ===\n");
    // se carga la matriz del mapa de mapa.txt
    Mapa mapa = cargar_mapa("DimensionesMapa.txt");
    printf("Dimensiones del jardin: %d filas x %d columnas\n", mapa.filas, mapa.columnas);
    printf("matriz cargada en ram:\n");
    // se muestra el mapa en pantalla
    imprimir_mapa(&mapa);

    //declaro una matriz de vida
    int **matriz_vidas = matriz_vida(&mapa, config);




    //parte tarea 2

    printf("\n bucle de juego \n");

    int turno = 1;
    int continua = 0; 
    int zombies_ronda = 20;

    //mientras aun no se gana o se pierde se sigue jugando
    while(continua != 1 && continua != 2){
        printf("\n--- TURNO %d ---\n", turno);
        
        mover_zombies(&mapa, matriz_vidas,turno);
        ataque_lanzaguisantes(&mapa, turno, config, matriz_vidas);
        imprimir_mapa(&mapa);

        //tengo que modificar esto para subir la dificultad ejjejejejejje
        if(zombies_ronda != 20){
            continua = verificar_victoria(&mapa);
        }
        
        
        //intenta spawnear zombies cada 3 turnos
        if(turno%3==0){
            int generados = spawnear_zombies(&mapa, matriz_vidas, config);
            zombies_ronda = zombies_ronda - generados;
        }

        sleep(1);
        turno ++;
    }

    if(continua == 1){
        printf("victoria");
    }
    else if(continua == 2){
        printf("derrota");
    }

    liberar_mapa(&mapa);
    printf("\n memoria liberada correctamente \n");

    return 0;
}