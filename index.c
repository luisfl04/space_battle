#include <stdio.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>

#include "./cabecalhos/screen.h"
#include "./cabecalhos/menu_inicial.h"
#include "./cabecalhos/game.h"

int main(int argc, char **argv){
    if(!al_init()) {
        fprintf(stderr, "Falha ao inicializar Allegro.\n");
        return -1;
    }
    al_init_font_addon();
    al_init_ttf_addon();
    al_init_primitives_addon();
    al_init_image_addon();
    al_install_audio();
    al_init_acodec_addon();
    al_install_keyboard();
    al_install_mouse();

    ALLEGRO_DISPLAY *display = al_create_display(800,600);
    if(!display) {
        fprintf(stderr, "Falha ao criar display.\n");
        return -1;
    }

    ALLEGRO_EVENT_QUEUE *event_queue = al_create_event_queue();
    ALLEGRO_TIMER *timer = al_create_timer(1.0/60.0);

    al_register_event_source(event_queue, al_get_display_event_source(display));
    al_register_event_source(event_queue, al_get_timer_event_source(timer));
    al_register_event_source(event_queue, al_get_mouse_event_source());
    al_register_event_source(event_queue, al_get_keyboard_event_source());

    bool running = true;
    bool redraw = true;

    Screen *currentScreen = &MenuScreen;
    Screen *gameScreen = &GameScreen;

    currentScreen->init();

    al_start_timer(timer);

    while(running){
        ALLEGRO_EVENT ev;
        al_wait_for_event(event_queue, &ev);

        if(ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
            running = false;
        } else if(ev.type == ALLEGRO_EVENT_TIMER) {
            currentScreen->update();
            redraw = true;
        } else {
            // Passa o evento para a tela atual
            if(currentScreen->handle_event) {
                currentScreen->handle_event(&ev, &currentScreen, gameScreen, &running);
            }
        }

        if(redraw && al_is_event_queue_empty(event_queue)) {
            redraw = false;
            currentScreen->draw();
            al_flip_display();
        }
    }

    // Sai do jogo
    currentScreen->destroy();

    al_destroy_timer(timer);
    al_destroy_event_queue(event_queue);
    al_destroy_display(display);

    return 0;
}
