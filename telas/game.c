#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_native_dialog.h>
#include <stdio.h>

// Variáveis globais
static ALLEGRO_DISPLAY *display_game = NULL;
static ALLEGRO_FONT *fonte_game = NULL;

// Função para carregar a tela do jogo
void carregarTelaJogo() {
    if (!al_init()) {
        al_show_native_message_box(NULL, "Erro", "Não foi possível inicializar o Allegro", NULL, NULL, ALLEGRO_MESSAGEBOX_ERROR);
        return;
    }

    al_init_font_addon();
    al_init_ttf_addon();
    al_install_keyboard();

    // Cria a janela do jogo
    display_game = al_create_display(1366, 768);
    if (!display_game) {
        al_show_native_message_box(NULL, "Erro", "Não foi possível criar a janela do jogo", "", NULL, ALLEGRO_MESSAGEBOX_ERROR);
        return;
    }
    al_set_window_title(display_game, "Tela do Jogo");

    // Carregando fonte
    fonte_game = al_load_ttf_font("./fonts/roboto/Roboto-Regular.ttf", 48, 0);
    if (!fonte_game) {
        al_show_native_message_box(NULL, "Erro", "Não foi possível carregar a fonte", "", NULL, ALLEGRO_MESSAGEBOX_ERROR);
        al_destroy_display(display_game);
        return;
    }

    // Configuração de eventos
    ALLEGRO_EVENT_QUEUE *fila_eventos = al_create_event_queue();
    al_install_keyboard();
    al_register_event_source(fila_eventos, al_get_display_event_source(display_game));
    al_register_event_source(fila_eventos, al_get_keyboard_event_source());

    // Renderiza a mensagem
    al_clear_to_color(al_map_rgb(0, 0, 0));
    al_draw_text(fonte_game, al_map_rgb(255, 255, 255), 1366 / 2, 768 / 2 - 50, ALLEGRO_ALIGN_CENTER, "Bem-vindo ao jogo!");
    al_draw_text(fonte_game, al_map_rgb(255, 255, 255), 1366 / 2, 768 / 2 + 50, ALLEGRO_ALIGN_CENTER, "Pressione ESC para voltar ao menu");
    al_flip_display();

    // Loop principal do jogo
    bool sair = false;
    while (!sair) {
        ALLEGRO_EVENT evento;
        al_wait_for_event(fila_eventos, &evento);

        if (evento.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
            sair = true;
        } else if (evento.type == ALLEGRO_EVENT_KEY_DOWN) {
            if (evento.keyboard.keycode == ALLEGRO_KEY_ESCAPE) {
                sair = true; // Voltar ao menu
            }
        }
    }

    // Libera recursos
    if (fonte_game) {
        al_destroy_font(fonte_game);
    }
    if (display_game) {
        al_destroy_display(display_game);
    }
    al_destroy_event_queue(fila_eventos);
}
