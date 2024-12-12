#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>
#include <allegro5/allegro_native_dialog.h>
#include "cabecalhos/menu_inicial.h"

static ALLEGRO_FONT *font = NULL;
static ALLEGRO_SAMPLE *menuMusic = NULL;
static bool musicOn = true;
static int screenWidth = 800;
static int screenHeight = 600;

// Definindo a estrutura 'RETANGULO':
typedef struct retangulo{
    float l;
    float t;
    float r;
    float b;
} RETANGULO;



// Inicialização dos botões do menu:
static RETANGULO startButton = {100, 200, 300, 250};
static RETANGULO toggleMusicButton = {100, 300, 350, 350};
static RETANGULO quitButton = {100, 400, 250, 450};

// Criação inicial das configurações do menu:
static void menu_init() {
    // Carregando fonte:
    font = al_load_ttf_font("./fonts/Roboto-Regular.ttf", 32, 0);
    if(!font) {
        al_show_native_message_box(NULL, "Erro", "Não foi possível carregar a fonte", NULL, NULL, ALLEGRO_MESSAGEBOX_ERROR);
        return;
    }

    // Carrega música
    al_reserve_samples(1);
    menuMusic = al_load_sample("./audio/menu_music.wav");
    if(!menuMusic) {
        al_show_native_message_box(NULL, "Erro", "Não foi possível carregar a música do menu", NULL, NULL, ALLEGRO_MESSAGEBOX_ERROR);
    } else {
        // Toca a música em loop
        al_play_sample(menuMusic, 1.0, 0.0,1.0, ALLEGRO_PLAYMODE_LOOP, NULL);
        musicOn = true;
    }
}

// Atualizações feitas no menu:
static void menu_update() {

}

// Carrega as ilustrações renderizadas no menu:
static void menu_draw() {

    al_clear_to_color(al_map_rgb(50,50,50));

    al_draw_text(font, al_map_rgb(255,255,255), screenWidth/2, 100, ALLEGRO_ALIGN_CENTER, "MENU / PAUSE");

    // Desenha botão "Iniciar Jogo"
    al_draw_filled_rectangle(startButton.l, startButton.t, startButton.r, startButton.b, al_map_rgb(100,100,200));
    al_draw_text(font, al_map_rgb(255,255,255), (startButton.l+startButton.r)/2, (startButton.t+startButton.b)/2 -16, ALLEGRO_ALIGN_CENTER, "Iniciar Jogo");

    // Desenha botão de toggle da música
    al_draw_filled_rectangle(toggleMusicButton.l, toggleMusicButton.t, toggleMusicButton.r, toggleMusicButton.b, al_map_rgb(100,200,100));
    const char *musicText = musicOn ? "Música: ON (clique)" : "Música: OFF (clique)";
    al_draw_text(font, al_map_rgb(255,255,255), (toggleMusicButton.l+toggleMusicButton.r)/2, (toggleMusicButton.t+toggleMusicButton.b)/2 -16, ALLEGRO_ALIGN_CENTER, musicText);

    // Desenha botão sair
    al_draw_filled_rectangle(quitButton.l, quitButton.t, quitButton.r, quitButton.b, al_map_rgb(200,100,100));
    al_draw_text(font, al_map_rgb(255,255,255), (quitButton.l+quitButton.r)/2, (quitButton.t+quitButton.b)/2 -16, ALLEGRO_ALIGN_CENTER, "Sair do Jogo");
}

// Função encarregada de 'destruir' os elementos que foram inicializados:
static void menu_destroy() {
    if (font) {
        al_destroy_font(font);
        font = NULL;
    }

    if(menuMusic) {
        al_destroy_sample(menuMusic);
        menuMusic = NULL;
    }
}

// Função auxiliar para verificar clique dentro de um retângulo
static bool is_point_in_rect(float x, float y, RETANGULO rect) {
    return (x >= rect.l && x <= rect.r && y >= rect.t && y <= rect.b);
}

static void menu_handle_event(ALLEGRO_EVENT *event, Screen **currentScreen, Screen *gameScreen, bool *running) {
    if(event->type == ALLEGRO_EVENT_MOUSE_BUTTON_UP) {
        float mx = event->mouse.x;
        float my = event->mouse.y;

        // Verifica se clicou no botão iniciar jogo
        if(is_point_in_rect(mx, my, startButton)) {
            // Troca para a tela do jogo
            (*currentScreen)->destroy();
            *currentScreen = gameScreen; 
            (*currentScreen)->init();
            return;
        }

        // Verifica se clicou no botão de toggle da música
        if(is_point_in_rect(mx, my, toggleMusicButton)) {
            if(musicOn) {
                // Pausa a música
                al_stop_samples();
                musicOn = false;
            } else {
                // Retoma a música
                al_play_sample(menuMusic, 1.0, 0.0,1.0, ALLEGRO_PLAYMODE_LOOP, NULL);
                musicOn = true;
            }
            return;
        }

        // Verifica se clicou no botão sair
        if(is_point_in_rect(mx, my, quitButton)) {
            // Sai do loop principal
            *running = false;
            return;
        }
    }
}

// Inicialização da estrutura screen que lida com as fases que a tela renderiza:
Screen MenuScreen = {
    .init = menu_init,
    .update = menu_update,
    .draw = menu_draw,
    .destroy = menu_destroy,
    .handle_event = menu_handle_event // Esta função foi adicionada à struct Screen para lidar com eventos
};
