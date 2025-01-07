#include <stdio.h>
#include <stdlib.h>
#include "telas/tela_de_cadastro.c" 
#include "telas/menu_inicial.c"

int main(int argc, char **argv){
    
    // Iniciando tela que obtém o username do jogador:
    iniciarTelaDeCadastro();

    // Ao obter o username, incio o menu inicial:
    carregarMenuInicial();

    return 0;
}
