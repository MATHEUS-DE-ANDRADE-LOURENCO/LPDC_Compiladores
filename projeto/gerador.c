/* *******************************************************************************
 * Matheus de Andrade Lourenço - 10419691
 * Murillo Cardoso Ferreira    - 10418082
 *
 * gerador.c
 * Implementação mínima (stubs) do gerador MEPA
 *******************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "gerador.h"

static int rotulo_count = 0;
static FILE *out = NULL;

void gera_init(const char *filename) {
    /*
     * Inicializa o emissor MEPA apontando `out` para o arquivo dado
     * pelo caminho `filename`. Se `filename` for NULL ou a abertura
     * falhar, `out` permanecerá NULL e as instruções poderão ser
     * direcionadas para stdout por fallback.
     */
    if (!filename) return;
    out = fopen(filename, "w");
    if (!out) {
        perror("erro ao abrir arquivo .mepa");
        out = NULL;
    }
}

void gera_close(void) {
    /* Fecha o arquivo MEPA se aberto e reseta o ponteiro `out`. */
    if (out) fclose(out);
    out = NULL;
}

void gera_instr_mepa(char *rotulo, char *mnemonico, char *param1, char *param2) {
    /*
     * Emite uma instrução MEPA formatada para o arquivo `out` (ou
     * stdout se `out` for NULL). O formato segue:
     *   [<rotulo>:]\t<mnemonico> [param1] [param2]\n
     */
    FILE *f = out ? out : stdout;
    if (rotulo) fprintf(f, "%s:\t", rotulo);
    else fprintf(f, "\t");
    if (mnemonico) fprintf(f, "%s", mnemonico);
    if (param1) fprintf(f, " %s", param1);
    if (param2) fprintf(f, " %s", param2);
    fprintf(f, "\n");
}

char* novo_rotulo(void) {
    /*
     * Gera um novo rótulo de forma incremental (alocado dinamicamente).
     * O chamador é responsável por liberar a string retornada.
     */
    char *b = (char*) malloc(32);
    snprintf(b, 32, "L%d", rotulo_count++);
    return b;
}
