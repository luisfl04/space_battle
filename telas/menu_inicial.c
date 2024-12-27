#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>
#include <allegro5/allegro_native_dialog.h>

// Configurações básicas de inicialização da tela:
static ALLEGRO_DISPLAY *display = NULL;
static ALLEGRO_FONT *fonte = NULL;
static ALLEGRO_SAMPLE *menu_de_musica = NULL;
static bool musica_tocando = true;
static int largura_janela = 800;
static int altura_janela = 600;


static void carregarMenuInicial() {

    /* Essa função é responsável por carregar o menu inicial do jogo que representa também o menu de pause. O usuário irá usá-la sempre que 
    o game for pausado ou quando o game for iniciado pela primeira vez */

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

    // Inicializa addons necessários:
    al_init_font_addon();
    al_init_ttf_addon();
    al_init_primitives_addon();
    al_install_audio();
    al_init_acodec_addon();

    // Carregando fonte:
    fonte = al_load_ttf_font("./fonts/roboto/Roboto-Regular.ttf", 32, 0);
    if (!fonte) {
        al_show_native_message_box(NULL, "Erro", "Não foi possível carregar a fonte", "", NULL, ALLEGRO_MESSAGEBOX_ERROR);
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

    // Prepara o fundo
    al_clear_to_color(al_map_rgb(0, 0, 0));

    // Desenha o texto centralizado
    const char *menuText = "Bem-vindo ao Jogo!";
    int textWidth = al_get_text_width(fonte, menuText);
    int textHeight = al_get_font_line_height(fonte);
    al_draw_text(fonte, al_map_rgb(255, 255, 255), largura_janela / 2 - textWidth / 2, altura_janela / 2 - textHeight / 2, 0, menuText);

    // Atualiza a tela
    al_flip_display();
    al_rest(14.0);

    // Liberando recursos usados:
    if (menu_de_musica) {
        al_destroy_sample(menu_de_musica);
    }
    if (fonte) {
        al_destroy_font(fonte);
    }
    if (display) {
        al_destroy_display(display);
    }

}