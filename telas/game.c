// Libs usadas:
#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_native_dialog.h>
#include <allegro5/allegro_primitives.h>
#include <stdio.h>
#include <math.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>
#include <stdlib.h>


// Constantes usadas:
#define MAX_INIMIGOS 100
#define MAX_TIROS 10
#define LARGURA_TELA 1366
#define ALTURA_TELA 768

// Estruturas usadas:
    // Estrutura para um inimigo:
    typedef struct {
        float x, y;       // Posição
        float dx, dy;     // Direção do movimento
        bool ativo;       // Estado do inimigo
    } Inimigo;

    // Estrutura para um tiro
    typedef struct {
        float x, y;
        float dx, dy; // Direção do movimento
        bool ativo;
    } Tiro;

// Variáveis globais
static ALLEGRO_SAMPLE *audio_disparo = NULL; 
static ALLEGRO_SAMPLE *audio_game_over = NULL;
static ALLEGRO_SAMPLE *audio_inimigo_abatido = NULL;
static ALLEGRO_SAMPLE_ID *audio_inimigo_abatido_id = NULL;
static ALLEGRO_DISPLAY *display_game = NULL;
static ALLEGRO_BITMAP *imagem_fundo = NULL;
static ALLEGRO_BITMAP *imagem_nave = NULL;
static ALLEGRO_BITMAP *nave_redimensionada = NULL;
static ALLEGRO_BITMAP *imagem_fundo_pause = NULL;
static Tiro tiros[MAX_TIROS];
static Inimigo inimigos[MAX_INIMIGOS];
static ALLEGRO_BITMAP *imagem_inimigo = NULL;
static int quantidade_inimigos = 1;
static bool sair = false;
static bool pausado = false;
static int pontuacao = 0;
static float velocidade_inimigos = 2.0; // Velocidade inicial


// Configurações gerais:

    // Velocidade da nave e da rotação
    static const float VELOCIDADE_NAVE = 5.0;
    static const float VELOCIDADE_ROTACAO = 0.05; // Em radianos

    // Variáveis para pontuação
    static time_t tempo_inicial;

// Funções gerais usadas no game:

    // Função para disparar um tiro
    void disparar_tiro(float x, float y, float angulo) {
        // Reproduz o áudio de disparo
        if (audio_disparo) {
            al_play_sample(audio_disparo, 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
        }

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

    // Função para salvar o record no arquivo
    void salvar_record(int pontuacao_atual) {
        FILE *arquivo = fopen("./artefatos/record.txt", "r");
        int record_atual = 0;

        if (arquivo) {
            fscanf(arquivo, "%d", &record_atual);
            fclose(arquivo);
        }

        if (pontuacao_atual > record_atual) {
            arquivo = fopen("./artefatos/record.txt", "w");
            if (arquivo) {
                fprintf(arquivo, "%d", pontuacao_atual);
                fclose(arquivo);
            }
        }
        else{
            // Se a pontuação nao foi record, descarto ela:
            pontuacao = 0;
        }
    }

    // Função para inicializar os inimigos
    void inicializar_inimigos() {
        for (int i = 0; i < MAX_INIMIGOS; i++) {
            inimigos[i].ativo = false;
        }
        imagem_inimigo = al_load_bitmap("./imagens/game/inimigo.png");
        if (!imagem_inimigo) {
            al_show_native_message_box(NULL, "Erro", "Não foi possível carregar a imagem do inimigo", "", NULL, ALLEGRO_MESSAGEBOX_ERROR);
            exit(1);
        }
    }

    // Função para criar inimigos
    void criar_inimigos() {
        for (int i = 0; i < quantidade_inimigos && i < MAX_INIMIGOS; i++) {
            if (!inimigos[i].ativo) {
                inimigos[i].x = rand() % LARGURA_TELA; // Posição aleatória no eixo X
                inimigos[i].y = 0; // Sempre no topo da tela.
                inimigos[i].ativo = true;

                // Define a velocidade inicial
                inimigos[i].dx = 0;
                inimigos[i].dy = 0;
            }
        }
        quantidade_inimigos++; // Incrementa a quantidade de inimigos para a próxima onda
    }

    // Função para mover inimigos em direção à nave:
    void mover_inimigos(float posicao_x_nave, float posicao_y_nave, float largura_inimigo, float altura_inimigo) {
        for (int i = 0; i < MAX_INIMIGOS; i++) {
            if (inimigos[i].ativo) {
                float direcao_x = posicao_x_nave - inimigos[i].x;
                float direcao_y = posicao_y_nave - inimigos[i].y;
                float magnitude = sqrt(direcao_x * direcao_x + direcao_y * direcao_y);

                // Normaliza a direção e define velocidade
                if (magnitude > 0) {
                    inimigos[i].dx = (direcao_x / magnitude) * velocidade_inimigos; // Velocidade fixa de 2
                    inimigos[i].dy = (direcao_y / magnitude) * velocidade_inimigos;
                }

                inimigos[i].x += inimigos[i].dx;
                inimigos[i].y += inimigos[i].dy;

                // Verifica colisão com a nave
                if (fabs(inimigos[i].x - posicao_x_nave) < largura_inimigo &&
                    fabs(inimigos[i].y - posicao_y_nave) < altura_inimigo) {

                    // Tocar audio de game over:
                    audio_game_over = al_load_sample("./audio/game_over.wav");
                    al_play_sample(audio_game_over, 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL); 
                    
                    al_show_native_message_box(NULL, "Game Over", "Você foi atingido pelo inimigo!", "Clique em OK para reiniciar", NULL, ALLEGRO_MESSAGEBOX_ERROR);
                    
                    // Salvar pontuação:
                    salvar_record(pontuacao);
                    
                    // Reiniciar o estado do jogo
                    quantidade_inimigos = 1; // Reinicia o número de inimigos
                    pontuacao = 0;          // Zera a pontuação
                    velocidade_inimigos = 2.0; // Volta a velocidade inicial.

                    // Reposiciona a nave no centro
                    posicao_x_nave = LARGURA_TELA / 2;
                    posicao_y_nave = ALTURA_TELA / 2;

                    // Desativa todos os inimigos
                    for (int j = 0; j < MAX_INIMIGOS; j++) {
                        inimigos[j].ativo = false;
                    }

                    // Reinicializa o game:
                    return;
                }
            }
        }
    }


    // Função para renderizar os inimigos
    void renderizar_inimigos(float largura_inimigo, float altura_inimigo) {
        for (int i = 0; i < MAX_INIMIGOS; i++) {
            if (inimigos[i].ativo) {
                al_draw_scaled_bitmap(imagem_inimigo, 0, 0, al_get_bitmap_width(imagem_inimigo),
                                    al_get_bitmap_height(imagem_inimigo), inimigos[i].x, inimigos[i].y,
                                    largura_inimigo, altura_inimigo, 0);
            }
        }
    }

    // Função para verificar colisão entre tiro e inimigo:
    void verificar_colisao_tiros_inimigos(float largura_tiro, float altura_tiro, float largura_inimigo, float altura_inimigo) {
        for (int i = 0; i < MAX_TIROS; i++) {
            if (tiros[i].ativo) {
                for (int j = 0; j < MAX_INIMIGOS; j++) {
                    if (inimigos[j].ativo) {
                        // Verifica colisão entre tiro e inimigo
                        if (fabs(tiros[i].x - inimigos[j].x) < largura_inimigo &&
                            fabs(tiros[i].y - inimigos[j].y) < altura_inimigo) {
                            // Desativa o inimigo e o tiro
                            inimigos[j].ativo = false;
                            tiros[i].ativo = false;

                            // Toca o som de explosão
                            if (audio_inimigo_abatido) {
                                al_play_sample(audio_inimigo_abatido, 1.0, 0.0, 2.0, ALLEGRO_PLAYMODE_ONCE, audio_inimigo_abatido_id);
                            }

                            // Incrementa a pontuação
                            pontuacao += 5;

                            break; // Sai do loop de inimigos
                        }
                    }
                }
            }
        }
    }


// Função principal da tela de jogo
void carregarTelaJogo() {
    // Inicialização do Allegro
    if (!al_init()) {
        al_show_native_message_box(NULL, "Erro", "Não foi possível inicializar o Allegro", " ", NULL, ALLEGRO_MESSAGEBOX_ERROR);
        return;
    }

    // Addons necessários:
    al_init_image_addon();
    al_install_keyboard();
    al_init_primitives_addon();

    // Inicializa o sistema de áudio
    if (!al_install_audio()) {
        al_show_native_message_box(NULL, "Erro", "Não foi possível inicializar o sistema de áudio", " ", NULL, ALLEGRO_MESSAGEBOX_ERROR);
        return;
    }

    // Inicializa os codecs de áudio
    if (!al_init_acodec_addon()) {
        al_show_native_message_box(NULL, "Erro", "Não foi possível inicializar o codec de áudio", " ", NULL, ALLEGRO_MESSAGEBOX_ERROR);
        return;
    }

    // Instala o driver de áudio
    if (!al_reserve_samples(10)) { // Reservando 1 canal para o áudio
        al_show_native_message_box(NULL, "Erro", "Não foi possível inicializar o driver de áudio", " ", NULL, ALLEGRO_MESSAGEBOX_ERROR);
        return;
    }

    // Carregar o áudio de disparo
    audio_disparo = al_load_sample("./audio/disparo.wav"); // Carrega o áudio de disparo
    if (!audio_disparo) {
        al_show_native_message_box(NULL, "Erro", "Não foi possível carregar o áudio de disparo", " ", NULL, ALLEGRO_MESSAGEBOX_ERROR);
        return;
    }
    
    // Carregar áudio de explosão do inimigo:
    audio_inimigo_abatido = al_load_sample("/home/luisfl/Documentos/prog/space_battle/audio/inimigo_abatido.wav");
    if (!audio_inimigo_abatido) {
        al_show_native_message_box(NULL, "Erro", "Não foi possível carregar o áudio de explosão do inimigo", "", NULL, ALLEGRO_MESSAGEBOX_ERROR);
        exit(1);
    }

    // Criação da janela do jogo
    display_game = al_create_display(LARGURA_TELA, ALTURA_TELA);
    if (!display_game) {
        al_show_native_message_box(NULL, "Erro", "Não foi possível criar a janela do jogo", " ", NULL, ALLEGRO_MESSAGEBOX_ERROR);
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

    // Inicializando inimigos:
    inicializar_inimigos();


    // Redimensiona a nave para um tamanho proporcional:
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

    // Variáveis de controle das teclas pressionadas:
    bool tecla_cima = false, tecla_baixo = false, tecla_esquerda = false, tecla_direita = false;

    // Variáveis de controle do tempo no game:
    tempo_inicial = time(NULL);
    time_t ultimo_tempo = tempo_inicial;
    time_t tempo_ultimo_inimigo = time(NULL);
    time_t ultimo_incremento_velocidade = time(NULL);

    // Modificar o controle das teclas no loop de eventos
    while (!sair) {
        ALLEGRO_EVENT evento;
        while (al_get_next_event(fila_eventos, &evento)) {
            if (evento.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
                sair = true;
            } else if (evento.type == ALLEGRO_EVENT_KEY_DOWN) {
                if (evento.keyboard.keycode == ALLEGRO_KEY_ESCAPE) {
                    sair = true; // Retornar ao menu
                } else if (evento.keyboard.keycode == ALLEGRO_KEY_P) {
                    pausado = !pausado; // Alterna entre pausado e em execução
                } else if (!pausado) { // Somente processa controles se não estiver pausado
                    if (evento.keyboard.keycode == ALLEGRO_KEY_UP) {
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
                }
            } else if (evento.type == ALLEGRO_EVENT_KEY_UP) {
                if (!pausado) { // Apenas processa quando não está pausado
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
        }

        // Renderiza mensagem de pausa:
        if (pausado) {
            // Carregando fonte:
            ALLEGRO_FONT *fonte_pausa = al_load_ttf_font("./fonts/roboto/Roboto-Medium.ttf", 48, 0);
            if (!fonte_pausa) {
                al_show_native_message_box(NULL, "Erro", "Não foi possível carregar a fonte para pausa", "", NULL, ALLEGRO_MESSAGEBOX_ERROR);
                return;
            }
            // Renderizando:
            al_clear_to_color(al_map_rgb(0, 0, 0)); // Fundo preto
            imagem_fundo_pause = al_load_bitmap("./imagens/fundo.png");
            al_draw_bitmap(imagem_fundo_pause, 0, 0, 0);
            al_draw_text(fonte_pausa, al_map_rgb(255, 255, 255), LARGURA_TELA / 2, ALTURA_TELA / 2,
                        ALLEGRO_ALIGN_CENTER, "PAUSADO - Pressione P para continuar");
            al_flip_display();
            al_rest(0.1); // Reduz a carga de CPU enquanto pausado
            continue; // Salta o resto do loop
        }

        // Atualiza inimigos a cada 5 segundos
        if (difftime(time(NULL), tempo_ultimo_inimigo) >= 5) {
            criar_inimigos();
            tempo_ultimo_inimigo = time(NULL);
        }
        // Movimento dos inimigos
        mover_inimigos(posicao_x_nave, posicao_y_nave, largura_nave / 2, altura_nave / 2);

        // Verifica colisão dos tiros com os inimigos:
        verificar_colisao_tiros_inimigos(5, 5, largura_nave / 2, altura_nave / 2); // Ajuste os tamanhos conforme necessário

        // Incrementando velocidade do inimigo:
        if (difftime(time(NULL), ultimo_incremento_velocidade) >= 10) { // Incrementa a cada 10 segundos
            velocidade_inimigos += 0.2; // Incremento mínimo na velocidade
            ultimo_incremento_velocidade = time(NULL);
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
        
        // Atualiza o tempo e pontuação
        time_t tempo_atual = time(NULL);
        if (tempo_atual - ultimo_tempo >= 5) {
            pontuacao += 1;
            ultimo_tempo = tempo_atual;
        }

        // Renderização
        al_clear_to_color(al_map_rgb(0, 0, 0)); // Fundo preto

        // Renderiza o fundo
        if (imagem_fundo) {
            al_draw_bitmap(imagem_fundo, 0, 0, 0);
        }
        renderizar_inimigos(largura_nave / 2, altura_nave / 2); // Renderiza inimigos
        
        // Renderiza a nave redimensionada e rotacionada
        if (nave_redimensionada) {
            al_draw_rotated_bitmap(nave_redimensionada, largura_nave / 2, altura_nave / 2,
                                posicao_x_nave, posicao_y_nave, angulo_nave, 0);
        }

        // Renderiza pontuação
        char texto_pontuacao[50];
        snprintf(texto_pontuacao, sizeof(texto_pontuacao), "Pontuação: %d", pontuacao);
        al_draw_text(al_create_builtin_font(), al_map_rgb(255, 255, 255), 10, 10, 0, texto_pontuacao);

        // Renderiza os tiros
        for (int i = 0; i < MAX_TIROS; i++) {
            if (tiros[i].ativo) {
                al_draw_filled_circle(tiros[i].x, tiros[i].y, 5, al_map_rgb(255, 255, 0));
            }
        }
        

        // Atualizando tela:
        al_flip_display();  
        al_rest(0.01);
    }

    // Salvando pontuação obtida:
    salvar_record(pontuacao);
    printf("Pontuação obtida -> %d \n", pontuacao); // debug

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
    if (nave_redimensionada) {
        al_destroy_bitmap(nave_redimensionada);
    }
    if(imagem_fundo_pause){
        al_destroy_bitmap(imagem_fundo_pause);
    }
    if(imagem_inimigo){
        al_destroy_bitmap(imagem_inimigo);
    }
    if(audio_disparo){
        al_destroy_sample(audio_disparo);
    }
    if(audio_game_over){
        al_destroy_sample(audio_game_over);
    }
    if(audio_inimigo_abatido){
        al_destroy_sample(audio_inimigo_abatido);
    }

    al_destroy_event_queue(fila_eventos); // Destruindo fila de eventos.
}