# Space Battle - Jogo 

### Olá, esse é um jogo do tipo 'space battle' desenvolvido em linguagem C com a [allegro 5](https://liballeg.org/)

## Descarregando repositório:

### 1 - Para clonar o repositório em sua máquina é nescessário primeiramente que você tenha o Git instalado em sua máquina. Segue o passo a passo:
-  Instale o git em sua máquina -> [link para baixar](https://git-scm.com/downloads). Após instalar, em um terminal certifique-se de que está funcionando corretamente com `git --version`.

- Em um terminal, nave até a pasta onde você deseja clonar. Ex: `luisfl@luisfl-NCL60-61:~$ cd Documentos/prog/`.

- Na pasta, use `git clone https://github.com/luisfl04/space_battle.git` ou `git clone git@github.com:luisfl04/space_battle.git` se você usa autenticação SSH.

- Após clonar, use `cd ./space_battle` para navegar até o repositório. Com isso, você já tem o repositório em sua máquina.

## Como Compilar o Jogo:

### 1 - Tenha a biblioteca allegro5 em sua máquina:

#### Em um terminal linux(Ubuntu/Debian) use -> `sudo apt-get update && sudo apt-get install liballegro5-dev`
#### Em um sistema windows -> [Acesse este link](https://www.nuget.org/api/v2/package/Allegro/5.2.10) para baixar.

### 2 - Tenha o compilador **GCC** para fazer a compilação do código fonte:
#### Acesse [este artigo](https://www.alura.com.br/artigos/compilando-executando-programas-c-c-windows?srsltid=AfmBOooPrL_e2Opiroz8XDEuVCR5q2KAPc_9m9ZCqYVUj90G45Orlp6F) com intruções de como instalá-lo.

### 3 - No repósitorio raiz do game, use o comando abaixo para compilar o jogo:
`gcc -o SpaceBattle index.c ./telas/game.c ./telas/menu_inicial.c -lallegro -lallegro_font -lallegro_ttf -lallegro_primitives -lallegro_audio -lallegro_acodec -lallegro_dialog -lallegro_image -lm `

### 3 - Um execultável com o nome `SpaceBattle` será criado. Use `./SpaceBattle` para execultar o programa no seu terminal ou acesse com dois cliques no explorador de arquivos do seu sistema operacional.

## Casos de Uso:
### 1 - Abaixo está o diagrama de caso de uso que representa o funcionamento geral do jogo:
![caso_de_uso](./artefatos/imagens/SpaceBattle_caso_de_uso.jpg)

## Funcionamento Geral do Jogo:
### 1 - Este é um jogo onde o jogador controla uma nave e deve evitar ser atingido pelos inimigos que aparecem no jogo. A dificuldade aumenta progressivamente conforme o tempo, com mais inimigos surgindo e movendo-se mais rápido.

### 2 - Teclas usadas no game:
- As setas do teclado são usadas para a movimentação da nave.
- A teclas `P` faz o jogo pausar e iniciar novamente
- A tecla `espaço` do teclado é usada para acionar os tiros da nave
- No game, ao teclar `esc` o usuário é redirecionado ao menu inicial.

## Funcionalidades:
### 1 - Menu Inicial:

- Botão para iniciar o jogo.

- Botão para fechar o jogo.

- Controle para ativar ou desativar a música de fundo.

- Visualização do nome do usuário e seu recorde atual.

![screenshot menu inicial](./artefatos/imagens/tela_de_menu.png)

### 2 - Tela do Jogo:

- Controle de nave utilizando o teclado.

- Sistema de pontuação que aumenta com o tempo e ao derrotar inimigos.

- Inimigos que seguem a nave do jogador, aumentando em quantidade e velocidade com o tempo.

- Mensagem de “Game Over” quando a nave é atingida, retornando ao menu inicial.

- Renderização de animação quando a nave é atingida.

- Sons interativos para os tiros da nave e para inimigos atingidos

- Implementação de menu de pause

![tela do game](./artefatos/imagens/game.png)

## Organização do respositório:
### 1 - Abaixo segue as pastas do repositório e seus respectivos contextos:
- `artefatos/`: Possui todos os arquivos auxiliares do repositório, como o arquivo 
`usuario.txt` que é usado para armazenar o username inserido pelo usuário de forma dinâmica.
- `audio/`: Contém todos os arquivos de assets de áudios usados no game, seja as músicas ou áudios de efeito.
- `fonts/`: Armazena arquivos de fontes usadas com a extensão`.ttf`
- `imagens/`: Todos os arquivos de imagens usadas no game.
- `telas/`: Arquivos em linguagem C que implementam o jogo em sí.
- `.gitignore`: Arquivo que lista as pastas ou arquivos que o git não deve rastrear
- `index.c`: Arquivo principal que contém a função `"main"` que é chamada no momento da compilação.

## Considerações gerais:

### 1 - Fique a vontade para abrir issues ou sugerir melhorias!
### 2 - Tecnologias usadas: 
- [Linguagem C](https://pt.wikipedia.org/wiki/C_(linguagem_de_programa%C3%A7%C3%A3o))
- [Compilador gcc](https://gcc.gnu.org/)
- [Allegro 5](https://liballeg.org/)
- link dos assets usados -> [acesse aqui](https://opengameart.org/content/space-game-assets) 

## Minhas redes sociais:
[![instagram](https://img.shields.io/badge/-Instagram-000?style=for-the-badge&logo=instagram&logoColor=FFF&color:FFF)](https://www.instagram.com/luisfl04_/)
[![Gmail](https://img.shields.io/badge/Gmail-D14836?style=for-the-badge&logo=gmail&logoColor=white)](mailto:luisfelipecontato08@gmail.com)
[![LinkedIn](https://img.shields.io/badge/LinkedIn-Profile-blue?style=flat-square&logo=linkedin)](https://www.linkedin.com/in/luis-felipe-8725a1291/)


