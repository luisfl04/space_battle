// Arquivo que contém toda a lógica principal da tela do game:

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
#define MAX_ANIMACOES 10

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

    // Estrutura para animações:
    typedef struct {
        float x, y;             // Posição da explosão
        int quadro_atual;       // Quadro atual da animação
        int total_quadros;      // Total de quadros na animação
        float tempo_por_quadro; // Tempo entre quadros
        float tempo_acumulado;  // Tempo acumulado para controle de quadros
        bool ativo;             // Estado da animação
    } Animacao;

// Variáveis globais usadas e ponteiros para arquivos de imagem, audio, fonte, display e etc... 
static ALLEGRO_SAMPLE *audio_disparo = NULL; // Audio do tiro da nave
static ALLEGRO_SAMPLE *audio_game_over = NULL; // Audio do game over
static ALLEGRO_SAMPLE *audio_inimigo_abatido = NULL; // Audio disparado quando inimigo é acertado:
static ALLEGRO_SAMPLE_ID *audio_inimigo_abatido_id = NULL; // Ponteiro usado para mapear o áudio de inimigo abatido
static ALLEGRO_DISPLAY *display_game = NULL; // Ponteiro para o display do jogo
static ALLEGRO_BITMAP *imagem_fundo = NULL; // Ponteiro imagem de fundo do game
static ALLEGRO_BITMAP *imagem_nave = NULL; // Ponteiro para a imagem da nave
static ALLEGRO_BITMAP *nave_redimensionada = NULL; // Ponteiro para a imagem em tamanho redimensionado
static ALLEGRO_BITMAP *imagem_fundo_pause = NULL; // Imagem de fundo no momento de pause do game
static Tiro tiros[MAX_TIROS]; // Inicializando estrutura de tiros da nave
static Inimigo inimigos[MAX_INIMIGOS]; // Inicializando estrutura de inimigos renderizados.
static ALLEGRO_BITMAP *imagem_inimigo = NULL; // Ponteiro para a imagem que renderiza o 'inimigo'
static int quantidade_inimigos = 1; // Variável que controla a quantidade de inimigos renderizados.
static bool sair = false; // Controle do loop principal do game
static bool pausado = false; // Controle do estado do jogo.
static int pontuacao = 0; // Controle da pontuação obtida pelo usuário.
static float velocidade_inimigos = 2.0;  // Inicialização do valor de velocidade de propagação dos inimigos.
static Animacao explosoes[MAX_ANIMACOES]; // Criação de inntância da estrutura da animação implementada no momento de game over.
static ALLEGRO_BITMAP *spritesheet_explosao = NULL;
static int largura_quadro = 0; // largura inicial da animação
static int altura_quadro = 0; // altura inicial da animação
static const float VELOCIDADE_NAVE = 5.0; // Controle da velocidade da nave
static const float VELOCIDADE_ROTACAO = 0.05; // Velocidade da rotação da nave:
static time_t tempo_inicial; // Controle do tempo em que o usuário está vivo no game.

// Funções gerais usadas no game:

    // Função para disparar um tiro
    void disparar_tiro(float x, float y, float angulo) {
        // Reproduz o áudio de disparo:
        if (audio_disparo) {
            al_play_sample(audio_disparo, 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
        }

        // Propagando disparos:
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

    // Função para salvar o record no arquivo, o record só é salvo quando o valor é maior do que o atual salvado.
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

    // Função para inicializar os inimigos renderizados:
    void inicializar_inimigos() {
        // inicializando:
        for (int i = 0; i < MAX_INIMIGOS; i++) {
            inimigos[i].ativo = false;
        }
        // Carregando imagem:
        imagem_inimigo = al_load_bitmap("./imagens/game/inimigo.png");
        if (!imagem_inimigo) {
            al_show_native_message_box(NULL, "Erro", "Não foi possível carregar a imagem do inimigo", "", NULL, ALLEGRO_MESSAGEBOX_ERROR);
            exit(1);
        }
    }

    // Função para criar inimigos definindo o local de renderização:
    void criar_inimigos() {
        for (int i = 0; i < quantidade_inimigos && i < MAX_INIMIGOS; i++) {
            if (!inimigos[i].ativo) {
                inimigos[i].x = rand() % LARGURA_TELA; // Posição aleatória no eixo X
                inimigos[i].y = 0; // Sempre no topo da tela.
                inimigos[i].ativo = true;

                // Define a coordenada de movimentação incial:
                inimigos[i].dx = 0;
                inimigos[i].dy = 0;
            }
        }
        quantidade_inimigos++; // Incrementa a quantidade de inimigos para a próxima onda
    }

    // Função que cria animação de explosão quando a nave é atingida.
    void criar_explosao(float x, float y, int largura_quadro, int altura_quadro) {
        for (int i = 0; i < MAX_ANIMACOES; i++) {
            if (!explosoes[i].ativo) {
                explosoes[i].x = x - largura_quadro / 2; // Centraliza a explosão
                explosoes[i].y = y - altura_quadro / 2; // Centraliza a explosão
                explosoes[i].quadro_atual = 0;
                explosoes[i].total_quadros = 64; // Quantidade total de quadros (8x8 na imagem)
                explosoes[i].tempo_por_quadro = 0.05; // 50ms por quadro
                explosoes[i].tempo_acumulado = 0;
                explosoes[i].ativo = true;
                break;
            }
        }
    }

    // Função para mover inimigos em direção à nave e verificar se a nave foi atingida:
    void mover_inimigos(float posicao_x_nave, float posicao_y_nave, float largura_inimigo, float altura_inimigo) {
        for (int i = 0; i < MAX_INIMIGOS; i++) {
            if (inimigos[i].ativo) {
                // Move o inimigo em direção a nave:
                float direcao_x = posicao_x_nave - inimigos[i].x;
                float direcao_y = posicao_y_nave - inimigos[i].y;
                float magnitude = sqrt(direcao_x * direcao_x + direcao_y * direcao_y);

                // Normaliza a direção e define velocidade:
                if (magnitude > 0) {
                    inimigos[i].dx = (direcao_x / magnitude) * velocidade_inimigos; // Velocidade fixa de 2
                    inimigos[i].dy = (direcao_y / magnitude) * velocidade_inimigos;
                }

                // Movimentando:
                inimigos[i].x += inimigos[i].dx;
                inimigos[i].y += inimigos[i].dy;

                // Verifica colisão com a nave:
                if (fabs(inimigos[i].x - posicao_x_nave) < largura_inimigo &&
                    fabs(inimigos[i].y - posicao_y_nave) < altura_inimigo) {

                    // Cria a explosão:
                    criar_explosao(posicao_x_nave, posicao_y_nave, largura_quadro, altura_quadro);
                    
                    // Tocar audio de game over:
                    audio_game_over = al_load_sample("./audio/game_over.wav");
                    al_play_sample(audio_game_over, 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL); 
                    
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


    // Função para renderizar os inimigos na tela:
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
                        // Verifica colisão entre tiro e inimigo:
                        if (fabs(tiros[i].x - inimigos[j].x) < largura_inimigo &&
                            fabs(tiros[i].y - inimigos[j].y) < altura_inimigo) {
                            // Desativa o inimigo e o tiro:
                            inimigos[j].ativo = false;
                            tiros[i].ativo = false;

                            // Toca o som de inimigo abatido:
                            if (audio_inimigo_abatido) {
                                al_play_sample(audio_inimigo_abatido, 1.0, 0.0, 2.0, ALLEGRO_PLAYMODE_ONCE, audio_inimigo_abatido_id);
                            }

                            // Incrementa a pontuação:
                            pontuacao += 5;

                            break; // Sai do loop de inimigos
                        }
                    }
                }
            }
        }
    }


    // Atualização dos frames relacionados a explosão do inimigo atingido: 
    void atualizar_explosoes(float delta_tempo) {
        // Itero os quadros da animação enquanto valor é menor que o definido na constante:
        for (int i = 0; i < MAX_ANIMACOES; i++) {
            if (explosoes[i].ativo) {
                explosoes[i].tempo_acumulado += delta_tempo;
                if (explosoes[i].tempo_acumulado >= explosoes[i].tempo_por_quadro) {
                    explosoes[i].quadro_atual++;
                    explosoes[i].tempo_acumulado = 0;

                    // Finaliza a animação quando todos os quadros forem exibidos:
                    if (explosoes[i].quadro_atual >= explosoes[i].total_quadros) {
                        explosoes[i].ativo = false;
                    }
                }
            }
        }
    }

    // Renderiza a explosão quando nave é atingida:
    void renderizar_explosoes() {
        int colunas = 8; // Número de colunas no spritesheet;
        int linhas = 8;  // Número de linhas no spritesheet;
        largura_quadro = al_get_bitmap_width(spritesheet_explosao) / colunas; // Largura do quadro na imagem
        altura_quadro = al_get_bitmap_height(spritesheet_explosao) / linhas; // Altura do quadro na imagem

        // Renderizando:
        for (int i = 0; i < MAX_ANIMACOES; i++) {
            if (explosoes[i].ativo) {
                int coluna = explosoes[i].quadro_atual % colunas;
                int linha = explosoes[i].quadro_atual / colunas;

                al_draw_bitmap_region(
                    spritesheet_explosao,
                    coluna * largura_quadro, linha * altura_quadro, largura_quadro, altura_quadro,
                    explosoes[i].x, explosoes[i].y, // Posiciona no centro do local
                    0
                );
            }
        }
    }
  
// Função principal da tela de jogo que contém toda a regra de negócio:
void carregarTelaJogo() {

    // Configurações gerais da tela:

        // Inicialização do Allegro:
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

        // Instala o driver de áudio reservando os samples.
        if (!al_reserve_samples(10)) { // Reservando 10 canais para o áudio
            al_show_native_message_box(NULL, "Erro", "Não foi possível inicializar o driver de áudio", " ", NULL, ALLEGRO_MESSAGEBOX_ERROR);
            return;
        }

        // Carregar o áudio de disparo de tiro:
        audio_disparo = al_load_sample("./audio/disparo.wav"); // Carrega o áudio de disparo
        if (!audio_disparo) {
            al_show_native_message_box(NULL, "Erro", "Não foi possível carregar o áudio de disparo", " ", NULL, ALLEGRO_MESSAGEBOX_ERROR);
            return;
        }
        
        // Carregar áudio de explosão do inimigo abatido:
        audio_inimigo_abatido = al_load_sample("./audio/inimigo_abatido.wav");
        if (!audio_inimigo_abatido) {
            al_show_native_message_box(NULL, "Erro", "Não foi possível carregar o áudio de explosão do inimigo", "", NULL, ALLEGRO_MESSAGEBOX_ERROR);
            exit(1);
        }

        // Criação da janela do jogo:
        display_game = al_create_display(LARGURA_TELA, ALTURA_TELA);
        if (!display_game) {
            al_show_native_message_box(NULL, "Erro", "Não foi possível criar a janela do jogo", " ", NULL, ALLEGRO_MESSAGEBOX_ERROR);
            return;
        }
        al_set_window_title(display_game, "Space Battle"); // Título da janela

        // Carrega a imagem de fundo da tela:
        imagem_fundo = al_load_bitmap("./imagens/fundo.png");
        if (!imagem_fundo) {
            al_show_native_message_box(NULL, "Erro", "Não foi possível carregar a imagem de fundo", "", NULL, ALLEGRO_MESSAGEBOX_ERROR);
            al_destroy_display(display_game);
            return;
        }

        // Carrega a imagem da nave:
        imagem_nave = al_load_bitmap("./imagens/game/nave_game.png");
        if (!imagem_nave) {
            al_show_native_message_box(NULL, "Erro", "Não foi possível carregar a imagem da nave", "", NULL, ALLEGRO_MESSAGEBOX_ERROR);
            al_destroy_display(display_game);
            al_destroy_bitmap(imagem_fundo);
            return;
        }

        // Imagem da animação de explosão:
        spritesheet_explosao = al_load_bitmap("./imagens/game/animacao_explosao.png");
        if (!spritesheet_explosao) {
            al_show_native_message_box(NULL, "Erro", "Não foi possível carregar o spritesheet de explosão", "", NULL, ALLEGRO_MESSAGEBOX_ERROR);
            exit(1);
        }

        // Inicializo os inimigos para fins de otimização: 
        inicializar_inimigos();

        // Redimensiona a nave para um tamanho proporcional(A imagem original tem um tamanho grande):
        float largura_nave = LARGURA_TELA * 0.1; // 10% da largura da tela
        float proporcao_nave = largura_nave / al_get_bitmap_width(imagem_nave); // Calcula proporção
        float altura_nave = al_get_bitmap_height(imagem_nave) * proporcao_nave; // obtém altura.
        nave_redimensionada = al_create_bitmap(largura_nave, altura_nave);
        ALLEGRO_BITMAP *anterior = al_get_target_bitmap();
        al_set_target_bitmap(nave_redimensionada); // Defino o alvo do ponteiro para a nave redimensionada
        al_draw_scaled_bitmap(imagem_nave, 0, 0, al_get_bitmap_width(imagem_nave), al_get_bitmap_height(imagem_nave),
                            0, 0, largura_nave, altura_nave, 0); // Desenho a nave
        al_set_target_bitmap(anterior); // Mudo o alvo do ponteiro para a variável auxiliar criada.

        // Coordenadas iniciais e ângulo da nave:
        float posicao_x_nave = LARGURA_TELA / 2;
        float posicao_y_nave = ALTURA_TELA / 2;
        float angulo_nave = 0.0;

        // Configuração do sistema de eventos:
        ALLEGRO_EVENT_QUEUE *fila_eventos = al_create_event_queue();
        al_register_event_source(fila_eventos, al_get_display_event_source(display_game));
        al_register_event_source(fila_eventos, al_get_keyboard_event_source());

        // Inicializa tiros para fins de otimização:
        for (int i = 0; i < MAX_TIROS; i++) {
            tiros[i].ativo = false;
        }

    // Loop principal do game:

        // Variáveis de controle das teclas pressionadas:
        bool tecla_cima = false, tecla_baixo = false, tecla_esquerda = false, tecla_direita = false;

        // Variáveis de controle do tempo no game:
        tempo_inicial = time(NULL); // Tempo inicial do game
        time_t ultimo_tempo = tempo_inicial; // Ultimo tempo do game para incrementação.
        time_t tempo_ultimo_inimigo = time(NULL); // Tempo da velocidade do ultimo inimigo renderizado
        time_t ultimo_incremento_velocidade = time(NULL); // Controle velocidade do inimigo

        // Loop de verificação dos eventos:
        while (!sair) {
            ALLEGRO_EVENT evento;
            while (al_get_next_event(fila_eventos, &evento)) {
                if (evento.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
                    sair = true; // Casos onde fecha a tela
                } else if (evento.type == ALLEGRO_EVENT_KEY_DOWN) {
                    // Verificação da tecla pressionada:
                    if (evento.keyboard.keycode == ALLEGRO_KEY_ESCAPE) {
                        sair = true; // Retornar ao menu inicial
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
                } else if (evento.type == ALLEGRO_EVENT_KEY_UP) { // Processamento dos valores após o pressionamento da tecla(Os valores são reiniciados) 
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
            // Processamento de valores após verificação do pressionamento da tecla:
                
                // Renderiza mensagem de pausa caso o usuário pressionou 'P':
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

                // Atualizando valores dos inimigos a cada 5 segundos:
                if (difftime(time(NULL), tempo_ultimo_inimigo) >= 5) {
                    criar_inimigos();
                    tempo_ultimo_inimigo = time(NULL);
                }

                // Movimento dos inimigos:
                mover_inimigos(posicao_x_nave, posicao_y_nave, largura_nave / 2, altura_nave / 2);

                // Verifica colisão dos tiros com os inimigos:
                verificar_colisao_tiros_inimigos(5, 5, largura_nave / 2, altura_nave / 2);

                // Incrementando velocidade do inimigo a cada loop:
                if (difftime(time(NULL), ultimo_incremento_velocidade) >= 10) { // Incrementa a cada 10 segundos
                    velocidade_inimigos += 0.2; // Incremento mínimo na velocidade
                    ultimo_incremento_velocidade = time(NULL);
                }

                // Atualiza a posição e ângulo da nave de acordo com a tecla pressionada relação ao valores de ida(cima) e volta(baixo):
                if (tecla_cima) {
                    posicao_x_nave += cos(angulo_nave) * VELOCIDADE_NAVE;
                    posicao_y_nave += sin(angulo_nave) * VELOCIDADE_NAVE;
                }
                if (tecla_baixo) {
                    posicao_x_nave -= cos(angulo_nave) * VELOCIDADE_NAVE;
                    posicao_y_nave -= sin(angulo_nave) * VELOCIDADE_NAVE;
                }

                // Limita a posição da nave para que ela não ultrapasse os limites da tela:
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
                
                // Rotaciono a nave conforme a tecla pressionada:
                if (tecla_esquerda) {
                    angulo_nave -= VELOCIDADE_ROTACAO; // Rotação para a esquerda
                }
                if (tecla_direita) {
                    angulo_nave += VELOCIDADE_ROTACAO; // Rotação para a direita
                }

                // Atualiza a posição dos tiros a cada loop e movimentação da nave:
                for (int i = 0; i < MAX_TIROS; i++) {
                    if (tiros[i].ativo) {
                        tiros[i].x += tiros[i].dx;
                        tiros[i].y += tiros[i].dy;

                        // Desativa o tiro se sair da tela:
                        if (tiros[i].x < 0 || tiros[i].x > LARGURA_TELA || tiros[i].y < 0 || tiros[i].y > ALTURA_TELA) {
                            tiros[i].ativo = false;
                        }
                    }
                }
                
                // Atualiza o tempo e pontuação do usuário:
                time_t tempo_atual = time(NULL);
                if (tempo_atual - ultimo_tempo >= 5) {
                    pontuacao += 1;
                    ultimo_tempo = tempo_atual;
                }

                // Renderização dos atores:

                    // Renderização do fundo da tela:
                    al_clear_to_color(al_map_rgb(0, 0, 0)); // Fundo preto

                    // Renderiza da imagem de fundo:
                    if (imagem_fundo) {
                        al_draw_bitmap(imagem_fundo, 0, 0, 0);
                    }

                    // Renderiza inimigos:
                    renderizar_inimigos(largura_nave / 2, altura_nave / 2);
                    
                    // Renderiza a nave redimensionada e rotacionada:
                    if (nave_redimensionada) {
                        al_draw_rotated_bitmap(nave_redimensionada, largura_nave / 2, altura_nave / 2,
                                            posicao_x_nave, posicao_y_nave, angulo_nave, 0);
                    }

                    // Renderiza pontuação:
                    char texto_pontuacao[50];
                    snprintf(texto_pontuacao, sizeof(texto_pontuacao), "Pontuação: %d", pontuacao);
                    al_draw_text(al_create_builtin_font(), al_map_rgb(255, 255, 255), 10, 10, 0, texto_pontuacao);

                    // Renderiza os tiros:
                    for (int i = 0; i < MAX_TIROS; i++) {
                        if (tiros[i].ativo) {
                            al_draw_filled_circle(tiros[i].x, tiros[i].y, 5, al_map_rgb(255, 255, 0)); // circulo amarelo.
                        }
                    }

                    // Atualiza a animação de explosões para fins de otimização:
                    atualizar_explosoes(0.016);
                    renderizar_explosoes();

                    // Atualizando tela:
                    al_flip_display();  
                    
                    // Pausa suave no loop:
                    al_rest(0.01);
        }

    // Após sair do loop, salvo pontuação obtida:
    salvar_record(pontuacao);
    printf("Pontuação obtida -> %d \n", pontuacao); // debug

    // Liberação de ponteiros usados:
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
    sair = false; // Defino sair como 'false' para que a execulção entre no loop quando a função for chamada novamente.
}