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
bool mostrar_aviso_oleada = false;
SDL_Window *ventana = NULL;
SDL_Renderer *renderizador = NULL;

SDL_Texture *cargarTextura(SDL_Renderer *renderizador, const char *archivo) {
    SDL_Texture *textura = IMG_LoadTexture(renderizador, archivo);
    if (textura == NULL) {
        SDL_Log("Error al cargar %s: %s", archivo, SDL_GetError());
    }
    return textura;
}

// Funcion para reproducir varios audios sin que se corten o interpongan
void reproducirEfecto(MIX_Track *pistas[], int totalPistas, int *indiceActual, MIX_Audio *sonido) {
    if (sonido == NULL) {
        return;
    }
    MIX_Track *pista = pistas[*indiceActual];
    MIX_SetTrackAudio(pista, sonido);
    MIX_PlayTrack(pista, 0);
    
    *indiceActual = (*indiceActual + 1) % totalPistas;
}

int main(int argc, char *argv[]) {
    
    //esta parte se encarga de generar una "semilla" nueva cada vez que se inicie, esto para el rand que se utiliza en generar_mapa_aleatorio
    srand((unsigned int)time(NULL));
    
    // Inicializar SDL y crear ventana
    if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO)) {
        SDL_Log("Error al iniciar SDL: %s", SDL_GetError());
        return 1;
    }
    
    if (!SDL_CreateWindowAndRenderer(tituloventana, anchuravent, alturavent, 0, &ventana, &renderizador)) {
        SDL_Log("Error al iniciar ventana: %s", SDL_GetError());
        SDL_Quit();
        return 1;
    }
    
    // Cargar configuracion usando las funciones existentes de la tarea1
    Configuracion config = cargar_configuracion("Logica.txt");
    Mapa mapa = {0};
    int contadorSoles = config.soles_iniciales;
    SDL_Event evento;
    
    SDL_Texture *texturaJardin = cargarTextura(renderizador, "Fotografias/Jardin.png");
    
    // Teclado y ventana
    while (inicio) {
        
        while (SDL_PollEvent(&evento)) {
            if (evento.type == SDL_EVENT_QUIT) {
                inicio = false;
                jugando = false;
            }
            
            if (evento.type == SDL_EVENT_KEY_DOWN && !evento.key.repeat) {
                switch (evento.key.scancode) {
                    case SDL_SCANCODE_E:
                        mapa = cargar_mapa("PartidaGuardada.txt", &contadorSoles); // le agrege que cargue los soles
                        inicio = false;
                        break;
                        
                        case SDL_SCANCODE_R:
                        mapa = generar_mapa_aleatorio(5, 10);
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
            SDL_RenderTexture(renderizador, texturaJardin, NULL, NULL);
            
            // Oscurecer el fondo del juego con una capa semitransparente
            SDL_SetRenderDrawBlendMode(renderizador, SDL_BLENDMODE_BLEND); // Activa la mezcla de transparencia 
            SDL_SetRenderDrawColor(renderizador, 0, 0, 0, 150);            // Color negro con opacidad media (150 de 255)
            SDL_FRect fondoFin = { 0, 0, anchuravent, alturavent };
            SDL_RenderFillRect(renderizador, &fondoFin);                   // Dibuja el rectángulo oscuro encima del tablero
            SDL_SetRenderDrawBlendMode(renderizador, SDL_BLENDMODE_NONE);  // Desactiva la transparencia para futuras imagenes
            
            int escalaMenu = 6;
            SDL_SetRenderDrawColor(renderizador, 255, 255, 255, 255);
            SDL_SetRenderScale(renderizador, escalaMenu, escalaMenu);
            SDL_RenderDebugText(renderizador, 20, 50, "Presione E para cargar partida");
            SDL_RenderDebugText(renderizador, 20, 90, "Presione R para empezar nueva partida");
            SDL_SetRenderScale(renderizador, 1, 1);
            
            SDL_RenderPresent(renderizador);
        }
        
        if (!jugando) {
            SDL_DestroyTexture(texturaJardin);
            SDL_DestroyRenderer(renderizador);
            SDL_DestroyWindow(ventana);
            SDL_Quit();
            return 0;
        }
        
        // Respaldo si se presiono E pero aun no existe PartidaGuardada.txt
        if (mapa.matriz == NULL) {
            SDL_Log("No se encontro partida guardada, generando mapa aleatorio...");
            mapa = generar_mapa_aleatorio(5, 10);
        }
        
        // Matrices de la Tarea 2
        int **matriz_vida_juego = matriz_vida(&mapa, config);
        int **matriz_guis = matriz_guisantes(&mapa);
        int **matriz_tumba = matriz_tumbas(&mapa);
        int **matriz_sol = matriz_soles(&mapa);
        
        int filaJugador = 1;
        int columnaJugador = 1;
        
        // Matriz auxiliar para mover el fichero del jugador
        int movilidad[mapa.filas][mapa.columnas];
        
        // Guarda hasta qué milisegundo se ve la explosión, pose previa al disparo y retener el guisante para luego disparar
        Uint64 fin_explosion[mapa.filas][mapa.columnas];
        Uint64 fin_disparo[mapa.filas][mapa.columnas];
        Uint64 tiempo_salida_guisante[mapa.filas][mapa.columnas];
        int guisante_pendiente[mapa.filas][mapa.columnas];
        
        // Cuenta cuántos turnos lleva una Cometumbas comiéndose una tumba
        int turnos_cometumba[mapa.filas][mapa.columnas];
        
        // Control continuo del deslizamiento de zombies (de 0.0 a 1.35 por casilla)
        float progreso_zombie[mapa.filas][mapa.columnas];
        Uint64 duracion_deslizamiento_zombie[mapa.filas][mapa.columnas];
        int turnos_zombie_en_celda[mapa.filas][mapa.columnas];
        
        // Indica si el guisante de esa celda se está deslizando desde la anterior
        bool guisante_deslizando[mapa.filas][mapa.columnas];
        
        for (int i = 0; i < mapa.filas; i++) {
            for (int j = 0; j < mapa.columnas; j++) {
                //rellenamos matrices extras con 0
                movilidad[i][j] = 0;
                fin_explosion[i][j] = 0;
                fin_disparo[i][j] = 0;
                tiempo_salida_guisante[i][j] = 0;
                guisante_pendiente[i][j] = 0;
                turnos_cometumba[i][j] = 0;
            progreso_zombie[i][j] = 1.0f;
            duracion_deslizamiento_zombie[i][j] = 4000;
            turnos_zombie_en_celda[i][j] = 0;
            guisante_deslizando[i][j] = false;
        }
    }
    
    // Cargar assets
    SDL_Texture *texturaLanzaguisantesPaquete = cargarTextura(renderizador, "Fotografias/Lanzaguisantes_Paquete.png");
    SDL_Texture *texturaGirasolPaquete = cargarTextura(renderizador, "Fotografias/Girasol_Paquete.png");
    SDL_Texture *texturaNuezPaquete = cargarTextura(renderizador, "Fotografias/Nuez_Paquete.png");
    SDL_Texture *texturaPapapumPaquete = cargarTextura(renderizador, "Fotografias/Papapum_Paquete.png");
    SDL_Texture *texturaCometumbasPaquete = cargarTextura(renderizador, "Fotografias/Cometumbas_Paquete.png");
    SDL_Texture *texturaCuentaSol = cargarTextura(renderizador, "Fotografias/GuardaSoles.png");
    SDL_Texture *texturaHuecoPala = cargarTextura(renderizador, "Fotografias/huecoPala.png");
    SDL_Texture *texturaPala = cargarTextura(renderizador, "Fotografias/Pala.png");
    
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
    
    // Validaciones
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
    
    // Configurar sonidos
    MIX_Init();
    MIX_Mixer *mixer = MIX_CreateMixerDevice(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, NULL);
    MIX_Track *pistaMusica = MIX_CreateTrack(mixer);
    
    //para poder poner 8 sonidos de forma simultanea
    const int TOTAL_PISTAS_EFECTOS = 8;
    MIX_Track *pistasEfectos[8];
    for (int k = 0; k < TOTAL_PISTAS_EFECTOS; k++) {
        pistasEfectos[k] = MIX_CreateTrack(mixer);
    }
    int indicePista = 0;
    
    //Cargamos sonidos
    MIX_Audio *musicaFondo = MIX_LoadAudio(mixer, "Audios/MusicaDia.mp3", false);
    MIX_Audio *SonidoInicio = MIX_LoadAudio(mixer, "Audios/Listos.mp3", true);
    MIX_Audio *sonidoPlantar = MIX_LoadAudio(mixer, "Audios/Plantar.mp3", true);
    MIX_Audio *sonidoDesplantar = MIX_LoadAudio(mixer, "Audios/DesplantarPala.mp3", true);
    MIX_Audio *sonidoAgarrarPala = MIX_LoadAudio(mixer, "Audios/AgarrarPala.mp3", true);
    MIX_Audio *sonidoSeleccionar = MIX_LoadAudio(mixer, "Audios/Seleccionar.mp3", true);
    MIX_Audio *sonidoSol = MIX_LoadAudio(mixer, "Audios/RecogerSol.mp3", true);
    MIX_Audio *sonidoExplosion = MIX_LoadAudio(mixer, "Audios/PapapumExplosion.mp3", true);
    MIX_Audio *sonidoComerTumba = MIX_LoadAudio(mixer, "Audios/ComerTumba.mp3", true);
    MIX_Audio *sonidoDisparo = MIX_LoadAudio(mixer, "Audios/DispararGuisante.mp3", true);
    MIX_Audio *sonidoImpactoGuisante = MIX_LoadAudio(mixer, "Audios/ImpactoGuisante.mp3", true);
    MIX_Audio *sonidoMordisco1 = MIX_LoadAudio(mixer, "Audios/Masticar1.mp3", true);
    MIX_Audio *sonidoMordisco2 = MIX_LoadAudio(mixer, "Audios/Masticar2.mp3", true);
    MIX_Audio *sonidoDerrota = MIX_LoadAudio(mixer, "Audios/Derrota.mp3", true);
    MIX_Audio *sonidoGritoDerrota = MIX_LoadAudio(mixer, "Audios/GritoDerrota.mp3", true);
    MIX_Audio *sonidoVictoria = MIX_LoadAudio(mixer, "Audios/Victoria.mp3", true);
    MIX_Audio *sonidoOleadaAviso = MIX_LoadAudio(mixer, "Audios/OleadaAviso.mp3", true);
    MIX_Audio *sonidoSirenaOleada = MIX_LoadAudio(mixer, "Audios/SirenaOleada.mp3", true);

    if (SonidoInicio != NULL) {
        reproducirEfecto(pistasEfectos, TOTAL_PISTAS_EFECTOS, &indicePista, SonidoInicio);
    }

    // Variables de estado de la partida
    int resultadoPartida = 0;           // 0 = en curso, 1 = victoria, 2 = derrota
    EstadoRondas estado_rondas = iniciar_estado_rondas(config); // controla en que fase estamos
    int turno = 1;
    int turno_disparo = 1;
    int turnos_por_paso_zombie = 1;
    int turnoMordisco = 0;              // 0 = Mordisco1, 1 = Mordisco2
    const Uint64 TIEMPO_MORDISCO = 800; // Ms entre cada mordisco
    const Uint64 TIEMPO_TURNO = 4000;     // Más alto = zombies más lentos (4 segundos por paso)
    const Uint64 TIEMPO_GUISANTE = 350;   // Menor valor = guisantes más rápidos
    const Uint64 TIEMPO_DISPARO = 2000;   // Intervalo en ms entre disparos
    
    
    int plantaSeleccionada = 0; // 0 = ninguna seleccionada
    bool palaActiva = false;    // false = pala en su hueco, true = pala en mano
    
    bool enIntro = true;
    
    // Cantidad de ms que pasaron desde que se inicio
    Uint64 inicioIntro = SDL_GetTicks();
    Uint64 tiempo_anterior = inicioIntro;
    Uint64 ultimo_tiempo = inicioIntro;
    Uint64 ultimo_tiempo_guisante = inicioIntro;
    Uint64 ultimo_tiempo_disparo = inicioIntro;
    Uint64 ultimo_tiempo_mordisco = inicioIntro;
    Uint64 tiempo_inicio_oleada = 0; // Usa el mismo tipo de dato que 'tiempo_actual'
    
    // Bucle principal de juego
    while (jugando) {
        Uint64 tiempo_actual = SDL_GetTicks();
        Uint64 delta_tiempo = tiempo_actual - tiempo_anterior;
        tiempo_anterior = tiempo_actual;

        // Fase inicial de 2 segundos: Ready Set PLANT
        if (enIntro) {
            while (SDL_PollEvent(&evento)) {
                if (evento.type == SDL_EVENT_QUIT ||
                   (evento.type == SDL_EVENT_KEY_DOWN && evento.key.scancode == SDL_SCANCODE_ESCAPE)) {
                    jugando = false;
                }
            }

            Uint64 transcurrido = tiempo_actual - inicioIntro;
            if (transcurrido < 2000) {
                //Cambiamos las imagenes durante 2 segundos
                int indiceFoto = (int)(transcurrido / 600);
                if (indiceFoto > 2) indiceFoto = 2;

                //renderizar y cuadrar
                SDL_RenderClear(renderizador);
                SDL_RenderTexture(renderizador, texturaJardin, NULL, NULL);

                SDL_FRect rectIntro = { 650, 350, 500, 250 };
                SDL_RenderTexture(renderizador, fotosIntro[indiceFoto], NULL, &rectIntro);

                SDL_RenderPresent(renderizador);
                SDL_Delay(16);
                continue;
            } else {
                enIntro = false;
                tiempo_anterior = tiempo_actual;
                ultimo_tiempo = tiempo_actual;
                ultimo_tiempo_guisante = tiempo_actual;
                ultimo_tiempo_disparo = tiempo_actual;
                ultimo_tiempo_mordisco = tiempo_actual;

                if (musicaFondo == NULL) {
                    SDL_Log("Error cargando musica: %s", SDL_GetError());
                } else {
                    //Establecemos musica
                    MIX_SetTrackAudio(pistaMusica, musicaFondo);
                    MIX_SetTrackGain(pistaMusica, 1); //volumen

                    SDL_PropertiesID opciones = SDL_CreateProperties();
                    //-1 para bucle infinito
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
                if (evento.key.scancode == SDL_SCANCODE_ESCAPE) {
                    jugando = false;
                } else if (resultadoPartida == 0) {
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
                            palaActiva = !palaActiva;
                            plantaSeleccionada = 0;
                            if (palaActiva) {
                                reproducirEfecto(pistasEfectos, TOTAL_PISTAS_EFECTOS, &indicePista, sonidoAgarrarPala);
                            }
                            break;

                        case SDL_SCANCODE_G:
                            guardar_mapa("PartidaGuardada.txt", &mapa, contadorSoles); // le agregue que guarde los soles
                            SDL_Log("La partida se ha guardado");
                            jugando = false;
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
                            plantaSeleccionada = 10; // Papapum
                            palaActiva = false;
                            break;

                        case SDL_SCANCODE_5:
                            plantaSeleccionada = 12; // Cometumbas
                            palaActiva = false;
                            break;

                        case SDL_SCANCODE_SPACE: {
                            int f = filaJugador - 1;
                            int c = columnaJugador - 1;

                            if (palaActiva) {
                                //Validamos que haya una planta para desplantar al seleccionar pala
                                int celda = mapa.matriz[f][c];
                                if (celda == 5 || celda == 6 || celda == 7 || celda == 10 || celda == 11 || celda == 12) {
                                    mapa.matriz[f][c] = 0;
                                    matriz_vida_juego[f][c] = 0;
                                    turnos_cometumba[f][c] = 0;
                                    fin_disparo[f][c] = 0;
                                    palaActiva = false;
                                    reproducirEfecto(pistasEfectos, TOTAL_PISTAS_EFECTOS, &indicePista, sonidoDesplantar);
                                }
                                
                            // solo permite plantar si hay una planta elegida y esta en la fase de preparacion o descanso
                            } else if (plantaSeleccionada != 0 && puede_plantar(&estado_rondas) && c < mapa.columnas - 1) { 
                                if (plantaSeleccionada == 12) {
                                    if (matriz_tumba[f][c] > 0) {
                                        int plantado = colocar_planta(&mapa, matriz_vida_juego, f, c, 12, &contadorSoles);
                                        //Logica soles cometumbas
                                        if (plantado != 1 && contadorSoles >= 75) {
                                            mapa.matriz[f][c] = 12;
                                            matriz_vida_juego[f][c] = 100;
                                            contadorSoles -= 75;
                                            plantado = 1;
                                        }
                                        //plantamos cometumbas
                                        if (plantado == 1) {
                                            turnos_cometumba[f][c] = 0;
                                            plantaSeleccionada = 0;
                                            reproducirEfecto(pistasEfectos, TOTAL_PISTAS_EFECTOS, &indicePista, sonidoPlantar);
                                            reproducirEfecto(pistasEfectos, TOTAL_PISTAS_EFECTOS, &indicePista, sonidoComerTumba);
                                        }
                                    }
                                } 
                                //si no hay tumba podemos plantar
                                else if (matriz_tumba[f][c] == 0) {
                                    int plantado = colocar_planta(&mapa, matriz_vida_juego, f, c, plantaSeleccionada, &contadorSoles);
                                    if (plantado == 1) {
                                        plantaSeleccionada = 0;
                                        reproducirEfecto(pistasEfectos, TOTAL_PISTAS_EFECTOS, &indicePista, sonidoPlantar);
                                    }
                                }
                            }
                            break;
                        }

                        default:
                            break;
                    }
                    //Encerramos al jugador en las dimensiones de matriz
                    if (siguienteFila >= 1 && siguienteFila <= mapa.filas &&
                        siguienteColumna >= 1 && siguienteColumna <= mapa.columnas-1 &&
                        movilidad[siguienteFila - 1][siguienteColumna - 1] == 0) {
                        filaJugador = siguienteFila;
                        columnaJugador = siguienteColumna;

                        //validamos que se recogio un sol y desplegamos su sonido
                        if (matriz_sol[filaJugador - 1][columnaJugador - 1] > 0) {
                            contadorSoles += 25;
                            matriz_sol[filaJugador - 1][columnaJugador - 1] = 0;
                            reproducirEfecto(pistasEfectos, TOTAL_PISTAS_EFECTOS, &indicePista, sonidoSol);
                        }
                    }
                }
            }
        }

        // La logica de la partida solo avanza mientras no haya terminado (resultadoPartida == 0)
        if (resultadoPartida == 0) {

            // Suelta los guisantes retenidos justo al terminar la animación previa de disparo
            for (int i = 0; i < mapa.filas; i++) {
                for (int j = 0; j < mapa.columnas; j++) {
                    if (guisante_pendiente[i][j] > 0 && tiempo_actual >= tiempo_salida_guisante[i][j]) {
                        matriz_guis[i][j] = guisante_pendiente[i][j];
                        guisante_pendiente[i][j] = 0;
                        tiempo_salida_guisante[i][j] = 0;
                        reproducirEfecto(pistasEfectos, TOTAL_PISTAS_EFECTOS, &indicePista, sonidoDisparo);
                    }
                }
            }

            // Movimiento independiente y continuo de los guisantes
            if (tiempo_actual - ultimo_tiempo_guisante >= TIEMPO_GUISANTE) {
                int guis_antes[mapa.filas][mapa.columnas];
                for (int i = 0; i < mapa.filas; i++) {
                    for (int j = 0; j < mapa.columnas; j++) {
                        //hace aparecer al guisante
                        guis_antes[i][j] = matriz_guis[i][j];
                    }
                }

                mover_guisantes(&mapa, matriz_vida_juego, matriz_guis);

                for (int i = 0; i < mapa.filas; i++) {
                    for (int j = 0; j < mapa.columnas - 1; j++) {
                        if (guis_antes[i][j] > 0 && matriz_guis[i][j] == 0 && matriz_guis[i][j + 1] == 0) {
                            //si se cumple es porque impacto a algo
                            reproducirEfecto(pistasEfectos, TOTAL_PISTAS_EFECTOS, &indicePista, sonidoImpactoGuisante);
                        }
                    }
                }

                for (int i = 0; i < mapa.filas; i++) {
                    for (int j = 0; j < mapa.columnas; j++) {
                        //sigue deslizando
                        if (matriz_guis[i][j] > 0 && j > 0 && guis_antes[i][j - 1] > 0) {
                            guisante_deslizando[i][j] = true;
                        } else {
                            guisante_deslizando[i][j] = false;
                        }
                    }
                }

                ultimo_tiempo_guisante = tiempo_actual;
            }

            // Anticipación para imprimir png de disparo y disparo de plantas
            if (tiempo_actual - ultimo_tiempo_disparo >= TIEMPO_DISPARO) {
                int guis_antes[mapa.filas][mapa.columnas];
                for (int i = 0; i < mapa.filas; i++) {
                    for (int j = 0; j < mapa.columnas; j++) {
                        guis_antes[i][j] = matriz_guis[i][j];
                    }
                }

                buscar_zombies_atacar(&mapa, turno_disparo, config, matriz_guis);

                //Ajustamos para que no se vea mas de una vez si no esta disparando
                Uint64 anticipacion;
                if (TIEMPO_DISPARO > 400) {
                    anticipacion = 350;
                } else {
                    anticipacion = TIEMPO_DISPARO / 2;
                }

                // Retiene el guisante nuevo durante 350 ms para mostrar primero el PNG de disparo
                for (int i = 0; i < mapa.filas; i++) {
                    for (int j = 0; j < mapa.columnas; j++) {
                        if (guis_antes[i][j] == 0 && matriz_guis[i][j] > 0) {
                            guisante_pendiente[i][j] = matriz_guis[i][j];
                            matriz_guis[i][j] = 0;
                            tiempo_salida_guisante[i][j] = tiempo_actual + anticipacion;

                            //si hay lanzaguisantes
                            if (mapa.matriz[i][j] == 6) {
                                fin_disparo[i][j] = tiempo_actual + anticipacion;
                            } else if (j > 0 && mapa.matriz[i][j - 1] == 6) {
                                fin_disparo[i][j - 1] = tiempo_actual + anticipacion;
                            }
                        }
                    }
                }

                turno_disparo++;
                ultimo_tiempo_disparo = tiempo_actual;
            }

            // Logica por turnos
            if (tiempo_actual - ultimo_tiempo >= TIEMPO_TURNO) {

                int fase_anterior = estado_rondas.fase_actual;
                actualizar_estado_rondas(&estado_rondas, &mapa, config);

                    // Se activa únicamente en el turno exacto en que empieza la oleada
                    if (fase_anterior != fase_oleada && estado_rondas.fase_actual == fase_oleada) {
                        mostrar_aviso_oleada = true;
                        tiempo_inicio_oleada = tiempo_actual;

                        reproducirEfecto(pistasEfectos, TOTAL_PISTAS_EFECTOS, &indicePista, sonidoOleadaAviso);
                        reproducirEfecto(pistasEfectos, TOTAL_PISTAS_EFECTOS, &indicePista, sonidoSirenaOleada);
                    }
                
                SDL_RenderPresent(renderizador);

                // solo genera zombies si estamos en medio de una oleada y aun quedan zombies por salir


                if (estado_rondas.fase_actual == fase_oleada && estado_rondas.zombies_por_spawnear > 0) {
                    int generados = spawnear_zombies(&mapa, matriz_vida_juego, config, estado_rondas.ronda_actual);
                    estado_rondas.zombies_por_spawnear -= generados;
                }    

                bool tenia_papapum[mapa.filas][mapa.columnas];
                //copia de la matriz original vacia
                int mapa_antes[mapa.filas][mapa.columnas];

                for (int i = 0; i < mapa.filas; i++) {
                    for (int j = 0; j < mapa.columnas; j++) {
                        //devuelve false o true si hay... papapum
                        tenia_papapum[i][j] = (mapa.matriz[i][j] == 11);
                        mapa_antes[i][j] = mapa.matriz[i][j];
                    }
                }
                //llamo funciones tarea2
                generar_soles(matriz_sol, turno, &mapa, config);
                mover_zombies(&mapa, matriz_vida_juego, turno);
                patatapum_comportamiento(&mapa, turno);

                // Recorre todo el tablero celda por celda para actualizar la animación de desplazamiento
                for (int i = 0; i < mapa.filas; i++) {
                    for (int j = 0; j < mapa.columnas; j++) {
                        
                        // Obtiene el código de lo que hay AHORA en la celda (i, j) después de mover_zombies
                        int actual = mapa.matriz[i][j];
                        
                        // Comprueba si corresponde a algún tipo de zombie
                        bool esZombieActual = (actual == 1 || actual == 2 || actual == 13 || actual == 14);

                        if (esZombieActual) {
                            // Revisa qué había en esta misma celda ANTES de ejecutar mover_zombies
                            int antes = mapa_antes[i][j];
                            bool habiaZombieAqui = (antes == 1 || antes == 2 || antes == 13 || antes == 14);
                            bool veniaDeDerecha = false;

                            // Si no estamos en el borde derecho del mapa, revisa si en la celda de la derecha (j + 1) había un zombie
                            if (j + 1 < mapa.columnas) {
                                int derecha = mapa_antes[i][j + 1];
                                veniaDeDerecha = (derecha == 1 || derecha == 2 || derecha == 13 || derecha == 14);
                            }
                            // Si había un zombie aquí Y otro justo a su derecha, puede que estén detenidos comiendo una planta,
                            // o puede que toda la fila haya avanzado 1 casilla a la izquierda al mismo tiempo.
                            bool grupoDesplazado = false;
                            if (habiaZombieAqui && veniaDeDerecha) {
                                int frenteGrupo = j;

                                // Camina hacia la izquierda en el estado anterior (mapa_antes) para encontrar al primer zombie de la fila
                                while (frenteGrupo > 0) {
                                    int izqAntes = mapa_antes[i][frenteGrupo - 1];

                                    if (izqAntes == 1 || izqAntes == 2 || izqAntes == 13 || izqAntes == 14) {
                                        frenteGrupo--; // Sigue habiendo un zombie pegado adelante; seguimos buscando la punta
                                    } else {
                                        break; // Encontramos el inicio de la fila de zombies
                                    }
                                }

                                // Mira la casilla que está justo delante del líder del grupo en el mapa ACTUAL
                                if (frenteGrupo > 0) {
                                    int delanteAhora = mapa.matriz[i][frenteGrupo - 1];
                                    // Si ahora hay un zombie ahí, significa que el líder avanzó y arrastró a todo el grupo
                                    if (delanteAhora == 1 || delanteAhora == 2 || delanteAhora == 13 || delanteAhora == 14) {
                                        grupoDesplazado = true;
                                    }
                                }
                            }
            
                            // Pasa aquí si el zombie acaba de llegar a esta casilla:
                            // - si entró desde la derecha (veniaDeDerecha)
                            // - si acaba de aparecer en el borde derecho (j == mapa.columnas - 1)
                            // - si Había otro zombie aquí, pero toda la fila avanzó y este lo reemplazó (grupoDesplazado)
                            if ((!habiaZombieAqui && (veniaDeDerecha || j == mapa.columnas - 1)) || grupoDesplazado) {
                                int progreso_inicial = 0;

                                if (veniaDeDerecha) {
                                    // Hereda cuántos turnos le tomó cruzar la celda anterior para mantener una velocidad consistente
                                    if (turnos_zombie_en_celda[i][j + 1] > 0) {
                                        turnos_por_paso_zombie = turnos_zombie_en_celda[i][j + 1];
                                    }
                                    // Si la animación anterior se pasó un poco de 1 (100%), conserva ese excedente para que no dé tirones
                                    if (progreso_zombie[i][j + 1] > 1) {
                                        progreso_inicial = progreso_zombie[i][j + 1] - 1;
                                    }
                                }

                                // Calcula cuánto tiempo total debe durar el deslizamiento visual por esta nueva casilla
                                duracion_deslizamiento_zombie[i][j] = TIEMPO_TURNO * turnos_por_paso_zombie;
                                // Reinicia el progreso visual de esta celda (0 = recién entrando por la derecha, 1 = llegó al centro)
                                progreso_zombie[i][j] = progreso_inicial;
                                // Marca que el zombie lleva 1 turno en esta nueva casilla
                                turnos_zombie_en_celda[i][j] = 1;
                            } else {
                                // El zombie NO cambió de casilla en este turno (sigue caminando dentro de la misma celda o comiendo)
                                // Suma 1 al contador de turnos que lleva aquí
                                turnos_zombie_en_celda[i][j]++;
                            }
                        } else {
                            // Si en esta celda ya no hay ningún zombie, deja su progreso visual en 1 (100% completado)
                            progreso_zombie[i][j] = 1;
                        }
                    }
                }

                // Comportamiento de Cometumbas, la cual tarda 2 turnos en eliminar la tumba
                for (int i = 0; i < mapa.filas; i++) {
                    for (int j = 0; j < mapa.columnas; j++) {
                        if (mapa.matriz[i][j] == 12) {
                            if (matriz_tumba[i][j] > 0) {
                                turnos_cometumba[i][j]++;
                                if (turnos_cometumba[i][j] >= 2) {
                                    matriz_tumba[i][j] = 0;
                                    mapa.matriz[i][j] = 0;
                                    matriz_vida_juego[i][j] = 0;
                                    turnos_cometumba[i][j] = 0;
                                }
                            } else {
                                mapa.matriz[i][j] = 0;
                                matriz_vida_juego[i][j] = 0;
                                turnos_cometumba[i][j] = 0;
                            }
                        } else {
                            turnos_cometumba[i][j] = 0;
                        }
                    }
                }

                // Si había una Papapum lista y desapareció, aparece la explosión y suena
                for (int i = 0; i < mapa.filas; i++) {
                    for (int j = 0; j < mapa.columnas; j++) {
                        if (tenia_papapum[i][j] && mapa.matriz[i][j] == 0) {
                            fin_explosion[i][j] = tiempo_actual + 800;
                            reproducirEfecto(pistasEfectos, TOTAL_PISTAS_EFECTOS, &indicePista, sonidoExplosion);
                        }
                    }
                }
                //Verificacion de derrota o perdida
                int estado = verificar_victoria(&mapa);
                if (estado_rondas.fase_actual == fase_victoria) {
                    resultadoPartida = 1;
                    MIX_StopTrack(pistaMusica, 0);
                    reproducirEfecto(pistasEfectos, TOTAL_PISTAS_EFECTOS, &indicePista, sonidoVictoria);
                } else if (estado == 2) {
                    resultadoPartida = 2;
                    MIX_StopTrack(pistaMusica, 0);
                    reproducirEfecto(pistasEfectos, TOTAL_PISTAS_EFECTOS, &indicePista, sonidoDerrota);
                    reproducirEfecto(pistasEfectos, TOTAL_PISTAS_EFECTOS, &indicePista, sonidoGritoDerrota);
                }

                turno++;
                ultimo_tiempo = tiempo_actual;
            }

            // Sonido de mordisco alternado solo cuando el zombie ya se acercó a la planta
            if (tiempo_actual - ultimo_tiempo_mordisco >= TIEMPO_MORDISCO) {
                bool hayZombieComiendo = false;

                for (int i = 0; i < mapa.filas; i++) {
                    for (int j = 1; j < mapa.columnas; j++) {
                        int actual = mapa.matriz[i][j];
                        bool esZombie = (actual == 1 || actual == 2 || actual == 13 || actual == 14);

                        if (esZombie && progreso_zombie[i][j] >= 1.30f) {
                            int frente = mapa.matriz[i][j - 1];
                            bool esPlanta = (frente == 5 || frente == 6 || frente == 7 || frente == 10 || frente == 11 || frente == 12);

                            if (esPlanta) {
                                hayZombieComiendo = true;
                            }
                        }
                    }
                }
                // se alterna
                if (hayZombieComiendo) {
                    if (turnoMordisco == 0) {
                        reproducirEfecto(pistasEfectos, TOTAL_PISTAS_EFECTOS, &indicePista, sonidoMordisco1);
                        turnoMordisco = 1;
                    } else {
                        reproducirEfecto(pistasEfectos, TOTAL_PISTAS_EFECTOS, &indicePista, sonidoMordisco2);
                        turnoMordisco = 0;
                    }
                }

                ultimo_tiempo_mordisco = tiempo_actual;
            }

            // Actualizar el deslizamiento fotograma a fotograma
            for (int i = 0; i < mapa.filas; i++) {
                for (int j = 0; j < mapa.columnas; j++) {
                    // Obtiene el identificador de lo que hay en la celda actual (i, j)
                    int actual = mapa.matriz[i][j];
                    // Verifica si ese identificador corresponde a alguno de los tipos de zombie (1, 2, 13 o 14)
                    bool esZombie = (actual == 1 || actual == 2 || actual == 13 || actual == 14);

                    if (esZombie) {
                        // Límite normal de avance dentro de la celda: 1.0f significa llegar al centro de su propia casilla
                        float limite = 1.0f;

                        // Si el zombie no está en la primera columna (borde izquierdo), revisa qué tiene justo enfrente (j - 1)
                        if (j > 0) {
                            int frente = mapa.matriz[i][j - 1];
                            // Comprueba si en la casilla de la izquierda hay alguna planta (códigos 5, 6, 7, 10, 11 o 12)
                            bool hayPlantaEnfrente = (frente == 5 || frente == 6 || frente == 7 || frente == 10 || frente == 11 || frente == 12);
                            // Comprueba si en la casilla de la izquierda hay otro zombie
                            bool hayZombieEnfrente = (frente == 1 || frente == 2 || frente == 13 || frente == 14);

                            if (hayPlantaEnfrente) {
                                // Permite que el zombie sobrepase en un 35% el centro de su celda (1.35f) para acercarse a morder la planta
                                limite = 1.35f;
                            } else if (hayZombieEnfrente) {
                                // Si hay un zombie adelante, calcula un tope dejando 15% de separación para que no se monten uno encima del otro
                                float limiteDetras = progreso_zombie[i][j - 1] - 0.15f;

                                // Si ese tope es menor al centro normal de la celda (1.0f), ajusta el límite del zombie de atrás
                                if (limiteDetras < 1.0f) {
                                    if (limiteDetras < 0.5f) {
                                        // Como mínimo lo deja avanzar hasta la mitad de su celda (0.5f) para que no quede colgado atrás
                                        limite = 0.5f;
                                    } else {
                                        // Si está entre 0.5f y 1.0f, usa exactamente la distancia calculada detrás del otro zombie
                                        limite = limiteDetras;
                                    }
                                }
                            }
                        }

                        // Si el zombie aún no alcanza su límite visual y tiene una duración de animación válida:
                        if (progreso_zombie[i][j] < limite && duracion_deslizamiento_zombie[i][j] > 0) {
                            // Suma el avance proporcional al tiempo transcurrido en este fotograma (delta_tiempo)
                            progreso_zombie[i][j] += (float)delta_tiempo / (float)duracion_deslizamiento_zombie[i][j];

                            // Si en este fotograma se pasó un poco del tope permitido, lo recorta exactamente al límite
                            if (progreso_zombie[i][j] > limite) {
                                progreso_zombie[i][j] = limite;
                            }
                        }
                    }
                }
            }
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

        // Calcular progreso de interpolación de los guisantes
        float progreso_guisante = (float)(tiempo_actual - ultimo_tiempo_guisante) / (float)TIEMPO_GUISANTE;
        if (progreso_guisante > 1.0f) progreso_guisante = 1.0f;

        // Recorremos la matriz real mapa.matriz siguiendo el struct de tarea1.h
        for (int i = 0; i < mapa.filas; i++) {
            for (int j = 0; j < mapa.columnas; j++) {

                float offset_zombie_x = ANCHO_CASILLA * (1.0f - progreso_zombie[i][j]);

                float offset_guisante_x = 0.0f;
                if (guisante_deslizando[i][j]) {
                    offset_guisante_x = -ANCHO_CASILLA * (1.0f - progreso_guisante);
                }

                //Funciones constantes para determinar el tamaño y distancia de cada imagen a insertar
                SDL_FRect rectCasilla = {
                    EJE_X + (j * ANCHO_CASILLA),
                    EJE_Y + (i * ALTO_CASILLA),
                    ANCHO_CASILLA,
                    ALTO_CASILLA,
                };

                SDL_FRect rectZombie = {
                    EJE_X_ZOMBIE + (j * ANCHO_CASILLA) + offset_zombie_x,
                    EJE_Y_ZOMBIE + (i * ALTO_CASILLA),
                    ANCHO_ZOMBIE,
                    ALTO_ZOMBIE,
                };

                SDL_FRect rectCaracono = {
                    EJE_X_ZOMBIECARACONO + (j * ANCHO_CASILLA) + offset_zombie_x,
                    EJE_Y_ZOMBIECARACONO + (i * ALTO_CASILLA),
                    ANCHO_ZOMBIECARACONO,
                    ALTO_ZOMBIECARACONO,
                };

                SDL_FRect rectCaracubo = {
                    EJE_X_ZOMBIECARACUBO + (j * ANCHO_CASILLA) + offset_zombie_x,
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
                    EJE_X_ZOMBIE_ABANDERADO + (j * ANCHO_CASILLA) + offset_zombie_x,
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
                    EJE_X_PROYECTIL + (j * ANCHO_CASILLA) + offset_guisante_x,
                    EJE_Y_PROYECTIL + (i * ALTO_CASILLA),
                    ANCHO_PROYECTIL,
                    ALTO_PROYECTIL,
                };

                int Valor_Posicion = mapa.matriz[i][j];

                if (matriz_tumba[i][j] > 0) {
                    SDL_RenderTexture(renderizador, texturaTumba, NULL, &rectCasilla);
                }

                //Cargamos valores que encuentra en la matriz
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
                    case 6:
                        // Muestra la pose de disparo durante los ms previos a que salga el guisante
                        if (tiempo_actual < fin_disparo[i][j]) {
                            SDL_RenderTexture(renderizador, texturaLanzaguisantesDisparando, NULL, &rectPlanta);
                        } else {
                            SDL_RenderTexture(renderizador, texturaLanzaguisantes, NULL, &rectPlanta);
                        }
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
                //Validaciones respecto movimiento o desaparicion de algun numero (planta o sol)
                if (tiempo_actual < fin_explosion[i][j]) {
                    SDL_RenderTexture(renderizador, texturaPapapumExplosion, NULL, &rectPapapumExplosion);
                }

                if (matriz_sol[i][j] > 0) {
                    SDL_RenderTexture(renderizador, texturaSol, NULL, &rectPlanta);
                }

                if (matriz_guis[i][j] > 0) {
                    SDL_RenderTexture(renderizador, texturaGuisante, NULL, &rectProyectil);
                }
            }
        }

        // Posicion seleccionador y/o pala encima si esta activa
        float posicion_x = (EJE_X_JUGADOR + ((columnaJugador - 1) * ANCHO_CASILLA)) - ((ANCHO_JUGADOR - ANCHO_CASILLA) / 2.0f);
        float posicion_y = (EJE_Y_JUGADOR + ((filaJugador - 1) * ALTO_CASILLA)) - (ALTO_JUGADOR - ALTO_CASILLA);

        SDL_FRect rectJugador = {
            posicion_x,
            posicion_y,
            ANCHO_JUGADOR,
            ALTO_JUGADOR,
        };

        //ponemos la pala encima si fue electa
        SDL_RenderTexture(renderizador, texturaJugador, NULL, &rectJugador);
        if (palaActiva) {
            SDL_RenderTexture(renderizador, texturaPala, NULL, &rectJugador);
        }

        // Textos de interfaz
        char textoSoles[5];
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
        SDL_SetRenderScale(renderizador, 1.0f, 1.0f);

        // Dibujar cartel de oleada durante 3 segundos 
        if (mostrar_aviso_oleada) {
            if (tiempo_actual - tiempo_inicio_oleada <3000) {
                SDL_FRect rectOleada = { 300, 400, 1300, 200 };
                SDL_RenderTexture(renderizador, texturaOleada, NULL, &rectOleada);
            } else {
                mostrar_aviso_oleada = false; // Ya pasaron los 3 segundos, se deja de dibujar
            }
        }

        // Pantalla de fin de partida
        if (resultadoPartida == 1) { 
            // misma funcion utlizada para la pantalla inicial
            SDL_SetRenderDrawBlendMode(renderizador, SDL_BLENDMODE_BLEND);  
            SDL_SetRenderDrawColor(renderizador, 0, 0, 0, 150); 
            SDL_FRect fondoFin = { 0, 0, anchuravent, alturavent };
            SDL_RenderFillRect(renderizador, &fondoFin);
            SDL_SetRenderDrawBlendMode(renderizador, SDL_BLENDMODE_NONE);

            // Dibujar el texto principal "Ganaste"
            int escalaVictoria = 12;                                 
            SDL_SetRenderDrawColor(renderizador, 255, 255, 255, 255);       
            SDL_SetRenderScale(renderizador, escalaVictoria, escalaVictoria); 
            
            SDL_RenderDebugText(renderizador, 400, 200, "Ganaste"); 

            // Dibujar la instrucción para salir debajo del título
            int escalaSalir = 3;                                     
            SDL_SetRenderDrawColor(renderizador, 255, 255, 255, 255);     
            SDL_SetRenderScale(renderizador, escalaSalir, escalaSalir);    
            
            SDL_RenderDebugText(renderizador, 250 , 800 , "Presione ESC para salir");
            SDL_SetRenderScale(renderizador, 1, 1);// Restaura la escala normal (1x) 

        } else if (resultadoPartida == 2) { 
            
            // misma funcion utlizada para la pantalla inicial
            SDL_SetRenderDrawBlendMode(renderizador, SDL_BLENDMODE_BLEND);
            SDL_SetRenderDrawColor(renderizador, 0, 0, 0, 150);
            SDL_FRect fondoFin = { 0, 0, (float)anchuravent, (float)alturavent };
            SDL_RenderFillRect(renderizador, &fondoFin);
            SDL_SetRenderDrawBlendMode(renderizador, SDL_BLENDMODE_NONE);

            // Definir dimensiones y centrar la imagen de derrota
            SDL_FRect rectDerrota = { 590, 230, 750, 550 };
            SDL_RenderTexture(renderizador, texturaDerrota, NULL, &rectDerrota);

            // Dibujar la instrucción para salir justo debajo de la imagen de derrota
            int escalaSalir = 3;
            SDL_SetRenderDrawColor(renderizador, 255, 255, 255, 255);
            SDL_SetRenderScale(renderizador, escalaSalir, escalaSalir);
            
            SDL_RenderDebugText(renderizador, 250, 1000, "Presione ESC para salir");
            SDL_SetRenderScale(renderizador, 1.0f, 1.0f);   // Restaura la escala normal (1x)
        }

        // Actualiza la ventana mostrando todo lo que se dibujó en memoria durante este fotograma
        SDL_RenderPresent(renderizador);
        // Pausa la ejecución 16 milisegundos para mantener el juego cerca de 60 FPS 
        SDL_Delay(16);
    }

    // Liberar memoria de texturas
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

    // Liberar audio y mixer, todos los MIX_Audio deben destruirse antes de MIX_DestroyMixer
    MIX_DestroyTrack(pistaMusica);
    for (int k = 0; k < TOTAL_PISTAS_EFECTOS; k++) {
        MIX_DestroyTrack(pistasEfectos[k]);
    }
    MIX_DestroyAudio(musicaFondo);
    MIX_DestroyAudio(SonidoInicio);
    MIX_DestroyAudio(sonidoPlantar);
    MIX_DestroyAudio(sonidoDesplantar);
    MIX_DestroyAudio(sonidoAgarrarPala);
    MIX_DestroyAudio(sonidoSeleccionar);
    MIX_DestroyAudio(sonidoSol);
    MIX_DestroyAudio(sonidoExplosion);
    MIX_DestroyAudio(sonidoComerTumba);
    MIX_DestroyAudio(sonidoDisparo);
    MIX_DestroyAudio(sonidoImpactoGuisante);
    MIX_DestroyAudio(sonidoMordisco1);
    MIX_DestroyAudio(sonidoMordisco2);
    MIX_DestroyAudio(sonidoDerrota);
    MIX_DestroyAudio(sonidoGritoDerrota);
    MIX_DestroyAudio(sonidoVictoria);
    MIX_DestroyAudio(sonidoOleadaAviso);
    MIX_DestroyAudio(sonidoSirenaOleada);
    MIX_DestroyMixer(mixer);
    MIX_Quit();

    SDL_DestroyRenderer(renderizador);
    SDL_DestroyWindow(ventana);
    SDL_Quit();

    return 0;
}
