#include <stdio.h>
#include "Tarea1Constantes.h"
#include <stdlib.h>

//funcion para una matriz invisible que controla la vida de todas las entidades
int** matriz_vida(Mapa *mapa, Configuracion config) {
    
    //reservo la memoria de la matriz
    int **matriz_vida = (int **)malloc(mapa->filas * sizeof(int *));
    
    //reviso de arriba a abajo
    for (int y = 0; y < mapa->filas; y++) {

        //reservo la memoria para las columnas??, tuve que preguntar esto pues la verdad no tengo claro porque va acá
        matriz_vida[y] = (int *)malloc(mapa->columnas * sizeof(int));

        //reviso de izquierda a derecha
        for (int x = 0; x < mapa->columnas; x++) {
            
            //acá guardo que entidad hay en cada casila
            int entidad = mapa->matriz[y][x];

            //toda esta seccion es solo yo asignando la vida a cada entidad
            if (entidad == zombie_basico) {
                matriz_vida[y][x] = config.resistencia_zombie; 
            } 
            else if (entidad == zombie_flag) {
                matriz_vida[y][x] = 2*config.resistencia_zombie;
            } 
            else if (entidad == lanzaguisantes || entidad == girasol) {
                matriz_vida[y][x] = 5; 
            }
            else if (entidad == nuez) {
                matriz_vida[y][x] = 5; 
            }
            else if (entidad == obstaculo) {
                matriz_vida[y][x] = 20;
            }
            else {
                matriz_vida[y][x] = 0;
            }
        }
    }
    
    // Devolvemos la matriz lista para usar
    return matriz_vida;
}

//funcion para crear una matriz que muestra la posicion de los guisantes en cada momento
int** matriz_guisantes(Mapa *mapa) {
    
    //reservo la memoria de la matriz
    int **matriz_guisantes = (int **)malloc(mapa->filas * sizeof(int *));
    
    //reviso de arriba a abajo
    for (int y = 0; y < mapa->filas; y++) {

        //reservo la memoria para las columnas??, tuve que preguntar esto pues la verdad no tengo claro porque va acá
        matriz_guisantes[y] = (int *)malloc(mapa->columnas * sizeof(int));

        //reviso de izquierda a derecha
        for (int x = 0; x < mapa->columnas; x++) {

            //lleno la matriz de ceros para poder trabajar con ella
            matriz_guisantes[y][x] = 0;

        }
    }
    return matriz_guisantes;
}

//funcion para crear una matriz que muestra la posicion los soles en cada momento
int** matriz_soles(Mapa *mapa) {
    
    //reservo la memoria de la matriz
    int **matriz_soles = (int **)malloc(mapa->filas * sizeof(int *));
    
    //reviso de arriba a abajo
    for (int y = 0; y < mapa->filas; y++) {

        //reservo la memoria para las columnas??, tuve que preguntar esto pues la verdad no tengo claro porque va acá
        matriz_soles[y] = (int *)malloc(mapa->columnas * sizeof(int));

        //reviso de izquierda a derecha
        for (int x = 0; x < mapa->columnas; x++) {

            //lleno la matriz de ceros para poder trabajar con ella
            matriz_soles[y][x] = 0;

        }
    }
    return matriz_soles;
}

//funcion que en cada turno mueve a los zombies o los hace comer
void mover_zombies(Mapa *mapa, int** matriz_vida,int turno) {

    //veo de arriba a abajo
    for (int y = 0; y < mapa->filas; y++) {
        
        // veo de izquierda a derecha
        for (int x = 0; x < mapa->columnas; x++) {
            
            // reviso si en la casilla [y][x] hay un zombie basico
            if (mapa->matriz[y][x] == zombie_basico) {
                

                //reviso si la casilla siguiente es una planta para atacarla si ese es el caso
                if (x > 0 && (mapa->matriz[y][x-1] == nuez || mapa->matriz[y][x-1] == girasol || mapa->matriz[y][x-1] == lanzaguisantes)) {
                    
                    //le baja vida a la planta
                    matriz_vida[y][x-1] = matriz_vida[y][x-1] - 2;

                    //si la planta esta muerta la destruye
                    if (matriz_vida[y][x-1] <= 0){
                        mapa->matriz[y][x-1] = casilla_vacia;
                        
                    }

                }

                //esto esta aquí para que siempre coma pero solo en uno de cada 2 turnos avance
                else if (turno % 2 != 0){
                    return;
                }

                // reviso si la casilla siguiente al zombie basico esta vacia en cuyo caso:
                else if (x > 0 && mapa->matriz[y][x - 1] == casilla_vacia) {

                    // muevo el zombie basico a la izquierda
                    mapa->matriz[y][x - 1] = zombie_basico; 
                    // dejo limpia la casilla actual
                    mapa->matriz[y][x] = casilla_vacia;    
                    //muevo la vida del zombie a la izquierda
                    matriz_vida[y][x-1] = matriz_vida[y][x];
                    //dejo limpia la casilla actual de la vida
                    matriz_vida[y][x] = 0;      

                }
            }


            // no comento esta parte porque es como exactamente la misma logica de antes pero para el zombie abanderado
            if (mapa->matriz[y][x] == zombie_flag) {
                if (x > 0 && mapa->matriz[y][x - 1] == casilla_vacia) {
                    mapa->matriz[y][x - 1] = zombie_flag; 
                    mapa->matriz[y][x] = casilla_vacia;    
                    matriz_vida[y][x-1] = matriz_vida[y][x];
                    matriz_vida[y][x] = 0;      
                }
                else if (x > 0 && (mapa->matriz[y][x-1] == nuez || mapa->matriz[y][x-1] == girasol || mapa->matriz[y][x-1] == lanzaguisantes)) {
                    matriz_vida[y][x-1] = matriz_vida[y][x-1] - 1;
                    if (matriz_vida[y][x-1] <= 0){
                        mapa->matriz[y][x-1] = casilla_vacia;
                        matriz_vida[y][x-1] = 0;
                    }
                }
            }
        }
    }
}

//funcion que revisa si los lanza guisantes deben atacar y si es así lanza un guisante
void buscar_zombies_atacar (Mapa *mapa, int turno, Configuracion config, int** matriz_guisantes) {

    //entonces si no estamos en un turno divisor exacto de la frecuencia de disparo del lanzaguisantes no disparara, cosa que por ejemplo:
    
    if (turno % config.frecuencia_disparo != 0) {
        return;
    }

    //acá guardo como la posicion que estoy buscando para atacar
    int index;
    //acá guardo si se encontro un zombie o no
    int encontrar_zombie;

    //veo de arriba a abajo
    for (int y = 0; y < mapa->filas; y++) {
        
        // veo de izquierda a derecha
        for (int x = 0; x < mapa->columnas; x++) {
            
            // reviso si en la casilla [y][x] hay un lanzaguisantes
            if (mapa->matriz[y][x] == lanzaguisantes) {
                
                //reviso desde la posicion del lanzaguisantes si hay un zombie
                index = x ;

                //si esto es cero no se ha encontrado un zombie y se sigue buscando, si es 1 deja de buscar y lo asesina
                encontrar_zombie = 0;
                
                //mientras no se haya encontrado un zombie y no se haya llegado al final
                while(index<9 && encontrar_zombie == 0){

                    //si la casilla es un zombie lanza un guisante y deja de buscar
                    if(mapa->matriz[y][index]==zombie_basico || mapa->matriz[y][index]==zombie_flag || mapa->matriz[y][index]==obstaculo){

                        matriz_guisantes[y][x] = guisante ;
                        encontrar_zombie = 1;
                    }

                    //pasa a la siguiente posicion
                    index++;

                }
                


            }
        }
    }
}

//funcion que mueve los guisantes hacia los zombies y les hace daño
void mover_guisantes (Mapa *mapa ,int** matriz_vida, int** matriz_guisantes){

    //veo de arriba a abajo
    for (int y = 0; y < mapa->filas; y++) {
        
        // veo de izquierda a derecha(para no mover el mismo guisante 2 veces ni nada por el estilo)
        for (int x = mapa->columnas-1; x > -1; x--) {    

            // verifico si hay un guisante en una posicion
            if(matriz_guisantes[y][x] == guisante){
                
                // Si el guisante está en la última columna, lo eliminamos para que no explote todo HUOCDHASJB
                if(x + 1 >= mapa->columnas){
                    matriz_guisantes[y][x] = casilla_vacia;
                    continue; 
                }
                
                // Revisamos si hay un zombie en la casilla actual
                //si es asi
                if(mapa->matriz[y][x] == zombie_basico || mapa->matriz[y][x] == zombie_flag || mapa->matriz[y][x] == obstaculo){
                    
                    //destruyo el guisante
                    matriz_guisantes[y][x] = casilla_vacia;
                    //le hago daño al zombie
                    matriz_vida[y][x] = matriz_vida[y][x] - 5;

                    //si la entidad muere la asesino
                    if (matriz_vida[y][x] <= 0){
                        matriz_vida[y][x] = 0;
                        mapa->matriz[y][x] = casilla_vacia;
                    }
                }

                //exactamente lo mismo pero lo pruebo para el zombie de en frente
                //lo hago 2 veces por si requiero como cambiar el orden de las funciones en main.c
                //ya sufri por no pensar en ese casos
                else if(mapa->matriz[y][x+1] == zombie_basico || mapa->matriz[y][x+1] == zombie_flag || mapa->matriz[y][x+1] == obstaculo){
                    matriz_guisantes[y][x] = casilla_vacia;
                    matriz_vida[y][x+1] = matriz_vida[y][x+1] - 5;
                    if (matriz_vida[y][x+1] <= 0){
                        matriz_vida[y][x+1] = 0;
                        mapa->matriz[y][x+1] = casilla_vacia;
                    }
                }

                //si no hay nada en frente muevo el guisante simplemente
                else {
                    //asesino al guisante
                    matriz_guisantes[y][x] = casilla_vacia;
                    //coloco un guisante en la siguiente posicion
                    matriz_guisantes[y][x+1] = guisante;
                }
            }
        }
    }
}

//funcion que se activa cada turno para verificar si ganaste o perdiste
int verificar_victoria (Mapa *mapa){
    

    //en esta función verificare si debo parar el juego y en ese caso quien gano

    //verifico derrota

    //si este valor es cero no hay derrota y si es uno si lo hay
    int derrota = 0;

    for(int m = 0; m < 5; m++){
        
        //verifico si algun zombie esta en la ultima fila, si es así perdemos todos
        if(mapa->matriz[m][0] == zombie_basico || mapa->matriz[m][0] == zombie_flag){
            derrota = 1;

        }
    }

    //verifico victoria
    int victoria = 1;

    //si no hay ningun zombie ganamos yeiiii
    for(int x=0; x<9; x++){
        for(int y=0; y<5; y++){
        
            if(mapa->matriz[y][x] == zombie_basico || mapa->matriz[y][x] == zombie_flag){
                victoria = 0;
            }
        }
    }

    if(victoria == 1){
        return 1;
    }
    else if(derrota == 1){
        return 2;
    }
    else{
        return 0;
    }
    
}

//funcion que sirve para spawnear zombies 
int spawnear_zombies(Mapa *mapa, int** matriz_vida, Configuracion config){


    //spawnea a lo largo de toda la ultima columna
    for(int r = 0; r < 5; r++){

        //probabilidades
        int probabilidad_aparicion = rand()%100;
        int probabilidad_zombie = rand()%3;

        //como va por oleadas y no tienen como zombies infinitos
        //voy contando cuantos zombies aparecen para restarselos al total de zombies de la ronda
        int spawneados = 0;

        //un 5% de posibilidad por casilla de spawnear un zombie
        if(probabilidad_aparicion < 25){

            //un 33% de spawnear un zombie bandera y un 66% de spawnear un zombie normal
            if(probabilidad_zombie == 0 || probabilidad_zombie == 2){
                mapa->matriz[r][8] = 1;
                matriz_vida[r][8] = config.resistencia_zombie;
            }
            else if(probabilidad_zombie == 1){
                mapa->matriz[r][8] = 2;
                matriz_vida[r][8] = 2*config.resistencia_zombie;
            }
            spawneados++;
        
        if(r==4){
            return spawneados;
        }
        }
    }
    return 0;
}

//funcion para imprimir la matriz guisantes en cada turno
void imprimir_matriz_guisantes(int** matriz_guisantes, Mapa *mapa){

    printf("matriz guisantes");
    printf("\n");

    for(int y = 0; y < mapa->filas; y++){
        for(int x = 0; x < mapa->columnas; x++){

            printf("%i ", matriz_guisantes[y][x]);


        }
        printf("\n");
    }

}

//funcion para generar soles aleatorios en una casilla aleatoria del mapa
void generar_soles(int** matriz_soles, int turno, Mapa *mapa, Configuracion config){

    //parte que genera los soles aleatorios que caen del cielo

    //genera un sol aleatorio cada 10 turnos
    if(turno % 10 == 0){

        //genera una posicion aleatoria
        int x_sol = rand()%mapa->columnas;
        int y_sol = rand()%mapa->filas;

        //coloca un sol en el lugar previamente colocado
        matriz_soles[y_sol][x_sol] = matriz_soles[y_sol][x_sol] + 1;

    }

    //parte que spawnea un sol en un girasol cada x turnos

    if(turno % config.frecuencia_sol == 0){

        for(int y = 0; y < mapa->filas; y++){

            for(int x = 0; x < mapa->columnas; x++){

                if(mapa->matriz[y][x] == girasol){

                    matriz_soles[y][x] = matriz_soles[y][x] + 1;

                }
            }
        }

    }

}

//funcion para imprimir la matriz soles en cada turno
void imprimir_matriz_soles(int** matriz_soles, Mapa *mapa){

    printf("matriz soles");
    printf("\n");

    for(int y = 0; y < mapa->filas; y++){
        for(int x = 0; x < mapa->columnas; x++){

            printf("%i ", matriz_soles[y][x]);


        }
        printf("\n");
    }

}