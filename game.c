#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <stdio.h>

int main() {
    // Inicialização do Allegro
    if (!al_init()) {
        printf("Falha ao inicializar Allegro\n");
        return -1;
    }

    // Inicializa o módulo de teclado e de primitivas gráficas
    al_install_keyboard();
    al_init_primitives_addon();

    // Configurações da janela
    const int LARGURA = 800;
    const int ALTURA = 600;
    ALLEGRO_DISPLAY *janela = al_create_display(LARGURA, ALTURA);
    if (!janela) {
        printf("Falha ao criar a janela\n");
        return -1;
    }

    // Configurações iniciais do retângulo
    float x = LARGURA / 2;
    float y = ALTURA / 2;
    float velocidade = 4.0;

    // Evento de temporizador (60 FPS)
    ALLEGRO_TIMER *temporizador = al_create_timer(1.0 / 60);
    ALLEGRO_EVENT_QUEUE *fila_eventos = al_create_event_queue();
    al_register_event_source(fila_eventos, al_get_display_event_source(janela));
    al_register_event_source(fila_eventos, al_get_timer_event_source(temporizador));
    al_register_event_source(fila_eventos, al_get_keyboard_event_source());

    // Inicia o temporizador
    al_start_timer(temporizador);

    int sair = 0;
    int teclas[4] = { 0, 0, 0, 0 }; // Cima, baixo, esquerda, direita

    // Loop principal do jogo
    while (!sair) {
        ALLEGRO_EVENT evento;
        al_wait_for_event(fila_eventos, &evento);

        // Manipulação de eventos
        if (evento.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
            sair = 1;
        }
        else if (evento.type == ALLEGRO_EVENT_TIMER) {
            // Atualiza posição do retângulo conforme teclas pressionadas
            if (teclas[0] && y >= velocidade) y -= velocidade;     // Cima
            if (teclas[1] && y <= ALTURA - 20 - velocidade) y += velocidade; // Baixo
            if (teclas[2] && x >= velocidade) x -= velocidade;     // Esquerda
            if (teclas[3] && x <= LARGURA - 20 - velocidade) x += velocidade; // Direita

            // Limpa e desenha o retângulo na tela
            al_clear_to_color(al_map_rgb(0, 0, 0));
            al_draw_filled_rectangle(x, y, x + 20, y + 20, al_map_rgb(255, 0, 0));
            al_flip_display();
        }
        else if (evento.type == ALLEGRO_EVENT_KEY_DOWN) {
            // Detecta teclas pressionadas
            switch (evento.keyboard.keycode) {
                case ALLEGRO_KEY_UP:
                    teclas[0] = 1;
                    break;
                case ALLEGRO_KEY_DOWN:
                    teclas[1] = 1;
                    break;
                case ALLEGRO_KEY_LEFT:
                    teclas[2] = 1;
                    break;
                case ALLEGRO_KEY_RIGHT:
                    teclas[3] = 1;
                    break;
            }
        }
        else if (evento.type == ALLEGRO_EVENT_KEY_UP) {
            // Detecta teclas soltas
            switch (evento.keyboard.keycode) {
                case ALLEGRO_KEY_UP:
                    teclas[0] = 0;
                    break;
                case ALLEGRO_KEY_DOWN:
                    teclas[1] = 0;
                    break;
                case ALLEGRO_KEY_LEFT:
                    teclas[2] = 0;
                    break;
                case ALLEGRO_KEY_RIGHT:
                    teclas[3] = 0;
                    break;
                case ALLEGRO_KEY_ESCAPE:
                    sair = 1;
                    break;
            }
        }
    }

    // Finaliza os componentes
    al_destroy_timer(temporizador);
    al_destroy_display(janela);
    al_destroy_event_queue(fila_eventos);

    return 0;
}
