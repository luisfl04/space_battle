// Este arquivo renderiza tda a regra de negócio da tela de cadastro de username:

// Libs usadas:
#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_primitives.h>
#include <string.h>
#include <stdio.h>
#include <allegro5/allegro_native_dialog.h>
#include <allegro5/allegro_image.h>

// Funções de outros arquivos usadas: 
extern void carregarMenuInicial();

// Constantes:
#define SCREEN_WIDTH 1366
#define SCREEN_HEIGHT 768

// Função que escreva o username obtido no arquivo:
void salvarUsername(const char *username) {
    FILE *file = fopen("./artefatos/username.txt", "w");
    if (file) {
        fprintf(file, "%s", username);
        fclose(file);
    }
}

// Função principal da tela:
int iniciarTelaDeCadastro() {
    
    // Inicializando allegro:
    if (!al_init()) {
        fprintf(stderr, "Falha ao inicializar o Allegro.\n");
        return -1;
    }

    // Inicializações adicionais:
    al_init_font_addon();
    al_init_ttf_addon();
    al_init_primitives_addon();
    al_install_keyboard();
    al_init_image_addon();

    // Criando display da tela:
    ALLEGRO_DISPLAY *display = al_create_display(SCREEN_WIDTH, SCREEN_HEIGHT);
    if (!display) {
        fprintf(stderr, "Falha ao criar o display.\n");
        return -1;
    }
    al_set_window_title(display, "Cadastro de Username"); // Titulo da janela.

    // Criando a fila de eventos e definindo a fonte:
    ALLEGRO_EVENT_QUEUE *event_queue = al_create_event_queue();
    ALLEGRO_FONT *font = al_load_ttf_font("./fonts/roboto/Roboto-Bold.ttf", 48, 0);

    // Verificando inicalização da fonte e da fila de eventos:
    if (!event_queue || !font) {
        fprintf(stderr, "Falha ao carregar recursos.\n");
        return -1;
    }

    // Carregando imagem de fundo da tela:
    ALLEGRO_BITMAP *imagem_fundo = al_load_bitmap("./imagens/fundo.png");
    if (!imagem_fundo) {
        al_show_native_message_box(NULL, "Erro", "Não foi possível carregar a imagem de fundo", "", NULL, ALLEGRO_MESSAGEBOX_ERROR);
    }

    // Registrando eventos que serão mapeados:
    al_register_event_source(event_queue, al_get_keyboard_event_source());
    al_register_event_source(event_queue, al_get_display_event_source(display));

    // Variáveis de controle do loop:
    char username[50] = "";
    int username_length = 0;
    bool running = true;
    bool enter_pressed = false;
    bool display_fechado = false;

    // Loop principal do arquivo:
    while (running) {
        ALLEGRO_EVENT event;
        al_wait_for_event(event_queue, &event);

        if (event.type == ALLEGRO_EVENT_KEY_CHAR) {
            // Entrada de texto:
            if (event.keyboard.unichar >= 32 && event.keyboard.unichar <= 126 && username_length < 49) {
                username[username_length++] = (char)event.keyboard.unichar;
                username[username_length] = '\0';
            }
            // Apagar caractere:
            else if (event.keyboard.keycode == ALLEGRO_KEY_BACKSPACE && username_length > 0) {
                username[--username_length] = '\0';
            }
            // Confirmar (Enter):
            else if (event.keyboard.keycode == ALLEGRO_KEY_ENTER && username_length > 0) {
                enter_pressed = true;
            }
        } else if (event.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
            running = false;
            display_fechado = true;
        } // Casos onde o usuário fecha a tela.

        // Ao sair do loop, salvo o username fornecido.
        if (enter_pressed) {
            salvarUsername(username); // Salva o username
            printf("Username salvo: %s\n", username); // Feedback no terminal
            running = false; // Encerra o loop da tela de login
        }

        // Renderização da tela:
        al_clear_to_color(al_map_rgb(0, 0, 0)); // cor de fundo
        al_draw_bitmap(imagem_fundo, 0, 0, 0); // imagem de fundo

        // Renderização do texto:
        al_draw_text(font, al_map_rgb(200, 200, 200), SCREEN_WIDTH / 2, SCREEN_HEIGHT / 4,
                     ALLEGRO_ALIGN_CENTER, "Insira um username:");
        al_draw_rectangle(SCREEN_WIDTH / 4, SCREEN_HEIGHT / 2 - 30, SCREEN_WIDTH * 3 / 4, SCREEN_HEIGHT / 2 + 36,
                          al_map_rgb(200, 200, 200), 2);
        al_draw_text(font, al_map_rgb(200, 200, 200), SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 - 16,
                     ALLEGRO_ALIGN_CENTER, username);

        al_flip_display(); // Atualização da página a cada loop.
    }

    // Liberação de recursos usados:
    al_destroy_font(font);
    al_destroy_event_queue(event_queue);
    al_destroy_display(display);
    al_destroy_bitmap(imagem_fundo);

    // Verifico se o usuário fechou a tela deliberadamente:
    if(display_fechado == false){
        carregarMenuInicial();
    }
    else{
        return 0;
    }

    return 0;
}
