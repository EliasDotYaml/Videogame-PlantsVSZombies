#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3/SDL_image.h>
#include <stdio.h>
#include <stdlib.h>
#include "VisualizacionConstantes.h"
#include <stdbool.h>

bool jugando = true;

SDL_Window *ventana;
SDL_Renderer *renderizador;

SDL_Texture *cargarTextura(SDL_Renderer *renderizador, char *archivo){
	SDL_Texture *textura = IMG_LoadTexture(renderizador, archivo);

  	if(textura == NULL){
		SDL_Log("Error al cargar %s: %s", archivo, SDL_GetError());
		return textura;
	}
}


// Comprobamos que se inicializo correctamente
int main(int argc, char *argv[]){

	int filaJugador = 1;
	int columnaJugador = 1;
	int filas, columnas;

	//Abrimos el archivo que contiene a la matriz generada aleatoriamente
	FILE *ParaMatriz = fopen("DimensionesMapa.txt", "r");
	
	if (ParaMatriz == NULL) {
		perror("Error al abrir el archivo");
		return 1;
	}
    //Leer las dimensiones
    if (fscanf(ParaMatriz, "%d %d", &filas, &columnas) != 2) {
		printf("Formato de dimensiones invalido.\n");
        fclose(ParaMatriz);
        return 1;
    }
	
	int movilidad[filas][columnas];
	//Rellenamos la matriz para moverse de 0's
	for (int i = 0; i < filas; i++) {
		for (int j = 0; j < columnas; j++) {
			movilidad[i][j] = 0;
		}
	}
	
	int matriz[filas][columnas];

    //Llenar la matriz elemento x elemento
    for (int i = 0; i < filas; i++) {
        for (int j = 0; j < columnas; j++) {
            fscanf(ParaMatriz, "%d", &matriz[i][j]);
        }
    }

  	if(!SDL_Init(SDL_INIT_VIDEO)){
      SDL_Log("Error al iniciar SDL: %s", SDL_GetError());
      return 1;
    }

    if(!SDL_CreateWindowAndRenderer(tituloventana,anchuravent,alturavent, 0, &ventana, &renderizador)){
      SDL_Log("Error al iniciar SDL: %s", SDL_GetError());
      SDL_Quit();
      return 1;
    }

    // Cargamos assets
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


    // Comprobamos que las texturas se cargaron bien
    if(!texturaJardin || !texturaJugador){
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

		//Si el evento es de tipo cerrar la ventana terminamos el ciclo
		if(evento.type == SDL_EVENT_QUIT)
		jugando = false;
	
		//Si presionamos una tecla
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
	if(siguienteFila >=0 && siguienteFila < filas && siguienteColumna >=0 && siguienteColumna < columnas && movilidad[siguienteFila][siguienteColumna] == 0){

	    filaJugador = siguienteFila;
	    columnaJugador = siguienteColumna;
	  		}
		}
	}

    // Asignamos color al fondo y actualizamos
    SDL_RenderClear(renderizador);
    SDL_RenderTexture(renderizador,texturaJardin,NULL,NULL);

    // Recorremos la matriz dibujando con las texturas
    for(int i = 0; i < filas; i++){
		for(int j = 0; j < columnas; j++){
			
			//Seguir por posicion x, y, w, h
			// Convertimos casillas a pixeles
			SDL_FRect rectCasilla = {
				EJE_X + (j * ANCHO_CASILLA),
				EJE_Y + (i * ALTO_CASILLA),
				ANCHO_CASILLA,
				ALTO_CASILLA,
			};

			//Ajustamos Zombies
			SDL_FRect rectZombie = {
				EJE_X_ZOMBIE + (j * ANCHO_CASILLA), 
				EJE_Y_ZOMBIE + (i * ALTO_CASILLA), 
				ANCHO_ZOMBIE, 
				ALTO_ZOMBIE,
			};

			//Ajustamos Zombie ABANDERADO
			SDL_FRect rectZombieAB = {
				EJE_X_ZOMBIE_ABANDERADO + (j * ANCHO_CASILLA), 
				EJE_Y_ZOMBIE_ABANDERADO + (i * ALTO_CASILLA), 
				ANCHO_ZOMBIE_ABANDERADO, 
				ALTO_ZOMBIE_ABANDERADO,
			};
			
			//Ajustamos PLanta
			SDL_FRect rectPlanta = {
				EJE_X_PLANTA + (j * ANCHO_CASILLA),
				EJE_Y_PLANTA + (i * ALTO_CASILLA),
				ANCHO_PLANTA,
				ALTO_PLANTA,
			};
			
			//Ajustamos Girasol
			SDL_FRect rectGirasol = {
				EJE_X_GIRASOL + (j * ANCHO_CASILLA),
				EJE_Y_GIRASOL + (i * ALTO_CASILLA),
				ANCHO_GIRASOL,
				ALTO_GIRASOL,
			};
			
			//Ajustamos Guisantes
			SDL_FRect rectProyectil = {
				EJE_X_PROYECTIL + (j * ANCHO_CASILLA),
				EJE_Y_PROYECTIL + (i * ALTO_CASILLA),
				ANCHO_PROYECTIL,
				ALTO_PROYECTIL,
			};

			int Valor_Posicion = matriz[i][j];
			// Si el valor es 1, cargamos la textura de zombie
			switch(Valor_Posicion){
				case 1:
	    		// Dibujamos la textura, NULL indica que usaremos la imagen completa
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
	
    // Creamos el rect del jugador
    // Posición base de la casilla en la matriz
	// Calculamos primero las coordenadas del personaje
	float posicion_x = (EJE_X_JUGADOR + ((columnaJugador-1) * ANCHO_CASILLA)) - ((ANCHO_JUGADOR - ANCHO_CASILLA) / 2.0f);
	float posicion_y = (EJE_Y_JUGADOR + ((filaJugador-1) * ALTO_CASILLA)) - (ALTO_JUGADOR - ALTO_CASILLA);

	SDL_FRect rectJugador = { 
		posicion_x,
		posicion_y, 
		ANCHO_JUGADOR, 
		ALTO_JUGADOR,
	};

	SDL_RenderTexture(renderizador, texturaJugador, NULL, &rectJugador);
			SDL_RenderTexture(renderizador, texturaJugador, NULL, &rectJugador);
			SDL_RenderPresent(renderizador);
			SDL_Delay(16);
		}

    SDL_DestroyTexture(texturaJugador);
    SDL_DestroyTexture(texturaJardin);
    SDL_DestroyTexture(texturaTumba);

    SDL_DestroyRenderer(renderizador);
    SDL_DestroyWindow(ventana);
    SDL_Quit();

    return 0;
}