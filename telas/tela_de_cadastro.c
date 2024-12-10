#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_primitives.h>
#include <string.h>
#include <stdio.h>

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600

void save_username(const char *username) {
    FILE *file = fopen("./extras/username.txt", "w");
    if (file) {
        fprintf(file, "%s", username);
        fclose(file);
    }
}

int iniciarTelaDeCadastro() {
    
    // Inicializações principais
    if (!al_init()) {
        fprintf(stderr, "Falha ao inicializar o Allegro.\n");
        return -1;
    }

    // Inicializações adicionais:
    al_init_font_addon();
    al_init_ttf_addon();
    al_init_primitives_addon();
    al_install_keyboard();

    // Configurações iniciais
    ALLEGRO_DISPLAY *display = al_create_display(SCREEN_WIDTH, SCREEN_HEIGHT);
    if (!display) {
        fprintf(stderr, "Falha ao criar o display.\n");
        return -1;
    }
    al_set_window_title(display, "Cadastro de Username");

    // Criando a fila de eventos e definindo a fonte:
    ALLEGRO_EVENT_QUEUE *event_queue = al_create_event_queue();
    ALLEGRO_FONT *font = al_load_ttf_font("./fonts/roboto/Roboto-Regular.ttf", 32, 0);

    // Verificando inicalização da fonte e da fila de eventos:
    if (!event_queue || !font) {
        fprintf(stderr, "Falha ao carregar recursos.\n");
        return -1;
    }

    // Registrando eventos:
    al_register_event_source(event_queue, al_get_keyboard_event_source());
    al_register_event_source(event_queue, al_get_display_event_source(display));

    // Variáveis de controle
    char username[50] = "";
    int username_length = 0;
    bool running = true;
    bool enter_pressed = false;

    while (running) {
        ALLEGRO_EVENT event;
        al_wait_for_event(event_queue, &event);

        if (event.type == ALLEGRO_EVENT_KEY_CHAR) {
            // Entrada de texto
            if (event.keyboard.unichar >= 32 && event.keyboard.unichar <= 126 && username_length < 49) {
                username[username_length++] = (char)event.keyboard.unichar;
                username[username_length] = '\0';
            }
            // Apagar caractere
            else if (event.keyboard.keycode == ALLEGRO_KEY_BACKSPACE && username_length > 0) {
                username[--username_length] = '\0';
            }
            // Confirmar (Enter)
            else if (event.keyboard.keycode == ALLEGRO_KEY_ENTER && username_length > 0) {
                enter_pressed = true;
            }
        } else if (event.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
            running = false;
        }

        // Lógica para sair após Enter
        if (enter_pressed) {
            save_username(username); // Salva o username
            printf("Username salvo: %s\n", username); // Feedback no terminal
            running = false; // Encerra o loop da tela de login
        }

        // Renderização
        al_clear_to_color(al_map_rgb(200, 200, 200)); 

        al_draw_text(font, al_map_rgb(10, 10, 10), SCREEN_WIDTH / 2, SCREEN_HEIGHT / 4,
                     ALLEGRO_ALIGN_CENTER, "Insira um username:");
        al_draw_rectangle(SCREEN_WIDTH / 4, SCREEN_HEIGHT / 2 - 20, SCREEN_WIDTH * 3 / 4, SCREEN_HEIGHT / 2 + 20,
                          al_map_rgb(0, 0, 0), 2);
        al_draw_text(font, al_map_rgb(10, 10, 10), SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 - 16,
                     ALLEGRO_ALIGN_CENTER, username);

        al_flip_display();
    }

    // Liberação de recursos:
    al_destroy_font(font);
    al_destroy_event_queue(event_queue);
    al_destroy_display(display);

    return 0;
}
