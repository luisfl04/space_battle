Space Battle - Jogo

Bem-vindo ao Space Battle, um jogo de nave onde o objetivo é sobreviver ao máximo enquanto enfrenta ondas crescentes de inimigos!

Como Compilar o Jogo

Use o comando abaixo para compilar o jogo:

gcc -o index index.c ./telas/game.c -lallegro -lallegro_font -lallegro_ttf -lallegro_primitives -lallegro_audio -lallegro_acodec -lallegro_dialog -lallegro_image -lm

Certifique-se de que você possui todas as dependências da biblioteca Allegro instaladas no seu sistema.

Diagrama de Caso de Uso

Abaixo está o diagrama de caso de uso que representa o funcionamento geral do jogo:

[]



Funcionamento Geral do Jogo

Space Battle é um jogo onde o jogador controla uma nave e deve evitar ser atingido pelos inimigos que aparecem no jogo. A dificuldade aumenta progressivamente conforme o tempo, com mais inimigos surgindo e movendo-se mais rápido.

Funcionalidades:

Menu Inicial:

Botão para iniciar o jogo.

Botão para fechar o jogo.

Controle para ativar ou desativar a música de fundo.

Visualização do nome do usuário e seu recorde atual.



Tela do Jogo:

Controle de nave utilizando o teclado.

Sistema de pontuação que aumenta com o tempo e ao derrotar inimigos.

Inimigos que seguem a nave do jogador, aumentando em quantidade e velocidade com o tempo.

Mensagem de “Game Over” quando a nave é atingida, retornando ao menu inicial.



