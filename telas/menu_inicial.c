// Arquivo destinado a tela de menu inicial do game:

// Libs usadas:
#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>
#include <allegro5/allegro_native_dialog.h>
#include <allegro5/allegro_image.h>
#include <stdio.h>
#include <string.h>

// Funções externas de outros arquivos usadas:
extern void carregarTelaJogo(); // função da tela do game.

// Configurações básicas de inicialização da tela:
static ALLEGRO_DISPLAY *display = NULL;
static ALLEGRO_FONT *fonte_geral = NULL;
static ALLEGRO_FONT *fonte_mensagem_boas_vindas = NULL;
static ALLEGRO_FONT *fonte_rendereizacao_usuario = NULL;
static ALLEGRO_FONT *fonte_rendereizacao_valores_usuario = NULL;
static ALLEGRO_SAMPLE *menu_de_musica = NULL;
static ALLEGRO_SAMPLE_ID musica_id;
static ALLEGRO_BITMAP *imagem_fundo = NULL;
static ALLEGRO_BITMAP *imagem_nave = NULL;
static ALLEGRO_BITMAP *icone_musica_ligada = NULL;
static ALLEGRO_BITMAP *icone_musica_desligada = NULL;
static bool musica_tocando = true;
static int largura_janela = 1366;
static int altura_janela = 768;

// Função que zera o valor do record salvo no arquivo:
void zerar_record() {
    FILE *arquivo = fopen("./artefatos/record.txt", "w");
    if (arquivo) {
        fprintf(arquivo, "%d", 0);
        fclose(arquivo);
    }
}

// Função para alternar o estado da música:
static void alternar_musica() {
    if (musica_tocando) {
        al_stop_sample(&musica_id);
        musica_tocando = false;
    } else {
        al_play_sample(menu_de_musica, 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_LOOP, &musica_id);
        musica_tocando = true;
    }
}

// Função para desenhar o botão de música
static void desenhar_botao_musica(float x, float y, float largura, float altura) {
    // Limpa a área do botão para evitar sobreposição
    al_draw_filled_rectangle(x, y, x + largura, y + altura, al_map_rgb(0, 0, 0));

    ALLEGRO_BITMAP *icone = musica_tocando ? icone_musica_ligada : icone_musica_desligada;
    if (icone) {
        al_draw_scaled_bitmap(icone, 0, 0, al_get_bitmap_width(icone), al_get_bitmap_height(icone), x, y, largura, altura, 0);
    } else {
        // Caso o ícone não seja carregado, desenha um botão simples
        al_draw_filled_rectangle(x, y, x + largura, y + altura, al_map_rgb(200, 200, 200));
        const char *texto = musica_tocando ? "Música: Ligada" : "Música: Mutada";
        al_draw_text(fonte_geral, al_map_rgb(0, 0, 0), x + largura / 2, y + altura / 4, ALLEGRO_ALIGN_CENTER, texto);
    }
}

// Função para ler o nome do usuário a partir do arquivo:
static void obter_nome_usuario(char *nome, size_t tamanho) {
    FILE *arquivo = fopen("./artefatos/username.txt", "r");
    if (arquivo) {
        fgets(nome, tamanho, arquivo);
        nome[strcspn(nome, "\n")] = 0; // Remove o caractere de nova linha, se existir
        fclose(arquivo);
    } else {
        strncpy(nome, "Usuário Desconhecido", tamanho);
    }
}

// Função para ler o record do usuário a partir do arquivo:
static int obter_record_usuario() {
    FILE *arquivo = fopen("./artefatos/record.txt", "r");
    int record = 0;
    if (arquivo) {
        fscanf(arquivo, "%d", &record);
        fclose(arquivo);
    }
    return record;
}

// Função para desenhar botão de iniciar e fechar jogo:
static void desenharBotao(float x, float y, float largura, float altura, const char *texto, ALLEGRO_COLOR cor_fundo, ALLEGRO_COLOR cor_texto) {
    /* Função responsável por renderizar os botões usados no menu inicial. Ela pode ser usada em outras implementações, visto que desenha os botões
    de acordo com o parâmetros passados. */
    
    // Desenha os botões:
    al_draw_filled_rectangle(x, y, x + largura, y + altura, cor_fundo);
    float texto_x = x + (largura - al_get_text_width(fonte_geral, texto)) / 2;
    float texto_y = y + (altura - al_get_font_line_height(fonte_geral)) / 2;
    al_draw_text(fonte_geral, cor_texto, texto_x, texto_y, 0, texto);
}

void carregarMenuInicial() {

    /* Essa função é responsável por carregar o menu inicial do jogo que representa também o menu de pause. O usuário irá usá-la sempre que 
    o game for pausado ou quando o game for iniciado pela primeira vez */
    
    // Configurações gerais da tela:
        
        // Inicializando allegro:
        if (!al_init()) {
            al_show_native_message_box(NULL, "Erro", "Não foi possível inicializar o Allegro", NULL, NULL, ALLEGRO_MESSAGEBOX_ERROR);
            return;
        }

        // Cria a janela com o tamanho especificado:
        display = al_create_display(largura_janela, altura_janela);
        if (!display) {
            al_show_native_message_box(NULL, "Erro", "Não foi possível criar a janela", "", NULL, ALLEGRO_MESSAGEBOX_ERROR);
            return;
        }
        al_set_window_title(display, "Menu inicial"); // Definindo titulo da janela

        // Inicializa addons necessários:
        al_init_font_addon();
        al_init_ttf_addon();
        al_init_primitives_addon();
        al_install_audio();
        al_init_acodec_addon();
        al_init_image_addon();

        // Configuração de eventos:
        ALLEGRO_EVENT_QUEUE *fila_eventos = al_create_event_queue();
        al_install_mouse();
        al_register_event_source(fila_eventos, al_get_display_event_source(display));
        al_register_event_source(fila_eventos, al_get_mouse_event_source());

        // Carregando fonte geral:
        fonte_geral = al_load_ttf_font("./fonts/roboto/Roboto-Regular.ttf", 48, 0);
        if (!fonte_geral) {
            al_show_native_message_box(NULL, "Erro", "Não foi possível carregar a fonte geral", "", NULL, ALLEGRO_MESSAGEBOX_ERROR);
            al_destroy_display(display);
            return;
        }

        // Carregando música:
        al_reserve_samples(1);
        menu_de_musica = al_load_sample("./audio/menu_music.wav");
        if (!menu_de_musica) {
            al_show_native_message_box(NULL, "Erro", "Não foi possível carregar a música do menu", "", NULL, ALLEGRO_MESSAGEBOX_ERROR);
        } else {
            // Toca a música em loop:
            al_play_sample(menu_de_musica, 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_LOOP, NULL);
            musica_tocando = true;
        }

        // Carregando os ícones do botão de alternar música:
        icone_musica_ligada = al_load_bitmap("./imagens/menu_inicial/sound_on.png");
        icone_musica_desligada = al_load_bitmap("./imagens/menu_inicial/sound_off.png");
        if(!icone_musica_ligada || !icone_musica_desligada) {
            al_show_native_message_box(NULL, "Erro", "Não foi possível carregar os ícones do botão de musica", "", NULL, ALLEGRO_MESSAGEBOX_ERROR);
        }

        // Prepara o fundo e carrega a imagem com estrelas sobre o fundo:
        al_clear_to_color(al_map_rgb(0, 0, 0));
        imagem_fundo = al_load_bitmap("./imagens/menu_inicial/fundo_estrelas_menu_inicial.png");
        if (!imagem_fundo) {
            al_show_native_message_box(NULL, "Erro", "Não foi possível carregar a imagem de fundo", "", NULL, ALLEGRO_MESSAGEBOX_ERROR);
        } else {
            // Desenha a imagem na tela
            al_draw_bitmap(imagem_fundo, 0, 0, 0);
        }

        // Configurando os botões e os desenhando na tela:
        float largura_botao = 380;
        float altura_botao = 90;
        float x_botoes = largura_janela / 2 - largura_botao / 2;
        float y_botao_iniciar = altura_janela / 2 - altura_botao - 10;
        float y_botao_fechar = altura_janela / 2 + 10;

        // Desenhando botões de iniciar e fechar jogo:
        desenharBotao(x_botoes, y_botao_iniciar, largura_botao, altura_botao, "Iniciar jogo", al_map_rgb(50, 150, 50), al_map_rgb(255, 255, 255));
        desenharBotao(x_botoes, y_botao_fechar, largura_botao, altura_botao, "Fechar", al_map_rgb(150, 50, 50), al_map_rgb(255, 255, 255));

        // Botão de música no canto inferior direito
        float largura_botao_musica = 64;
        float altura_botao_musica = 64;
        float x_botao_musica = largura_janela - largura_botao_musica - 100;
        float y_botao_musica = altura_janela - altura_botao_musica - 100;
        desenhar_botao_musica(x_botao_musica, y_botao_musica, largura_botao_musica, altura_botao_musica);

    // Desenhando mensagem de boas vindas:
        
        // Carregando a imagem da nave que é renderizada ao lado da mensagem de boas vindas:
        imagem_nave = al_load_bitmap("./imagens/game/nave_game.png");
        if (!imagem_nave) {
            al_show_native_message_box(NULL, "Erro", "Não foi possível carregar a imagem da nave", "", NULL, ALLEGRO_MESSAGEBOX_ERROR);
        }

        // Carregando a fonte:
        fonte_mensagem_boas_vindas = al_load_ttf_font("./fonts/roboto/Roboto-Bold.ttf", 55, 0);
        if (!fonte_mensagem_boas_vindas) {
            al_show_native_message_box(NULL, "Erro", "Não foi possível carregar a fonte de boas vindas", "", NULL, ALLEGRO_MESSAGEBOX_ERROR);
            al_destroy_display(display);
            return;
        }

        // Configurações da mensagem de boas vindas:
        const char *mensagem = "Bem-vindo ao Space Game!";
        float largura_mensagem = al_get_text_width(fonte_mensagem_boas_vindas, mensagem);
        float altura_mensagem = al_get_font_line_height(fonte_mensagem_boas_vindas);
        float x_mensagem = largura_janela / 4;
        float y_mensagem = altura_janela / 4;
        
        // Desenhando a mensagem:
        al_draw_text(fonte_mensagem_boas_vindas, al_map_rgb(255, 255, 255), x_mensagem , y_mensagem - 70, 0, mensagem);

        // Redimensiona e desenha a imagem da nave ao lado do texto:
        if (imagem_nave) {
            // Obtendo o tamanho da nave original:
            int largura_original = al_get_bitmap_width(imagem_nave);
            int altura_original = al_get_bitmap_height(imagem_nave);

            // Redimensionando a nave para fins de organização do seu tamanho:
            ALLEGRO_BITMAP *nave_redimensionada = al_create_bitmap(largura_original / 2, altura_original / 2);
            ALLEGRO_BITMAP *anterior = al_get_target_bitmap();
            al_set_target_bitmap(nave_redimensionada);

            // Desenhando a imagem escalada ao lado do texto:
            al_draw_scaled_bitmap(
                imagem_nave,
                0, 0, largura_original, altura_original, // Dimensões originais
                0, 0, (largura_original / 2) - 40, (altura_original / 2) - 40, // Dimensões reduzidas
                0
            );

            al_set_target_bitmap(anterior); // Mudando o alvo do ponteiro bitmap.

            // Desenhando a nave redimensionada ao lado do texto:
            al_draw_bitmap(nave_redimensionada, x_mensagem + largura_mensagem + 25, y_mensagem - 60, 0);

            // Liberando o recurso:
            al_destroy_bitmap(nave_redimensionada);
        }

    // Desenhando o nome do usuário e o record:

        // Configurando as fontes:
            // Fonte para os campos chave:
            fonte_rendereizacao_usuario = al_load_ttf_font("./fonts/roboto/Roboto-Bold.ttf", 35, 0);
            if (!fonte_rendereizacao_usuario) {
                al_show_native_message_box(NULL, "Erro", "Não foi possível carregar a fonte geral", "", NULL, ALLEGRO_MESSAGEBOX_ERROR);
                al_destroy_display(display);
                return;
            }

            // fonte para os valores:
            fonte_rendereizacao_valores_usuario = al_load_ttf_font("./fonts/roboto/Roboto-Regular.ttf", 40, 0);
            if (!fonte_rendereizacao_valores_usuario) {
                al_show_native_message_box(NULL, "Erro", "Não foi possível carregar a fonte geral", "", NULL, ALLEGRO_MESSAGEBOX_ERROR);
                al_destroy_display(display);
                return;
            }

        // Obtendo nome do usuário e record:
        char nome_usuario[128];
        obter_nome_usuario(nome_usuario, sizeof(nome_usuario));
        int record_usuario = obter_record_usuario();

        // Posições fixas para os textos "Usuário:" e "Record:" no canto inferior esquerdo:
        float x_base = 20;               // Posição fixa no lado esquerdo
        float y_usuario = altura_janela - 180; // Altura inicial para "Usuário:" (ajustado para ficar acima da borda inferior)
        float espacamento = 50;          // Espaço entre as linhas renderizadas:

        // Renderizar o texto "Usuário:"
        al_draw_text(fonte_rendereizacao_usuario, al_map_rgb(255, 255, 255), x_base, y_usuario, ALLEGRO_ALIGN_LEFT, "Usuário:");

        // Renderizar o nome do usuário logo após "Usuário:"
        al_draw_text(fonte_rendereizacao_valores_usuario, al_map_rgb(255, 255, 255), x_base + 140, y_usuario - 5, ALLEGRO_ALIGN_LEFT, nome_usuario);

        // Renderizar o texto "Record:"
        al_draw_text(fonte_rendereizacao_usuario, al_map_rgb(255, 255, 255), x_base, y_usuario + espacamento, ALLEGRO_ALIGN_LEFT, "Record:");

        // Renderizar o valor do record logo após "Record:"
        char record_text[50];
        snprintf(record_text, sizeof(record_text), "%d", record_usuario);
        al_draw_text(fonte_rendereizacao_valores_usuario, al_map_rgb(255, 255, 255), x_base + 140, y_usuario + espacamento, ALLEGRO_ALIGN_LEFT, record_text);

        // Atualizando a tela:
        al_flip_display();

    // Loop principal do menu que espera por uma ação do usuário, seja ela iniciar o game, fechá-lo ou desativar e ativar música:
    bool sair = false;
    bool iniciar_jogo = false;
    while (!sair) {
        // Registrando evento e esperando por um:
        ALLEGRO_EVENT evento;
        al_wait_for_event(fila_eventos, &evento);

        if (evento.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
            sair = true; // Se fechou janela.
        }
        else if (evento.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN) {
            // Verifico se o clique do mouse foi na área em que os botões estão renderizados:
            if (evento.mouse.x >= x_botoes && evento.mouse.x <= x_botoes + largura_botao) {
                // Botão de fechar game:
                if (evento.mouse.y >= y_botao_fechar && evento.mouse.y <= y_botao_fechar + altura_botao) {
                    sair = true; // Fechar programa
                    zerar_record(); // Zerando o record quando o jogo terminar.
                }
                // Botão de iniciar game:
                if (evento.mouse.y >= y_botao_iniciar && evento.mouse.y <= y_botao_iniciar + altura_botao) {
                    iniciar_jogo = true; // Alterna para a tela do jogo
                    sair = true;
                }
            }
        // Verifica se clicou no botão de alternar música:
            if (evento.mouse.x >= x_botao_musica && evento.mouse.x <= x_botao_musica + largura_botao_musica) {
                if (evento.mouse.y >= y_botao_musica && evento.mouse.y <= y_botao_musica + altura_botao_musica) {
                    alternar_musica();
                    desenhar_botao_musica(x_botao_musica, y_botao_musica, largura_botao_musica, altura_botao_musica);
                    al_flip_display();
                }
            }
        }  
    }

    // Ao fechar loop, libero os recursos usados:
    if (menu_de_musica) {
        al_destroy_sample(menu_de_musica);
    }
    if (imagem_fundo) {
        al_destroy_bitmap(imagem_fundo);
    }
    if (imagem_nave) {
        al_destroy_bitmap(imagem_nave);
    }
    if (fonte_geral) {
        al_destroy_font(fonte_geral);
    }
    if (fonte_rendereizacao_usuario) {
        al_destroy_font(fonte_rendereizacao_usuario);
    }
    if (fonte_mensagem_boas_vindas) {
        al_destroy_font(fonte_mensagem_boas_vindas);
    }
    if (display) {
        al_destroy_display(display);
    }
    if (icone_musica_ligada) {
        al_destroy_bitmap(icone_musica_ligada);
    }
    if (icone_musica_desligada) {
        al_destroy_bitmap(icone_musica_desligada);
    }
    if (fonte_rendereizacao_valores_usuario) {
        al_destroy_font(fonte_rendereizacao_valores_usuario);
    }
    // Inicio o game de acordo com a variável de controle:
    if (iniciar_jogo) {
        carregarTelaJogo(); // Alterna para a tela do jogo
        carregarMenuInicial(); // Quando a tela do game fechar, chamo recursivamente a a função atual de tela de menu.
    }

    al_destroy_event_queue(fila_eventos); // Detruo a fila de eventos inicializada.
}
