/**
 * @file audio.c
 * @author Vitor Viana (vitorluizcviana@gmail.com)
 * @brief Sistema de áudio do jogo - sons e músicas
 * @version 0.1
 * @date 2026-01-15
 * 
 * @copyright Copyright (c) 2026
 * 
 */

#include "includes/audio.h"
#include <stdio.h>

// Sons do jogo
static Sound somDano = {0};
static Sound somTesouro = {0};
static Sound somCura = {0};
static Sound somGameOver = {0};
static Sound somStart = {0};
static Sound somTeleporte = {0};

// Músicas do jogo
static Music musicaPrincipal = {0};
static Music musicaMenu = {0};

// Estado do sistema de áudio
static bool audioInicializado = false;

void inicializarSistemaAudio(void) {
    if (audioInicializado) return;
    
    // Inicializar sistema de áudio
    InitAudioDevice();
    
    if (!IsAudioDeviceReady()) {
        printf("ERRO: Não foi possível inicializar o sistema de áudio\n");
        return;
    }
    
    audioInicializado = true;
    printf("Sistema de áudio inicializado com sucesso\n");
}

void carregarRecursosAudio(void) {
    if (!audioInicializado || !IsAudioDeviceReady()) {
        printf("AVISO: Sistema de áudio não inicializado\n");
        return;
    }
    
    // Carregar sons do jogo
    somDano = LoadSound("recursos/sons/dano.mp3");
    somTesouro = LoadSound("recursos/sons/tesouro.mp3");
    somCura = LoadSound("recursos/sons/cura.mp3");
    somGameOver = LoadSound("recursos/sons/gameOver.mp3");
    somStart = LoadSound("recursos/sons/start.mp3");
    somTeleporte = LoadSound("recursos/sons/teleporte.mp3");
    
    // Carregar músicas do jogo
    musicaPrincipal = LoadMusicStream("recursos/sons/musicaPrincipal.mp3");
    musicaMenu = LoadMusicStream("recursos/sons/musicaMenu.mp3");
    
    // Configurar volume das músicas
    SetMusicVolume(musicaPrincipal, 0.3f); // 30% do volume
    SetMusicVolume(musicaMenu, 0.2f); // 20% do volume (mais baixo para o menu)
    
    printf("Recursos de áudio carregados com sucesso\n");
}

void liberarRecursosAudio(void) {
    if (!audioInicializado) return;
    
    // Liberar sons
    UnloadSound(somDano);
    UnloadSound(somTesouro);
    UnloadSound(somCura);
    UnloadSound(somGameOver);
    UnloadSound(somStart);
    UnloadSound(somTeleporte);
    
    // Liberar músicas
    UnloadMusicStream(musicaPrincipal);
    UnloadMusicStream(musicaMenu);
    
    // Fechar sistema de áudio
    CloseAudioDevice();
    
    // Reset das estruturas
    somDano = (Sound){0};
    somTesouro = (Sound){0};
    somCura = (Sound){0};
    somGameOver = (Sound){0};
    somStart = (Sound){0};
    somTeleporte = (Sound){0};
    musicaPrincipal = (Music){0};
    musicaMenu = (Music){0};
    
    audioInicializado = false;
    printf("Recursos de áudio liberados\n");
}

// === FUNÇÕES DE EFEITOS SONOROS ===

void tocarSomDano(void) {
    if (audioInicializado && IsAudioDeviceReady()) {
        PlaySound(somDano);
    }
}

void tocarSomTesouro(void) {
    if (audioInicializado && IsAudioDeviceReady()) {
        PlaySound(somTesouro);
    }
}

void tocarSomCura(void) {
    if (audioInicializado && IsAudioDeviceReady()) {
        PlaySound(somCura);
    }
}

void tocarSomGameOver(void) {
    if (audioInicializado && IsAudioDeviceReady()) {
        PlaySound(somGameOver);
    }
}

void tocarSomStart(void) {
    if (audioInicializado && IsAudioDeviceReady()) {
        PlaySound(somStart);
    }
}

void tocarSomTeleporte(void) {
    if (audioInicializado && IsAudioDeviceReady()) {
        PlaySound(somTeleporte);
    }
}

// === FUNÇÕES DE MÚSICA ===

void iniciarMusicaPrincipal(void) {
    if (audioInicializado && IsAudioDeviceReady()) {
        PlayMusicStream(musicaPrincipal);
    }
}

void pararMusicaPrincipal(void) {
    if (audioInicializado && IsAudioDeviceReady()) {
        StopMusicStream(musicaPrincipal);
    }
}

void iniciarMusicaMenu(void) {
    if (audioInicializado && IsAudioDeviceReady()) {
        PlayMusicStream(musicaMenu);
    }
}

void pararMusicaMenu(void) {
    if (audioInicializado && IsAudioDeviceReady()) {
        StopMusicStream(musicaMenu);
    }
}

void atualizarMusica(void) {
    if (audioInicializado && IsAudioDeviceReady()) {
        UpdateMusicStream(musicaPrincipal);
        UpdateMusicStream(musicaMenu);
    }
}

// === FUNÇÕES DE STATUS ===

bool sistemaAudioPronto(void) {
    return audioInicializado && IsAudioDeviceReady();
}