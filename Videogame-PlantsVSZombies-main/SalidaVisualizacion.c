#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3/SDL_image.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "VisualizacionConstantes.h"
#include "Tarea1Constantes.h"
#include "Tarea2Constantes.h"

bool jugando = true;

SDL_Window *ventana;
SDL_Renderer *renderizador;

SDL_Texture *cargarTextura(SDL_Renderer *renderizador, char *archivo){
    SDL_Texture *textura = IMG_LoadTexture(renderizador, archivo);
    if(textura == NULL){
        SDL_Log("Error al cargar %s: %s", archivo, SDL_GetError());
    }
    return textura;
}

int main(int argc, char *argv[]){

    //Cargar configuracion y mapa usando las funciones existentes de la tarea1
    Configuracion config = cargar_configuracion("Logica.txt");
    Mapa mapa = cargar_mapa("DimensionesMapa.txt");

	//tarea2
    int **matriz_vida_juego = matriz_vida(&mapa, config);

    int filaJugador = 1;
    int columnaJugador = 1;

    // Matriz auxiliar para controlar la movilidad del seleccionador
    int movilidad[mapa.filas][mapa.columnas];
    for (int i = 0; i < mapa.filas; i++) {
        for (int j = 0; j < mapa.columnas; j++) {
            movilidad[i][j] = 0;
        }
    }

    //Validaciones
    if(!SDL_Init(SDL_INIT_VIDEO)){
      SDL_Log("Error al iniciar SDL: %s", SDL_GetError());
      return 1;
    }

    if(!SDL_CreateWindowAndRenderer(tituloventana, anchuravent, alturavent, 0, &ventana, &renderizador)){
      SDL_Log("Error al iniciar ventana: %s", SDL_GetError());
      SDL_Quit();
      return 1;
    }

    //Cargamos assets
    SDL_Texture *texturaJardin = cargarTextura(renderizador, "Fotografias/Jardin.jpg");
    SDL_Texture *texturaJugador = cargarTextura(renderizador, "Fotografias/Seleccionador.png");
	SDL_Texture *texturaZombie = cargarTextura(renderizador, "Fotografias/Zombie.png");
	SDL_Texture *texturaAbanderado = cargarTextura(renderizador, "Fotografias/FlagZ.png");
	SDL_Texture *texturaLanzaguisantes = cargarTextura(renderizador, "Fotografias/Lanzaguisantes.png");
	SDL_Texture *texturaGirasol = cargarTextura(renderizador, "Fotografias/Girasol.png");
	SDL_Texture *texturaNuez = cargarTextura(renderizador, "Fotografias/Nuez.png");
	SDL_Texture *texturaGuisante = cargarTextura(renderizador, "Fotografias/Guisante.png");
	SDL_Texture *texturaSol = cargarTextura(renderizador, "Fotografias/Sol.png");
	SDL_Texture *texturaTumba = cargarTextura(renderizador, "Fotografias/Tumba.png");

    if(!texturaJardin || !texturaJugador){
        SDL_DestroyRenderer(renderizador);
        SDL_DestroyWindow(ventana);
        SDL_Quit();
        return 1;
    }
    
    SDL_Event evento;
	//tarea2, se asignan cambios en la matriz cada ciertos ticks
    int turno = 1;
    Uint64 ultimo_tiempo = SDL_GetTicks(); 
    const Uint64 TIEMPO_TURNO = 1000; // 1 segundo por turno

    //Bucle de juego
    while(jugando){

        //Teclado y Ventana
        while(SDL_PollEvent(&evento)){
            if(evento.type == SDL_EVENT_QUIT){
                jugando = false;
            }
            
            if(evento.type == SDL_EVENT_KEY_DOWN && !evento.key.repeat){
                int siguienteFila = filaJugador;
                int siguienteColumna = columnaJugador;
                
                switch(evento.key.scancode){
                    case SDL_SCANCODE_W:
                    case SDL_SCANCODE_UP:
                        siguienteFila--;
                        break;

                    case SDL_SCANCODE_S:
                    case SDL_SCANCODE_DOWN:
                        siguienteFila++;
                        break;

                    case SDL_SCANCODE_A:
                    case SDL_SCANCODE_LEFT:
                        siguienteColumna--;
                        break;

                    case SDL_SCANCODE_D:
                    case SDL_SCANCODE_RIGHT:
                        siguienteColumna++;
                        break;

                    case SDL_SCANCODE_ESCAPE:
                        jugando = false;
                        break;

                    default:
                        break;
                }

                if(siguienteFila >= 1 && siguienteFila <= mapa.filas && siguienteColumna >= 1 && siguienteColumna <= mapa.columnas && movilidad[siguienteFila - 1][siguienteColumna - 1] == 0){
                    filaJugador = siguienteFila;
                    columnaJugador = siguienteColumna;
                }
            }
        }

        //Logica de turno, se ejecuta cada 1 segundo, tarea 2
        Uint64 tiempo_actual = SDL_GetTicks();
        if (tiempo_actual - ultimo_tiempo >= TIEMPO_TURNO) {
            mover_zombies(&mapa, matriz_vida_juego, turno);
            ataque_lanzaguisantes(&mapa, turno, config, matriz_vida_juego);
            spawnear_zombies(&mapa, matriz_vida_juego, config);

            int estado = verificar_victoria(&mapa);
            if (estado == 1) {
                SDL_Log("¡VICTORIA!");
            } else if (estado == 2) {
                SDL_Log("¡DERROTA!");
            }

            turno++;
            ultimo_tiempo = tiempo_actual;
        }

        SDL_RenderClear(renderizador);
        SDL_RenderTexture(renderizador, texturaJardin, NULL, NULL);

        // Recorremos la matriz real mapa.matriz siguiendo el struct de tarea1.h
        for(int i = 0; i < mapa.filas; i++){
            for(int j = 0; j < mapa.columnas; j++){
                
                SDL_FRect rectCasilla = {
                    EJE_X + (j * ANCHO_CASILLA),
                    EJE_Y + (i * ALTO_CASILLA),
                    ANCHO_CASILLA,
                    ALTO_CASILLA,
                };

                SDL_FRect rectZombie = {
                    EJE_X_ZOMBIE + (j * ANCHO_CASILLA), 
                    EJE_Y_ZOMBIE + (i * ALTO_CASILLA), 
                    ANCHO_ZOMBIE, 
                    ALTO_ZOMBIE,
                };

                SDL_FRect rectZombieAB = {
                    EJE_X_ZOMBIE_ABANDERADO + (j * ANCHO_CASILLA), 
                    EJE_Y_ZOMBIE_ABANDERADO + (i * ALTO_CASILLA), 
                    ANCHO_ZOMBIE_ABANDERADO, 
                    ALTO_ZOMBIE_ABANDERADO,
                };

                SDL_FRect rectPlanta = {
                    EJE_X_PLANTA + (j * ANCHO_CASILLA),
                    EJE_Y_PLANTA + (i * ALTO_CASILLA),
                    ANCHO_PLANTA,
                    ALTO_PLANTA,
                };

                SDL_FRect rectGirasol = {
                    EJE_X_GIRASOL + (j * ANCHO_CASILLA),
                    EJE_Y_GIRASOL + (i * ALTO_CASILLA),
                    ANCHO_GIRASOL,
                    ALTO_GIRASOL,
                };

                SDL_FRect rectProyectil = {
                    EJE_X_PROYECTIL + (j * ANCHO_CASILLA),
                    EJE_Y_PROYECTIL + (i * ALTO_CASILLA),
                    ANCHO_PROYECTIL,
                    ALTO_PROYECTIL,
                };

                //Usamos la nueva matriz mapa.matriz[i][j]
                int Valor_Posicion = mapa.matriz[i][j];

                switch(Valor_Posicion){
                    case 1:
                        SDL_RenderTexture(renderizador, texturaZombie, NULL, &rectZombie);
                        break;
                    case 2:
                        SDL_RenderTexture(renderizador, texturaAbanderado, NULL, &rectZombieAB);
                        break;
                    case 4:
                        SDL_RenderTexture(renderizador, texturaTumba, NULL, &rectCasilla);
                        break;
                    case 5:
                        SDL_RenderTexture(renderizador, texturaGirasol, NULL, &rectGirasol);
                        break;
                    case 6:
                        SDL_RenderTexture(renderizador, texturaLanzaguisantes, NULL, &rectPlanta);
                        break;
                    case 7:
                        SDL_RenderTexture(renderizador, texturaNuez, NULL, &rectPlanta);
                        break;
                    case 8:
                        SDL_RenderTexture(renderizador, texturaSol, NULL, &rectPlanta);
                        break;
                    case 9:
                        SDL_RenderTexture(renderizador, texturaGuisante, NULL, &rectProyectil);
                        break;
                }
            }
        }
    
        //Posicion del seleccionador
        float posicion_x = (EJE_X_JUGADOR + ((columnaJugador - 1) * ANCHO_CASILLA)) - ((ANCHO_JUGADOR - ANCHO_CASILLA) / 2.0f);
        float posicion_y = (EJE_Y_JUGADOR + ((filaJugador - 1) * ALTO_CASILLA)) - (ALTO_JUGADOR - ALTO_CASILLA);

        SDL_FRect rectJugador = { 
            posicion_x,
            posicion_y, 
            ANCHO_JUGADOR, 
            ALTO_JUGADOR,
        };

        SDL_RenderTexture(renderizador, texturaJugador, NULL, &rectJugador);
        
        SDL_RenderPresent(renderizador);
        SDL_Delay(16); // Control de fps

    }//Fin de while(jugando)

    //Liberar memoria al cerrar
    SDL_DestroyTexture(texturaJugador);
    SDL_DestroyTexture(texturaJardin);
    SDL_DestroyTexture(texturaZombie);
    SDL_DestroyTexture(texturaAbanderado);
    SDL_DestroyTexture(texturaLanzaguisantes);
    SDL_DestroyTexture(texturaGirasol);
    SDL_DestroyTexture(texturaNuez);
    SDL_DestroyTexture(texturaGuisante);
    SDL_DestroyTexture(texturaSol);
    SDL_DestroyTexture(texturaTumba);

    SDL_DestroyRenderer(renderizador);
    SDL_DestroyWindow(ventana);
    SDL_Quit();

    return 0;
}