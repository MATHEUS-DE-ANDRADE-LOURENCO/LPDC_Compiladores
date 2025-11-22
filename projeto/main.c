#/*******************************************************************************
 * Matheus de Andrade Lourenço - 1041691
 * Murillo Cardoso Ferreira    - 10418082
 *
 * main.c
 * Ponto de entrada do compilador LPD
 ******************************************************************************/
#include <stdio.h>
#include "analex.h"
#include "asdr.h"
#include "tabsimb.h"
#include <string.h>
#include "gerador.h"

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Uso: %s <arquivo fonte>\n", argv[0]);
        return 1;
    }

    fonte = fopen(argv[1], "r");
    if (!fonte) {
        perror("Erro ao abrir arquivo");
        return 1;
    }

    // Bootstrap: carrega o primeiro token
    lookahead = obter_atomo(); 
    
    // Inicializa tabela de símbolos
    ts_init();

    /* inicializa emissor MEPA (.mepa) antes da análise */
    char mepaname[1024];
    strncpy(mepaname, argv[1], sizeof(mepaname)-1);
    mepaname[sizeof(mepaname)-1] = '\0';
    char *dot2 = strrchr(mepaname, '.');
    if (dot2) strcpy(dot2, ".mepa"); else strcat(mepaname, ".mepa");
    gera_init(mepaname);

    // Inicia a análise sintática
    parse_ini();

    // Se o último token é EOF, sucesso!
    if (lookahead.tipo == sEOF) {
        /* gerar arquivo .tsim para a execução atual */
        char outname[1024];
        strncpy(outname, argv[1], sizeof(outname)-1);
        outname[sizeof(outname)-1] = '\0';
        /* substituir extensão .lpd por .tsim, se presente */
        char *dot = strrchr(outname, '.');
        if (dot) strcpy(dot, ".tsim"); else strcat(outname, ".tsim");
        ts_write_file(outname);
        /* finalizar emissor MEPA */
        gera_close();
        printf("Código compilado com sucesso!\n");
    }
    
    fclose(fonte);
    return 0;
}
