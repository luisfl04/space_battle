#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include "game.h"

static void game_init() {
    // Inicializa recursos do jogo
}

static void game_update() {
    // Atualização da lógica do jogo
}

static void game_draw() {
    al_clear_to_color(al_map_rgb(0,0,100));
    // Desenha elementos do jogo
}

static void game_destroy() {
    // Libera recursos do jogo
}

// Caso quiséssemos voltar ao menu ao apertar ESC, por exemplo
static void game_handle_event(ALLEGRO_EVENT *event, Screen **currentScreen, Screen *gameScreen, bool *running) {
    if (event->type == ALLEGRO_EVENT_KEY_UP) {
        if (event->keyboard.keycode == ALLEGRO_KEY_ESCAPE) {
            // Volta para o menu
            (*currentScreen)->destroy();
            extern Screen MenuScreen; // Referência externa ao MenuScreen
            *currentScreen = &MenuScreen;
            (*currentScreen)->init();
        }
    }
}

Screen GameScreen = {
    .init = game_init,
    .update = game_update,
    .draw = game_draw,
    .destroy = game_destroy,
    .handle_event = game_handle_event
};
