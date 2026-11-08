
//Cargamos dependencias
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3/SDL_image.h>
#include <stdio.h>
#include <stdbool.h>

#define flags SDL_INIT_VIDEO;
#define tituloventana "Open Window";
#define anchuravent 1920;
#define alturavent 1080;
bool jugando = true;

SDL_Window *ventana;
SDL_Renderer *renderizador;
 
 // Comprobamos que se inicializo correctamente
int main(int argc, char *argv[]){

  int filaJugador = 1;
  int columnaJugador = 1;

  if(!SDL_Init(SDL_INIT_VIDEO)){
      SDL_Log("Error al iniciar SDL: %s", SDL_GetError());
      return 1;
    }

    if(!SDL_CreateWindowAndRenderer("Mapa de juego",1024,728, 0, &ventana, &renderizador)){
      SDL_Log("Error al iniciar SDL: %s", SDL_GetError());
      SDL_Quit();
      return 1;
    }

    // Cargamos assets

    SDL_Texture *texturaPared = cargarTextura(renderizador, "Campo.jpg");
    SDL_Texture *texturaJugador = cargarTextura(renderizador, "Peashooter.png");


    // Comprobamos que las texturas se cargaron bien
    
    if(!texturaPared || !texturaJugador){
      SDL_DestroyRenderer(renderizador);
      SDL_DestroyWindow(ventana);
      SDL_Quit();

      return 1;
    }

    // Ciclo de juego

    while(jugando){

      // Variable para gestionar eventos
      
      SDL_Event evento;

      while(SDL_PollEvent(&evento)){

	// Si el evento es de tipo cerrar la ventana terminamos el ciclo
	
	if(evento.type == SDL_EVENT_QUIT)
	  jugando = false;
	
	// Si presionamos una tecla
	
	if(evento.type == SDL_EVENT_KEY_DOWN && !evento.key.repeat){

	  int siguienteFila = filaJugador;
	  int siguienteColumna = columnaJugador;

	  //Escaneamos el codigo de la tecla y aplicamos la logica correspondiente
	  
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

	  // Comprobamos que el siguiente movimiento sea valido y actualizamos la posicion

	  if(siguienteFila >=0 && siguienteFila < FILAS && siguienteColumna >=0 && siguienteColumna < COLUMNAS && mapa[siguienteFila][siguienteColumna] == 0){

	    filaJugador = siguienteFila;
	    columnaJugador = siguienteColumna;

	  }
	}
      }

      // Asignamos color al fondo y actualizamos
      
      SDL_SetRenderDrawColor(renderizador, 255, 255, 40, 255);
      SDL_RenderClear(renderizador);

      // Recorremos la matriz dibujando con las texturas

      for(int fila = 0; fila < FILAS; fila++){
	for(int columna = 0; columna < COLUMNAS; columna++){

	  // Si el valor es 1, cargamos la textura de muro
	  
	  if(mapa[fila][columna] == 1) {

	    // Convertimos casillas a pixeles
	    
	    SDL_FRect rectanguloPared = {columna * TAM_CASILLA, fila * TAM_CASILLA, TAM_CASILLA, TAM_CASILLA};
	  

	    // Dibujamos la textura, NULL indica que usaremos la imagen completa
	    
	    SDL_RenderTexture(renderizador, texturaPared, NULL, &rectanguloPared);
	  }
	}
      }

      // Creamos el rect del jugador
      
       SDL_FRect rectangulojugador = {columnaJugador * TAM_CASILLA, filaJugador * TAM_CASILLA, TAM_CASILLA, TAM_CASILLA};

       SDL_RenderTexture(renderizador, texturaJugador, NULL, &rectangulojugador);

       SDL_RenderPresent(renderizador);
       SDL_Delay(16);
    }

    SDL_DestroyTexture(texturaJugador);
    SDL_DestroyTexture(texturaPared);
    SDL_DestroyRenderer(renderizador);
    SDL_DestroyWindow(ventana);
    SDL_Quit();

    return 0;
}