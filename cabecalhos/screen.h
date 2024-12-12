// Arquivo com a struct screen que será usasda nas telas.

// estrutura:
#ifndef SCREEN_H
#define SCREEN_H

#include <allegro5/allegro.h>

typedef struct Screen {
    void (*init)();
    void (*update)();
    void (*draw)();
    void (*destroy)();
    void (*handle_event)(ALLEGRO_EVENT *event, struct Screen **currentScreen, struct Screen *gameScreen, bool *running);
} Screen;

#endif
