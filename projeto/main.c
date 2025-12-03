#/*******************************************************************************
 * Matheus de Andrade Lourenço - 10419691
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
#include <sys/stat.h>
#include <errno.h>

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

    /*
     * Criar diretórios de saída dentro de `projeto/` se ainda não existirem.
     * Usamos permissões 0755 para que o usuário do workspace possa ler/escrever.
     * Não interrompemos a execução se o diretório já existir.
     */
    mkdir("projeto/mepa", 0755);
    if (errno != 0 && errno != EEXIST) { errno = 0; }
    mkdir("projeto/tab_simbs", 0755);
    if (errno != 0 && errno != EEXIST) { errno = 0; }

    /*
     * Inicializa o emissor MEPA: calculamos o nome base do arquivo de
     * entrada (sem caminho) e criamos o arquivo `projeto/mepa/<base>.mepa`.
     * `gera_init` abre o arquivo para escrita; o gerador emite instruções
     * para o arquivo aberto durante a análise.
     */
    char mepaname[2048];
    const char *input = argv[1];
    const char *base = input;
    const char *slash = strrchr(input, '/');
    if (slash) base = slash + 1;
    strncpy(mepaname, base, sizeof(mepaname)-1);
    mepaname[sizeof(mepaname)-1] = '\0';
    char *dot2 = strrchr(mepaname, '.');
    if (dot2) strcpy(dot2, ".mepa"); else strcat(mepaname, ".mepa");
    char mepa_path[4096];
    snprintf(mepa_path, sizeof(mepa_path), "projeto/mepa/%s", mepaname);
    gera_init(mepa_path);

    // Inicia a análise sintática
    parse_ini();

    // Se o último token é EOF, sucesso!
    if (lookahead.tipo == sEOF) {
        /* gerar arquivo .tsim para a execução atual dentro de tab_simbs/ */
        char outname[2048];
        const char *input2 = argv[1];
        const char *base2 = input2;
        const char *slash2 = strrchr(input2, '/');
        if (slash2) base2 = slash2 + 1;
        strncpy(outname, base2, sizeof(outname)-1);
        outname[sizeof(outname)-1] = '\0';
        /* substituir extensão .lpd por .tsim, se presente */
        char *dot = strrchr(outname, '.');
        if (dot) strcpy(dot, ".tsim"); else strcat(outname, ".tsim");
        char tsim_path[4096];
        snprintf(tsim_path, sizeof(tsim_path), "projeto/tab_simbs/%s", outname);
        ts_write_file(tsim_path);
        /* finalizar emissor MEPA */
        gera_close();
        printf("Código compilado com sucesso!\n");
    }
    
    fclose(fonte);
    return 0;
}
