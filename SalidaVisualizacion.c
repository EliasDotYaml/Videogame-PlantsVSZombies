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
bool inicio = true;

SDL_Window *ventana = NULL;
SDL_Renderer *renderizador = NULL;

SDL_Texture *cargarTextura(SDL_Renderer *renderizador, const char *archivo) {
    SDL_Texture *textura = IMG_LoadTexture(renderizador, archivo);
    if (textura == NULL) {
        SDL_Log("Error al cargar %s: %s", archivo, SDL_GetError());
    }
    return textura;
}

int main(int argc, char *argv[]) {

    //Inicializar SDL y crear ventana
    if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO)) {
        SDL_Log("Error al iniciar SDL: %s", SDL_GetError());
        return 1;
    }

    if (!SDL_CreateWindowAndRenderer(tituloventana, anchuravent, alturavent, 0, &ventana, &renderizador)) {
        SDL_Log("Error al iniciar ventana: %s", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    //Cargar configuracion y mapa usando las funciones existentes de la tarea1
    Configuracion config = cargar_configuracion("Logica.txt");
    Mapa mapa = {0};
    SDL_Event evento;

    //Teclado y ventana
    while (inicio) {

        while (SDL_PollEvent(&evento)) {
            if (evento.type == SDL_EVENT_QUIT) {
                inicio = false;
                jugando = false;
            }

            if (evento.type == SDL_EVENT_KEY_DOWN && !evento.key.repeat) {
                switch (evento.key.scancode) {
                    case SDL_SCANCODE_E:
                        mapa = cargar_mapa("PartidaGuardada.txt");
                        inicio = false;
                        break;

                    case SDL_SCANCODE_R:
                        mapa = cargar_mapa("Logica.txt");
                        inicio = false;
                        break;

                    case SDL_SCANCODE_ESCAPE:
                        inicio = false;
                        jugando = false;
                        break;

                    default:
                        break;
                }
            }
        }
        SDL_RenderClear(renderizador);
        SDL_Texture *texturaJardin = cargarTextura(renderizador, "Fotografias/Jardin.jpg");
        SDL_RenderTexture(renderizador, texturaJardin, NULL, NULL);

        int escalaMenu = 6;
        SDL_SetRenderDrawColor(renderizador, 255, 255, 255, 255);
        SDL_SetRenderScale(renderizador, escalaMenu, escalaMenu);
        SDL_RenderDebugText(renderizador, 20, 50, "Presione E para cargar partida");
        SDL_RenderDebugText(renderizador, 20, 90, "Presione R para empezar nueva partida");
        SDL_SetRenderScale(renderizador, 1, 1);

        SDL_RenderPresent(renderizador);
    }

    if (!jugando) {
        SDL_DestroyRenderer(renderizador);
        SDL_DestroyWindow(ventana);
        SDL_Quit();
        return 0;
    }

    // Respaldo si se presiono E pero aun no existe PartidaGuardada.txt
    if (mapa.matriz == NULL) {
        SDL_Log("No se encontro partida guardada, iniciando partida nueva...");
        mapa = cargar_mapa("Logica.txt");
    }

    //matrices de la Tarea 2 
    int **matriz_vida_juego = matriz_vida(&mapa, config);
    int **matriz_guis = matriz_guisantes(&mapa);
    int **matriz_tumba = matriz_tumbas(&mapa);
    int **matriz_sol = matriz_soles(&mapa);

    int filaJugador = 1;
    int columnaJugador = 1;

     //Matriz auxiliar para mover el fichero del jugador
    int movilidad[mapa.filas][mapa.columnas];
    // Guarda hasta qué milisegundo se ve la explosión
    Uint64 fin_explosion[mapa.filas][mapa.columnas];

    for (int i = 0; i < mapa.filas; i++) {
        for (int j = 0; j < mapa.columnas; j++) {
            movilidad[i][j] = 0;
            fin_explosion[i][j] = 0;
        }
    }

    //Cargar assets
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
    SDL_Texture *texturaLanzaguisantesDisparando = cargarTextura(renderizador, "Fotografias/LanzaguisantesDisparando.png");
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

    //Validaciones
    if (!texturaJardin || !texturaJugador) {
        SDL_DestroyRenderer(renderizador);
        SDL_DestroyWindow(ventana);
        SDL_Quit();
        return 1;
    }

    SDL_Texture *fotosIntro[3] = {
        texturaPrepararse1,
        texturaPrepararse2,
        texturaPrepararse3
    };

    // Cuadrar fotos de paquetes y etc.
    SDL_FRect rectPaq1 = { 100, 160, 250, 150 };
    SDL_FRect rectPaq2 = { 100, 320, 250, 150 };
    SDL_FRect rectPaq3 = { 100, 475, 250, 150 };
    SDL_FRect rectPaq4 = { 100, 630, 250, 150 };
    SDL_FRect rectPaq5 = { 100, 780, 250, 150 };
    SDL_FRect rectCuentaSol = { 100, 5, 250, 150 };
    SDL_FRect rectSOL = { 20, 40, 80, 80 };
    SDL_FRect rectHuecoPala = { 1500, 950, 110, 110 };
    SDL_FRect rectPala = { 1500, 950, 110, 110 };

    // Configurar soinidos
    MIX_Init();
    MIX_Mixer *mixer = MIX_CreateMixerDevice(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, NULL);
    MIX_Track *pistaMusica = MIX_CreateTrack(mixer);
    MIX_Track *pistaEfectos = MIX_CreateTrack(mixer);
    MIX_Audio *musicaFondo = MIX_LoadAudio(mixer, "Audios/MusicaDia.mp3", false);
    MIX_Audio *SonidoInicio = MIX_LoadAudio(mixer, "Audios/Listos.mp3", true);

    if (SonidoInicio != NULL) {
        MIX_SetTrackAudio(pistaEfectos, SonidoInicio);
        MIX_PlayTrack(pistaEfectos, 0);
    }

    // Variables de estado de la partida
    int turno = 1;
    const Uint64 TIEMPO_TURNO = 2000;
    const Uint64 TIEMPO_GUISANTE = 2000;
    const Uint64 TIEMPO_DISPARO = 2000;
    const Uint64 TIEMPO_SOL = 2000;

    int contadorSoles = 5000; //soles iniciales
    int plantaSeleccionada = 0; // 0 = ninguna seleccionada
    bool palaActiva = false;    // false = pala en su hueco, true = pala en mano

    bool enIntro = true;
    Uint64 inicioIntro = SDL_GetTicks();
    Uint64 ultimo_tiempo = inicioIntro;

    //Bucle principal de juego
    while (jugando) {
        Uint64 tiempo_actual = SDL_GetTicks();

        // Fase inicial de 2 segundos: Ready Set  PLANT
        if (enIntro) {
            while (SDL_PollEvent(&evento)) {
                if (evento.type == SDL_EVENT_QUIT ||
                   (evento.type == SDL_EVENT_KEY_DOWN && evento.key.scancode == SDL_SCANCODE_ESCAPE)) {
                    jugando = false;
                }
            }

            Uint64 transcurrido = tiempo_actual - inicioIntro;

            if (transcurrido < 2000) {
                //Por cada cierto tiempo cambiamos el indice por lo tanto cambiamos la imagen
                int indiceFoto = (int)(transcurrido / 600);
                if (indiceFoto > 2) indiceFoto = 2;

                SDL_RenderClear(renderizador);
                SDL_RenderTexture(renderizador, texturaJardin, NULL, NULL);

                SDL_FRect rectIntro = { 650 , 350 , 500 , 250 };
                SDL_RenderTexture(renderizador, fotosIntro[indiceFoto], NULL, &rectIntro);

                SDL_RenderPresent(renderizador);
                SDL_Delay(16);
                continue;
            } else {
                //Validaciones
                enIntro = false;
                ultimo_tiempo = tiempo_actual;

                if (musicaFondo == NULL) {
                    SDL_Log("Error cargando musica: %s", SDL_GetError());
                } else {
                    MIX_SetTrackAudio(pistaMusica, musicaFondo);
                    MIX_SetTrackGain(pistaMusica, 0.6f);

                    SDL_PropertiesID opciones = SDL_CreateProperties();
                    SDL_SetNumberProperty(opciones, MIX_PROP_PLAY_LOOPS_NUMBER, -1);
                    MIX_PlayTrack(pistaMusica, opciones);
                    SDL_DestroyProperties(opciones);
                }
            }
        }

        // Eventos de teclado y ventana
        while (SDL_PollEvent(&evento)) {
            if (evento.type == SDL_EVENT_QUIT) {
                jugando = false;
            }

            if (evento.type == SDL_EVENT_KEY_DOWN && !evento.key.repeat) {
                int siguienteFila = filaJugador;
                int siguienteColumna = columnaJugador;

                switch (evento.key.scancode) {
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
                        // Activa o guarda la pala
                        palaActiva = !palaActiva; 
                        // Deselecciona cualquier planta
                        plantaSeleccionada = 0; 
                        break;

                    case SDL_SCANCODE_G:
                        guardar_mapa("PartidaGuardada.txt", &mapa);
                        SDL_Log("La partida se ha guardado");
                        jugando = false;
                        break;

                    case SDL_SCANCODE_1:
                        // Lanzaguisantes
                        plantaSeleccionada = 6; 
                        palaActiva = false;
                        break;

                    case SDL_SCANCODE_2:
                        // Girasol
                        plantaSeleccionada = 5; 
                        palaActiva = false;
                        break;

                    case SDL_SCANCODE_3:
                        // Nuez
                        plantaSeleccionada = 7; 
                        palaActiva = false;
                        break;

                    case SDL_SCANCODE_4:
                        // Papapum
                        plantaSeleccionada = 10; 
                        palaActiva = false;
                        break;

                    case SDL_SCANCODE_5:
                        // Cometumbas
                        plantaSeleccionada = 12; 
                        palaActiva = false;
                        break;

                    case SDL_SCANCODE_SPACE:
                        if (palaActiva) {
                            int celda = mapa.matriz[filaJugador - 1][columnaJugador - 1];
                            // Si hay una planta en esa casilla, la arranca
                            if (celda == 5 || celda == 6 || celda == 7 || celda == 10 || celda == 11 || celda == 12) {
                                mapa.matriz[filaJugador - 1][columnaJugador - 1] = 0;
                                matriz_vida_juego[filaJugador - 1][columnaJugador - 1] = 0;
                                // Devuelve la pala a su hueco
                                palaActiva = false;
                            }
                        } else if (plantaSeleccionada != 0) {
                            // Llamar a la funcion para plantar
                            int plantado = colocar_planta(&mapa, matriz_vida_juego, filaJugador - 1, columnaJugador - 1, plantaSeleccionada, &contadorSoles);
                            if (plantado == 1) {
                                // Suelta la seleccion tras plantar
                                plantaSeleccionada = 0;
                            }
                        }
                        break;

                    case SDL_SCANCODE_ESCAPE:
                        jugando = false;
                        break;

                    default:
                        break;
                }

                if (siguienteFila >= 1 && siguienteFila <= mapa.filas &&
                    siguienteColumna >= 1 && siguienteColumna <= mapa.columnas &&
                    movilidad[siguienteFila - 1][siguienteColumna - 1] == 0) {
                    filaJugador = siguienteFila;
                    columnaJugador = siguienteColumna;

                    //restamos 1 porque iniciamos fila columna jugador en 1
                    if (matriz_sol[filaJugador - 1][columnaJugador - 1] > 0) {
                        contadorSoles += 25;
                        // Borra el sol del mapa
                        matriz_sol[filaJugador - 1][columnaJugador - 1] = 0;
                    }
                }
            }
        }

        // Logica por turnos
        if (tiempo_actual - ultimo_tiempo >= TIEMPO_GUISANTE) {
            // Avanza e impacta zombies
            mover_guisantes(&mapa, matriz_vida_juego, matriz_guis);
        }

        if (tiempo_actual - ultimo_tiempo >= TIEMPO_SOL) {
            generar_soles(matriz_sol, turno, &mapa, config);
        }

        if (tiempo_actual - ultimo_tiempo >= TIEMPO_DISPARO) {
            // Genera los guisantes en matriz_guis
            buscar_zombies_atacar(&mapa, turno, config, matriz_guis);
        }

        if (tiempo_actual - ultimo_tiempo >= TIEMPO_TURNO) {
            //Revisamos que casillas tenian una Papapum lista antes del turno
            bool tenia_papapum[mapa.filas][mapa.columnas];
            for (int i = 0; i < mapa.filas; i++) {
                for (int j = 0; j < mapa.columnas; j++) {
                    tenia_papapum[i][j] = (mapa.matriz[i][j] == 11);
                }
            }

            mover_zombies(&mapa, matriz_vida_juego, turno);
            spawnear_zombies(&mapa, matriz_vida_juego, config, 1);
            patatapum_comportamiento(&mapa, turno);

            //Si había una Papapum lista y desaparecio, aparece la explosion
            for (int i = 0; i < mapa.filas; i++) {
                for (int j = 0; j < mapa.columnas; j++) {
                    if (tenia_papapum[i][j] && mapa.matriz[i][j] == 0) {
                        fin_explosion[i][j] = tiempo_actual + 800;
                    }
                }
            }

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

        // Renderizado de fondo e interfaz
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
        if (!palaActiva) {
            SDL_RenderTexture(renderizador, texturaPala, NULL, &rectPala);
        }

        // Recorremos la matriz real mapa.matriz siguiendo el struct de tarea1.h
        for (int i = 0; i < mapa.filas; i++) {
            for (int j = 0; j < mapa.columnas; j++) {

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

                SDL_FRect rectCaracono = {
                    EJE_X_ZOMBIECARACONO + (j * ANCHO_CASILLA),
                    EJE_Y_ZOMBIECARACONO + (i * ALTO_CASILLA),
                    ANCHO_ZOMBIECARACONO,
                    ALTO_ZOMBIECARACONO,
                };

                SDL_FRect rectCaracubo = {
                    EJE_X_ZOMBIECARACUBO + (j * ANCHO_CASILLA),
                    EJE_Y_ZOMBIECARACUBO + (i * ALTO_CASILLA),
                    ANCHO_ZOMBIECARACUBO,
                    ALTO_ZOMBIECARACUBO,
                };

                SDL_FRect rectPapapum = {
                    EJE_X_PAPAPUM + (j * ANCHO_CASILLA),
                    EJE_Y_PAPAPUM + (i * ALTO_CASILLA),
                    ANCHO_PAPAPUM,
                    ALTO_PAPAPUM,
                };

                SDL_FRect rectPapapumPlantada = {
                    EJE_X_PAPAPUMPLANTADA + (j * ANCHO_CASILLA),
                    EJE_Y_PAPAPUMPLANTADA + (i * ALTO_CASILLA),
                    ANCHO_PAPAPUMPLANTADA,
                    ALTO_PAPAPUMPLANTADA,
                };

                SDL_FRect rectPapapumExplosion = {
                    EJE_X_PAPAPUMEXPLOSION + (j * ANCHO_CASILLA),
                    EJE_Y_PAPAPUMEXPLOSION + (i * ALTO_CASILLA),
                    ANCHO_PAPAPUMEXPLOSION,
                    ALTO_PAPAPUMEXPLOSION,
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

                if (matriz_tumba[i][j] > 0) {
                    SDL_RenderTexture(renderizador, texturaTumba, NULL, &rectCasilla);
                }

                switch (Valor_Posicion) {
                    case 1:
                        SDL_RenderTexture(renderizador, texturaZombie, NULL, &rectZombie);
                        break;
                    case 2:
                        SDL_RenderTexture(renderizador, texturaAbanderado, NULL, &rectZombieAB);
                        break;
                    case 5:
                        SDL_RenderTexture(renderizador, texturaGirasol, NULL, &rectGirasol);
                        break;
                    case 6: {
                        //Funcion para generar png de disparo si detecta zombie
                        bool zombiEnFila = false;
                        for (int col = j; col < mapa.columnas; col++) {
                            int c = mapa.matriz[i][col];
                            if (c == 1 || c == 2 || c == 13 || c == 14) {
                                zombiEnFila = true;
                                break;
                            }
                        }
                        
                        // Muestra la foto de disparo durante los últimos 350 ms antes de que salga el guisante
                        if (zombiEnFila && (tiempo_actual - ultimo_tiempo >= TIEMPO_DISPARO - 350)) {
                            SDL_RenderTexture(renderizador, texturaLanzaguisantesDisparando, NULL, &rectPlanta);
                        } else {
                            SDL_RenderTexture(renderizador, texturaLanzaguisantes, NULL, &rectPlanta);
                        }
                        break;
                    }
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

                if (tiempo_actual < fin_explosion[i][j]) {
                    SDL_RenderTexture(renderizador, texturaPapapumExplosion, NULL, &rectPapapumExplosion);
                }

                if (matriz_sol[i][j] > 0) {
                    SDL_RenderTexture(renderizador, texturaSol, NULL, &rectPlanta);
                }
                
                if (matriz_guis[i][j] > 0) {
                    // Dibuja el proyectil si hay un guisante en esa celda de matriz_guis
                    SDL_RenderTexture(renderizador, texturaGuisante, NULL, &rectProyectil);
                }
            }
        }

        // Posicion seleccionador y/o pala encima si esta activa, esto es para cuadrarlo a la hora de renderizarlo
        float posicion_x = (EJE_X_JUGADOR + ((columnaJugador - 1) * ANCHO_CASILLA)) - ((ANCHO_JUGADOR - ANCHO_CASILLA) / 2.0f);
        float posicion_y = (EJE_Y_JUGADOR + ((filaJugador - 1) * ALTO_CASILLA)) - (ALTO_JUGADOR - ALTO_CASILLA);

        SDL_FRect rectJugador = {
            posicion_x,
            posicion_y,
            ANCHO_JUGADOR,
            ALTO_JUGADOR,
        };

        SDL_RenderTexture(renderizador, texturaJugador, NULL, &rectJugador);
        if (palaActiva) {
            SDL_RenderTexture(renderizador, texturaPala, NULL, &rectJugador);
        }

        // Textos de interfaz
        char textoSoles[16];
        snprintf(textoSoles, sizeof(textoSoles), "%d", contadorSoles);
        SDL_SetRenderDrawColor(renderizador, 0, 0, 0, 255);
        int escala = 5;
        SDL_SetRenderScale(renderizador, (float)escala, (float)escala);
        SDL_RenderDebugText(renderizador, 30, 12, textoSoles);

        int escalanumeros = 2;
        SDL_SetRenderDrawColor(renderizador, 255, 255, 255, 255);
        SDL_SetRenderScale(renderizador, (float)escalanumeros, (float)escalanumeros);
        SDL_RenderDebugText(renderizador, 250, 7, "'espacio' para plantar o desplantar (1 vez) 'g' para guardar partida");
        SDL_RenderDebugText(renderizador, 10, 110, "Key_1");
        SDL_RenderDebugText(renderizador, 10, 190, "Key_2");
        SDL_RenderDebugText(renderizador, 10, 270, "Key_3");
        SDL_RenderDebugText(renderizador, 10, 350, "Key_4");
        SDL_RenderDebugText(renderizador, 10, 430, "Key_5");
        SDL_RenderDebugText(renderizador, 700, 500, "Key_Q");
         // Regresar a 1, para no agrandar las imágenes del siguiente frame
        SDL_SetRenderScale(renderizador, 1.0f, 1.0f);

        SDL_RenderPresent(renderizador);
        // 16 fps
        SDL_Delay(16);
    }

    // Liberar memoria
    SDL_DestroyTexture(texturaLanzaguisantesPaquete);
    SDL_DestroyTexture(texturaGirasolPaquete);
    SDL_DestroyTexture(texturaNuezPaquete);
    SDL_DestroyTexture(texturaPapapumPaquete);
    SDL_DestroyTexture(texturaCometumbasPaquete);
    SDL_DestroyTexture(texturaCuentaSol);
    SDL_DestroyTexture(texturaHuecoPala);
    SDL_DestroyTexture(texturaPala);
    SDL_DestroyTexture(texturaJardin);
    SDL_DestroyTexture(texturaDerrota);
    SDL_DestroyTexture(texturaOleada);
    SDL_DestroyTexture(texturaJugador);
    SDL_DestroyTexture(texturaZombie);
    SDL_DestroyTexture(texturaAbanderado);
    SDL_DestroyTexture(texturaCaracono);
    SDL_DestroyTexture(texturaCaracubeta);
    SDL_DestroyTexture(texturaLanzaguisantes);
    SDL_DestroyTexture(texturaLanzaguisantesDisparando);
    SDL_DestroyTexture(texturaGuisante);
    SDL_DestroyTexture(texturaGirasol);
    SDL_DestroyTexture(texturaSol);
    SDL_DestroyTexture(texturaNuez);
    SDL_DestroyTexture(texturaTumba);
    SDL_DestroyTexture(texturaCometumba);
    SDL_DestroyTexture(texturaPapapum);
    SDL_DestroyTexture(texturaPapapumPLantada);
    SDL_DestroyTexture(texturaPapapumExplosion);
    SDL_DestroyTexture(texturaPrepararse1);
    SDL_DestroyTexture(texturaPrepararse2);
    SDL_DestroyTexture(texturaPrepararse3);

    //Liberar audio y mixer
    MIX_DestroyTrack(pistaMusica);
    MIX_DestroyTrack(pistaEfectos);
    MIX_DestroyAudio(musicaFondo);
    MIX_DestroyAudio(SonidoInicio);
    MIX_DestroyMixer(mixer);
    MIX_Quit();

    //Liberar matrices dinamicas
    for (int i = 0; i < mapa.filas; i++) {
        if (matriz_vida_juego != NULL) free(matriz_vida_juego[i]);
        if (matriz_guis != NULL) free(matriz_guis[i]);
        if (matriz_tumba != NULL) free(matriz_tumba[i]);
        if (matriz_sol != NULL) free(matriz_sol[i]);
        if (mapa.matriz != NULL) free(mapa.matriz[i]);
    }
    free(matriz_vida_juego);
    free(matriz_guis);
    free(matriz_tumba);
    free(matriz_sol);
    free(mapa.matriz);

    SDL_DestroyRenderer(renderizador);
    SDL_DestroyWindow(ventana);
    SDL_Quit();

    return 0;
}