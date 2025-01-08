#include <stdio.h>
#include <stdlib.h>
#include "telas/tela_de_cadastro.c" 

int main(int argc, char **argv){
    
    /* Só preciso iniciar a tela de obtenção do username do jogador, todo o resto do fluxo do game é encarregado as 
    funções subsequentes */
    iniciarTelaDeCadastro();
    return 0;
}
