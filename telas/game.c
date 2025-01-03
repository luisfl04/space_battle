#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_native_dialog.h>
#include <stdio.h>

// Variáveis globais
static ALLEGRO_DISPLAY *display_game = NULL;
static ALLEGRO_BITMAP *imagem_fundo = NULL;

void carregarTelaJogo() {
    // Inicialização do Allegro
    if (!al_init()) {
        al_show_native_message_box(NULL, "Erro", "Não foi possível inicializar o Allegro", NULL, NULL, ALLEGRO_MESSAGEBOX_ERROR);
        return;
    }

    al_init_image_addon();
    al_install_keyboard();

    // Cria a janela do jogo
    display_game = al_create_display(1366, 768);
    if (!display_game) {
        al_show_native_message_box(NULL, "Erro", "Não foi possível criar a janela do jogo", "", NULL, ALLEGRO_MESSAGEBOX_ERROR);
        return;
    }
    al_set_window_title(display_game, "Tela do Jogo");

    // Carrega a imagem de fundo
    imagem_fundo = al_load_bitmap("./imagens/fundo.png");
    if (!imagem_fundo) {
        al_show_native_message_box(NULL, "Erro", "Não foi possível carregar a imagem de fundo", "", NULL, ALLEGRO_MESSAGEBOX_ERROR);
        al_destroy_display(display_game);
        return;
    }

    // Loop principal do jogo
    ALLEGRO_EVENT_QUEUE *fila_eventos = al_create_event_queue();
    al_register_event_source(fila_eventos, al_get_display_event_source(display_game));
    al_register_event_source(fila_eventos, al_get_keyboard_event_source());

    bool sair = false;
    while (!sair) {
        ALLEGRO_EVENT evento;
        al_clear_to_color(al_map_rgb(0, 0, 0)); // Fundo preto

        // Renderiza a imagem de fundo
        if (imagem_fundo) {
            al_draw_bitmap(imagem_fundo, 0, 0, 0); // Desenha a imagem na posição (0, 0)
        }

        al_flip_display();

        al_wait_for_event(fila_eventos, &evento);
        if (evento.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
            sair = true;
        } else if (evento.type == ALLEGRO_EVENT_KEY_DOWN) {
            if (evento.keyboard.keycode == ALLEGRO_KEY_ESCAPE) {
                sair = true; // Retornar ao menu
            }
        }
    }

    // Liberação de recursos
    if (imagem_fundo) {
        al_destroy_bitmap(imagem_fundo);
    }
    if (display_game) {
        al_destroy_display(display_game);
    }
    al_destroy_event_queue(fila_eventos);
}
