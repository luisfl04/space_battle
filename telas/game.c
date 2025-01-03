#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_native_dialog.h>
#include <allegro5/allegro_primitives.h>
#include <stdio.h>
#include <math.h>


// Variáveis globais
static ALLEGRO_DISPLAY *display_game = NULL;
static ALLEGRO_BITMAP *imagem_fundo = NULL;
static ALLEGRO_BITMAP *imagem_nave = NULL;
static ALLEGRO_BITMAP *nave_redimensionada = NULL;

// Dimensões da tela
static const int LARGURA_TELA = 1366;
static const int ALTURA_TELA = 768;

// Velocidade da nave e da rotação
static const float VELOCIDADE_NAVE = 5.0;
static const float VELOCIDADE_ROTACAO = 0.05; // Em radianos

// Estrutura para um tiro
typedef struct {
    float x, y;
    float dx, dy; // Direção do movimento
    bool ativo;
} Tiro;

// Array de tiros
#define MAX_TIROS 10
static Tiro tiros[MAX_TIROS];

// Função para disparar um tiro
void disparar_tiro(float x, float y, float angulo) {
    for (int i = 0; i < MAX_TIROS; i++) {
        if (!tiros[i].ativo) {
            tiros[i].x = x;
            tiros[i].y = y;
            tiros[i].dx = cos(angulo) * 10; // Velocidade do tiro na direção X
            tiros[i].dy = sin(angulo) * 10; // Velocidade do tiro na direção Y
            tiros[i].ativo = true;
            break;
        }
    }
}

// Função principal da tela de jogo
void carregarTelaJogo() {
    // Inicialização do Allegro
    if (!al_init()) {
        al_show_native_message_box(NULL, "Erro", "Não foi possível inicializar o Allegro", NULL, NULL, ALLEGRO_MESSAGEBOX_ERROR);
        return;
    }

    al_init_image_addon();
    al_install_keyboard();
    al_init_primitives_addon();

    // Criação da janela do jogo
    display_game = al_create_display(LARGURA_TELA, ALTURA_TELA);
    if (!display_game) {
        al_show_native_message_box(NULL, "Erro", "Não foi possível criar a janela do jogo", "", NULL, ALLEGRO_MESSAGEBOX_ERROR);
        return;
    }
    al_set_window_title(display_game, "Space Battle - Jogo");

    // Carrega a imagem de fundo
    imagem_fundo = al_load_bitmap("./imagens/fundo.png");
    if (!imagem_fundo) {
        al_show_native_message_box(NULL, "Erro", "Não foi possível carregar a imagem de fundo", "", NULL, ALLEGRO_MESSAGEBOX_ERROR);
        al_destroy_display(display_game);
        return;
    }

    // Carrega a imagem da nave
    imagem_nave = al_load_bitmap("./imagens/game/nave_game.png");
    if (!imagem_nave) {
        al_show_native_message_box(NULL, "Erro", "Não foi possível carregar a imagem da nave", "", NULL, ALLEGRO_MESSAGEBOX_ERROR);
        al_destroy_display(display_game);
        al_destroy_bitmap(imagem_fundo);
        return;
    }

    // Redimensiona a nave para um tamanho proporcional
    float largura_nave = LARGURA_TELA * 0.1; // 10% da largura da tela
    float proporcao_nave = largura_nave / al_get_bitmap_width(imagem_nave); // Calcula proporção
    float altura_nave = al_get_bitmap_height(imagem_nave) * proporcao_nave;

    nave_redimensionada = al_create_bitmap(largura_nave, altura_nave);
    ALLEGRO_BITMAP *anterior = al_get_target_bitmap();
    al_set_target_bitmap(nave_redimensionada);
    al_draw_scaled_bitmap(imagem_nave, 0, 0, al_get_bitmap_width(imagem_nave), al_get_bitmap_height(imagem_nave),
                          0, 0, largura_nave, altura_nave, 0);
    al_set_target_bitmap(anterior);

    // Coordenadas iniciais e ângulo da nave
    float posicao_x_nave = LARGURA_TELA / 2;
    float posicao_y_nave = ALTURA_TELA / 2;
    float angulo_nave = 0.0;

    // Configuração do sistema de eventos
    ALLEGRO_EVENT_QUEUE *fila_eventos = al_create_event_queue();
    al_register_event_source(fila_eventos, al_get_display_event_source(display_game));
    al_register_event_source(fila_eventos, al_get_keyboard_event_source());

    // Inicializa tiros
    for (int i = 0; i < MAX_TIROS; i++) {
        tiros[i].ativo = false;
    }

    bool sair = false;
    bool tecla_cima = false, tecla_baixo = false, tecla_esquerda = false, tecla_direita = false;

    // Modificar o controle das teclas no loop de eventos
    while (!sair) {
        ALLEGRO_EVENT evento;
        while (al_get_next_event(fila_eventos, &evento)) {
            if (evento.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
                sair = true;
            } else if (evento.type == ALLEGRO_EVENT_KEY_DOWN) {
                if (evento.keyboard.keycode == ALLEGRO_KEY_ESCAPE) {
                    sair = true; // Retornar ao menu
                } else if (evento.keyboard.keycode == ALLEGRO_KEY_UP) {
                    tecla_cima = true; // Setas para cima para mover para frente
                } else if (evento.keyboard.keycode == ALLEGRO_KEY_DOWN) {
                    tecla_baixo = true; // Setas para baixo para mover para trás
                } else if (evento.keyboard.keycode == ALLEGRO_KEY_LEFT) {
                    tecla_esquerda = true; // Setas para esquerda para rotacionar para a esquerda
                } else if (evento.keyboard.keycode == ALLEGRO_KEY_RIGHT) {
                    tecla_direita = true; // Setas para direita para rotacionar para a direita
                } else if (evento.keyboard.keycode == ALLEGRO_KEY_SPACE) {
                    disparar_tiro(posicao_x_nave, posicao_y_nave, angulo_nave); // Disparo da nave
                }
            } else if (evento.type == ALLEGRO_EVENT_KEY_UP) {
                if (evento.keyboard.keycode == ALLEGRO_KEY_UP) {
                    tecla_cima = false;
                } else if (evento.keyboard.keycode == ALLEGRO_KEY_DOWN) {
                    tecla_baixo = false;
                } else if (evento.keyboard.keycode == ALLEGRO_KEY_LEFT) {
                    tecla_esquerda = false;
                } else if (evento.keyboard.keycode == ALLEGRO_KEY_RIGHT) {
                    tecla_direita = false;
                }
            }
        }

        // Atualiza a posição e ângulo da nave
        if (tecla_cima) {
            posicao_x_nave += cos(angulo_nave) * VELOCIDADE_NAVE;
            posicao_y_nave += sin(angulo_nave) * VELOCIDADE_NAVE;
        }
        if (tecla_baixo) {
            posicao_x_nave -= cos(angulo_nave) * VELOCIDADE_NAVE;
            posicao_y_nave -= sin(angulo_nave) * VELOCIDADE_NAVE;
        }

        // Limita a posição da nave para que ela não ultrapasse os limites da tela
        if (posicao_x_nave - largura_nave / 2 < 0) {
            posicao_x_nave = largura_nave / 2; // Limite esquerdo
        } else if (posicao_x_nave + largura_nave / 2 > LARGURA_TELA) {
            posicao_x_nave = LARGURA_TELA - largura_nave / 2; // Limite direito
        }

        if (posicao_y_nave - altura_nave / 2 < 0) {
            posicao_y_nave = altura_nave / 2; // Limite superior
        } else if (posicao_y_nave + altura_nave / 2 > ALTURA_TELA) {
            posicao_y_nave = ALTURA_TELA - altura_nave / 2; // Limite inferior
        }

        if (tecla_esquerda) {
            angulo_nave -= VELOCIDADE_ROTACAO; // Rotação para a esquerda
        }
        if (tecla_direita) {
            angulo_nave += VELOCIDADE_ROTACAO; // Rotação para a direita
        }


        // Atualiza a posição dos tiros
        for (int i = 0; i < MAX_TIROS; i++) {
            if (tiros[i].ativo) {
                tiros[i].x += tiros[i].dx;
                tiros[i].y += tiros[i].dy;

                // Desativa o tiro se sair da tela
                if (tiros[i].x < 0 || tiros[i].x > LARGURA_TELA || tiros[i].y < 0 || tiros[i].y > ALTURA_TELA) {
                    tiros[i].ativo = false;
                }
            }
        }

        // Renderização
        al_clear_to_color(al_map_rgb(0, 0, 0)); // Fundo preto

        // Renderiza o fundo
        if (imagem_fundo) {
            al_draw_bitmap(imagem_fundo, 0, 0, 0);
        }

        // Renderiza a nave redimensionada e rotacionada
        if (nave_redimensionada) {
            al_draw_rotated_bitmap(nave_redimensionada, largura_nave / 2, altura_nave / 2,
                                posicao_x_nave, posicao_y_nave, angulo_nave, 0);
        }

        // Renderiza os tiros
        for (int i = 0; i < MAX_TIROS; i++) {
            if (tiros[i].ativo) {
                al_draw_filled_circle(tiros[i].x, tiros[i].y, 5, al_map_rgb(255, 255, 0));
            }
        }

        al_flip_display();  
        
    }



    // Liberação de recursos
    if (imagem_fundo) {
        al_destroy_bitmap(imagem_fundo);
    }
    if (imagem_nave) {
        al_destroy_bitmap(imagem_nave);
    }
    if (display_game) {
        al_destroy_display(display_game);
    }
    al_destroy_event_queue(fila_eventos);
}