#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3/SDL_image.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define flags SDL_INIT_VIDEO

#define tituloventana "Plantas vs Zombies INFERNAL edition"
#define anchuravent 1920
#define alturavent 1080

#define EJE_X 395.0f
#define EJE_Y 160.0f

#define ANCHO_CASILLA 148.0f
#define ALTO_CASILLA  160.0f

#define EJE_X_JUGADOR 390.0f
#define EJE_Y_JUGADOR 155.0f

#define ANCHO_JUGADOR 153.0f
#define ALTO_JUGADOR 165.0f

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
	if(siguienteFila >=0 && siguienteFila < filas && siguienteColumna >=0 && siguienteColumna < columnas && matriz[siguienteFila][siguienteColumna] == 0){

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
			
			// Convertimos casillas a pixeles
			SDL_FRect rectCasilla = {
				EJE_X + (j * ANCHO_CASILLA),
				EJE_Y + (i * ALTO_CASILLA),
				ANCHO_CASILLA,
				ALTO_CASILLA
			};

			// Si el valor es 4, cargamos la textura de tumba
			if(matriz[i][j] == 4) {
				
	    		// Dibujamos la textura, NULL indica que usaremos la imagen completa
	    		SDL_RenderTexture(renderizador, texturaTumba, NULL, &rectCasilla);
	  			}
			}
    	}
	
    	// Creamos el rect del jugador
    	// Posición base de la casilla en la matriz
	float celda_x = EJE_X_JUGADOR + (columnaJugador * ANCHO_CASILLA);
	float celda_y = EJE_Y_JUGADOR + (filaJugador * ALTO_CASILLA);

	SDL_FRect rectJugador = {
		.x = celda_x - ((ANCHO_JUGADOR - ANCHO_CASILLA) / 2.0f), // Centrado horizontal
		.y = celda_y - (ALTO_JUGADOR - ALTO_CASILLA),            // Ajuste vertical a la base
		.w = ANCHO_JUGADOR,
		.h = ALTO_JUGADOR
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