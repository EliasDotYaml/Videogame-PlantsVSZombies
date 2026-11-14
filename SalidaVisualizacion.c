#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3/SDL_image.h>
#include <SDL3/SDL_mixer.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
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
    Mapa mapa = cargar_mapa("Logica.txt");

    //tarea2
    int **matriz_vida_juego = matriz_vida(&mapa, config);
    int **matriz_guis = matriz_guisantes(&mapa);
    int **matriz_tumba = matriz_tumbas(&mapa);
    int **matriz_sol = matriz_soles(&mapa);
    
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
    if(!SDL_Init(SDL_INIT_VIDEO || SDL_INIT_AUDIO)){
        SDL_Log("Error al iniciar SDL: %s", SDL_GetError());
        return 1;
    }

    if(!SDL_CreateWindowAndRenderer(tituloventana, anchuravent, alturavent, 0, &ventana, &renderizador)){
        SDL_Log("Error al iniciar ventana: %s", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    //Cargamos assets
    SDL_Texture *texturaLanzaguisantesPaquete = cargarTextura(renderizador, "Fotografias/Lanzaguisantes_Paquete.jpg");
    SDL_Texture *texturaGirasolPaquete = cargarTextura(renderizador, "Fotografias/Girasol_Paquete.jpg");
    SDL_Texture *texturaNuezPaquete = cargarTextura(renderizador, "Fotografias/Nuez_Paquete.jpg");
    SDL_Texture *texturaPapapumPaquete = cargarTextura(renderizador, "Fotografias/Papapum_Paquete.jpg");
    SDL_Texture *texturaCometumbasPaquete = cargarTextura(renderizador, "Fotografias/Cometumbas_Paquete.png");
    SDL_Texture *texturaCuentaSol = cargarTextura(renderizador, "Fotografias/GuardaSoles.png");
    SDL_Texture *texturaHuecoPala = cargarTextura(renderizador, "Fotografias/huecoPala.png");
    SDL_Texture *texturaPala = cargarTextura(renderizador, "Fotografias/Pala.png");
    
    SDL_Texture *texturaJardin = cargarTextura(renderizador, "Fotografias/Jardin.jpg");
    SDL_Texture *texturaDerrota = cargarTextura(renderizador, "Fotografias/ComieronTUCerebro.png");
    SDL_Texture *texturaOleada = cargarTextura(renderizador, "Fotografias/Oleada_Viene.png");
    SDL_Texture *texturaJugador = cargarTextura(renderizador, "Fotografias/Seleccionador.png");
    SDL_Texture *texturaZombie = cargarTextura(renderizador, "Fotografias/Zombie.png");
    SDL_Texture *texturaAbanderado = cargarTextura(renderizador, "Fotografias/FlagZ.png");
    SDL_Texture *texturaCaracono = cargarTextura(renderizador, "Fotografias/Conehead.png");
    SDL_Texture *texturaCaracubeta = cargarTextura(renderizador, "Fotografias/Buckethead.png");
    SDL_Texture *texturaLanzaguisantes = cargarTextura(renderizador, "Fotografias/Lanzaguisantes.png");
    SDL_Texture *texturaGuisante = cargarTextura(renderizador, "Fotografias/Guisante.png");
    SDL_Texture *texturaGirasol = cargarTextura(renderizador, "Fotografias/Girasol.png");
    SDL_Texture *texturaSol = cargarTextura(renderizador, "Fotografias/Sol.png");
    SDL_Texture *texturaNuez = cargarTextura(renderizador, "Fotografias/Nuez.png");
    SDL_Texture *texturaTumba = cargarTextura(renderizador, "Fotografias/Tumba.png");
    SDL_Texture *texturaCometumba = cargarTextura(renderizador, "Fotografias/ComeTumbas.png");
    SDL_Texture *texturaPapapum = cargarTextura(renderizador, "Fotografias/Papapum.png");
    SDL_Texture *texturaPapapumPLantada = cargarTextura(renderizador, "Fotografias/PapapumPlantada.png");
    SDL_Texture *texturaPapapumExplosion = cargarTextura(renderizador, "Fotografias/PapapumExplosion.png");
    SDL_Texture *texturaPrepararse1 = cargarTextura(renderizador, "Fotografias/Ready.png");
    SDL_Texture *texturaPrepararse2 = cargarTextura(renderizador, "Fotografias/Set.png");
    SDL_Texture *texturaPrepararse3 = cargarTextura(renderizador, "Fotografias/PLANT.png");

    if(!texturaJardin || !texturaJugador){
        SDL_DestroyRenderer(renderizador);
        SDL_DestroyWindow(ventana);
        SDL_Quit();
        return 1;
    }

    // Paquetes de plantas
    SDL_FRect rectPaq1 = { 100.0f, 160.0f, 250.0f, 150.0f };
    SDL_FRect rectPaq2 = { 100.0f, 320.0f, 250.0f, 150.0f };
    SDL_FRect rectPaq3 = { 100.0f, 475.0f, 250.0f, 150.0f };
    SDL_FRect rectPaq4 = { 100.0f, 630.0f, 250.0f, 150.0f };
    SDL_FRect rectPaq5 = { 100.0f, 780.0f, 250.0f, 150.0f };
    SDL_FRect rectCuentaSol = { 100.0f, 5.0f, 250.0f, 150.0f };
    SDL_FRect rectSOL = { 20.0f, 40.0f, 80.0f, 80.0f };
    SDL_FRect rectHuecoPala = { 1500.0f, 950.0f, 110.0f, 110.0f };
    SDL_FRect rectPala = { 1500.0f, 950.0f, 110.0f, 110.0f };
    
    SDL_Event evento;
    //tarea2, se asignan cambios en la matriz cada ciertos ticks
    int turno = 1;
    Uint64 ultimo_tiempo = SDL_GetTicks(); 
    const Uint64 TIEMPO_TURNO = 2000;
    const Uint64 TIEMPO_GUISANTE = 2000; 
    const Uint64 TIEMPO_DISPARO = 2000;  
    const Uint64 TIEMPO_SOL = 2000; 

    int contadorSoles = 50; //soles iniciales
    int plantaSeleccionada = 0; // 0 = ninguna seleccionada
    bool palaActiva = false;    // false = pala en su hueco, true = pala en mano

    MIX_Init();
    MIX_Mixer *mixer = MIX_CreateMixerDevice(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, NULL);
    MIX_Track *pistaMusica = MIX_CreateTrack(mixer);
    MIX_Audio *musicaFondo = MIX_LoadAudio(mixer, "Audios/MusicaDia.mp3", false);
    MIX_Audio *SonidoInicio = MIX_LoadAudio(mixer, "Audios/Listos.mp3", false);

    if (musicaFondo == NULL) {
        SDL_Log("Error cargando musica: %s", SDL_GetError());
    } else {
        MIX_SetTrackAudio(pistaMusica, musicaFondo);
        MIX_SetTrackGain(pistaMusica, 0.6f); // Volumen opcional (de 0.0f a 1.0f)

        SDL_PropertiesID opciones = SDL_CreateProperties();
        SDL_SetNumberProperty(opciones, MIX_PROP_PLAY_LOOPS_NUMBER, -1); // -1 = bucle infinito
        MIX_PlayTrack(pistaMusica, opciones);
        SDL_DestroyProperties(opciones);
    }

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

                    case SDL_SCANCODE_Q:
                        palaActiva = !palaActiva; // Activa o guarda la pala
                        plantaSeleccionada = 0;   // Deselecciona cualquier planta
                        break;

                    case SDL_SCANCODE_1:
                        plantaSeleccionada = 6; // Lanzaguisantes
                        palaActiva = false;
                        break;
                         
                    case SDL_SCANCODE_2:
                        plantaSeleccionada = 5; // Girasol
                        palaActiva = false;
                        break;
                         
                    case SDL_SCANCODE_3:
                        plantaSeleccionada = 7; // Nuez
                        palaActiva = false;
                        break;
                         
                    case SDL_SCANCODE_4:
                        plantaSeleccionada = 10; // Papapum (cambia a 10 si tu funcion usa 10)
                        palaActiva = false;
                        break;
                         
                    case SDL_SCANCODE_5:
                        plantaSeleccionada = 12; // Cometumbas
                        palaActiva = false;
                        break;

                    case SDL_SCANCODE_SPACE:
                        if (palaActiva) {
                            int celda = mapa.matriz[filaJugador - 1][columnaJugador - 1];
                            // Si hay una planta en esa casilla, la arranca
                            if (celda == 5 || celda == 6 || celda == 7 || celda == 10 || celda == 11 || celda == 12) {
                                mapa.matriz[filaJugador - 1][columnaJugador - 1] = 0;
                                matriz_vida_juego[filaJugador - 1][columnaJugador - 1] = 0;
                                palaActiva = false; // Devuelve la pala a su hueco
                            }
                        } 
                        else if (plantaSeleccionada != 0) {
                            // Llamada exacta a la funcion de tu captura
                            int plantado = colocar_planta(&mapa, matriz_vida_juego, filaJugador - 1, columnaJugador - 1, plantaSeleccionada, &contadorSoles);
                            if (plantado == 1) {
                                plantaSeleccionada = 0; // Suelta la carta tras plantar
                            }
                        }
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
                    //restamos 1 porque iniciamos fila columna jugador en 1
                    if(matriz_sol[filaJugador - 1][columnaJugador - 1] > 0){
                        contadorSoles += 25;
                        matriz_sol[filaJugador - 1][columnaJugador - 1] = 0; // Borra el sol del mapa
                    }
                }
            }
        }

        //Logica de turno, se ejecuta cada 1 segundo, tarea 2
        Uint64 tiempo_actual = SDL_GetTicks();
        if (tiempo_actual - ultimo_tiempo >= TIEMPO_GUISANTE) {
            mover_guisantes(&mapa, matriz_vida_juego, matriz_guis);    // Los avanza e impacta zombies
        }

        if (tiempo_actual - ultimo_tiempo >= TIEMPO_SOL) {
            generar_soles (matriz_sol, turno, &mapa,config); 
        }

        if (tiempo_actual - ultimo_tiempo >= TIEMPO_DISPARO) {
            buscar_zombies_atacar(&mapa, turno, config, matriz_guis); // Genera los guisantes en matriz_guis
        }
        
        if (tiempo_actual - ultimo_tiempo >= TIEMPO_TURNO) {
            mover_zombies(&mapa, matriz_vida_juego, turno);
            spawnear_zombies(&mapa, matriz_vida_juego, config,1);
            patatapum_comportamiento(&mapa,turno);

            int estado = verificar_victoria(&mapa);
            if (estado == 1) {
                SDL_Log("¡VICTORIA!");
                jugando = false;
            } else if (estado == 2) {
                SDL_Log("¡DERROTA!");
                jugando = false;
            }

            turno++;
            ultimo_tiempo = tiempo_actual;
        }

        SDL_RenderClear(renderizador);
        SDL_RenderTexture(renderizador, texturaJardin, NULL, NULL);
        SDL_RenderTexture(renderizador, texturaLanzaguisantesPaquete, NULL, &rectPaq1);
        SDL_RenderTexture(renderizador, texturaGirasolPaquete, NULL, &rectPaq2);
        SDL_RenderTexture(renderizador, texturaNuezPaquete, NULL, &rectPaq3);
        SDL_RenderTexture(renderizador, texturaPapapumPaquete, NULL, &rectPaq4);
        SDL_RenderTexture(renderizador, texturaCometumbasPaquete, NULL, &rectPaq5);
        SDL_RenderTexture(renderizador, texturaCuentaSol, NULL, &rectCuentaSol);
        SDL_RenderTexture(renderizador, texturaSol, NULL, &rectSOL);
        SDL_RenderTexture(renderizador, texturaHuecoPala, NULL, &rectHuecoPala);
        SDL_RenderTexture(renderizador, texturaPala, NULL, &rectPala);
        
        SDL_RenderTexture(renderizador, texturaHuecoPala, NULL, &rectHuecoPala);
        if (!palaActiva) {
            SDL_RenderTexture(renderizador, texturaPala, NULL, &rectPala);
        }
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

                SDL_FRect rectCaracono= {
                    EJE_X_ZOMBIECARACONO + (j * ANCHO_CASILLA), 
                    EJE_Y_ZOMBIECARACONO + (i * ALTO_CASILLA), 
                    ANCHO_ZOMBIECARACONO, 
                    ALTO_ZOMBIECARACONO,
                };
                
                SDL_FRect rectCaracubo= {
                    EJE_X_ZOMBIECARACUBO + (j * ANCHO_CASILLA), 
                    EJE_Y_ZOMBIECARACUBO + (i * ALTO_CASILLA), 
                    ANCHO_ZOMBIECARACUBO, 
                    ALTO_ZOMBIECARACUBO,
                };
                
                SDL_FRect rectPapapum= {
                    EJE_X_PAPAPUM + (j * ANCHO_CASILLA), 
                    EJE_Y_PAPAPUM + (i * ALTO_CASILLA), 
                    ANCHO_PAPAPUM, 
                    ALTO_PAPAPUM,
                };
                
                SDL_FRect rectPapapumPlantada= {
                    EJE_X_PAPAPUMPLANTADA + (j * ANCHO_CASILLA), 
                    EJE_Y_PAPAPUMPLANTADA + (i * ALTO_CASILLA), 
                    ANCHO_PAPAPUMPLANTADA, 
                    ALTO_PAPAPUMPLANTADA,
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
                
                //colocar_planta(&mapa, matriz_vida_juego,i,j,como saber cual planta es,puntero contadorSoles);

                if (matriz_tumba[i][j] > 0) {
                    SDL_RenderTexture(renderizador, texturaTumba, NULL, &rectCasilla);
                }
                switch(Valor_Posicion){
                    case 1:
                        SDL_RenderTexture(renderizador, texturaZombie, NULL, &rectZombie);
                        break;
                    case 2:
                        SDL_RenderTexture(renderizador, texturaAbanderado, NULL, &rectZombieAB);
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
                    case 10:
                        SDL_RenderTexture(renderizador, texturaPapapumPLantada, NULL, &rectPapapumPlantada);
                        break;
                    case 11:
                        SDL_RenderTexture(renderizador, texturaPapapum, NULL, &rectPapapum);
                        break;
                    case 12:
                        SDL_RenderTexture(renderizador, texturaCometumba, NULL, &rectCasilla);
                        break;
                    case 13:
                        SDL_RenderTexture(renderizador, texturaCaracono, NULL, &rectCaracono);
                        break;
                    case 14:
                        SDL_RenderTexture(renderizador, texturaCaracubeta, NULL, &rectCaracubo);
                        break;
                }
                // Dibuja el proyectil si hay un guisante en esa celda de matriz_guis
                if (matriz_sol[i][j] > 0) {
                    SDL_RenderTexture(renderizador, texturaSol, NULL, &rectPlanta);
                }
                if (matriz_guis[i][j] > 0) {
                    SDL_RenderTexture(renderizador, texturaGuisante, NULL, &rectProyectil);
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
        
        char textoSoles[5];
        snprintf(textoSoles, sizeof(textoSoles), "%d", contadorSoles);

        // Color del texto (R, G, B, Alpha) -> Negro
        SDL_SetRenderDrawColor(renderizador, 0, 0, 0, 255);
            
        // Agrandamos el texto para que sea legible
        int escala = 5;
        SDL_SetRenderScale(renderizador, escala, escala);
        // Dividir por 'escala' permite poner las coordenadas reales en píxeles (ej: x=100, y=90, arriba de tus cartas)
        SDL_RenderDebugText(renderizador, 30, 12, textoSoles);

        int escalanumeros = 2;
        SDL_SetRenderDrawColor(renderizador, 255, 255, 255, 255);
        SDL_SetRenderScale(renderizador, escalanumeros, escalanumeros);
        SDL_RenderDebugText(renderizador, 400, 7, "'espacio' para plantar o usar pala");
        SDL_RenderDebugText(renderizador, 10, 110, "Key_1");
        SDL_RenderDebugText(renderizador, 10, 190, "Key_2");
        SDL_RenderDebugText(renderizador, 10, 270, "Key_3");
        SDL_RenderDebugText(renderizador, 10, 350, "Key_4");
        SDL_RenderDebugText(renderizador, 10, 430, "Key_5");
        SDL_RenderDebugText(renderizador, 700, 500, "Key_Q");
        // IMPORTANTE: regresar la escala a 1.0f para no agrandar las imágenes del siguiente frame
        SDL_SetRenderScale(renderizador, 1, 1);

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