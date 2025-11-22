/* *******************************************************************************
 * Matheus de Andrade Lourenço - 1041691
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
    if (!filename) return;
    out = fopen(filename, "w");
    if (!out) {
        perror("erro ao abrir arquivo .mepa");
        out = NULL;
    }
}

void gera_close(void) {
    if (out) fclose(out);
    out = NULL;
}

void gera_instr_mepa(char *rotulo, char *mnemonico, char *param1, char *param2) {
    FILE *f = out ? out : stdout;
    if (rotulo) fprintf(f, "%s:\t", rotulo);
    else fprintf(f, "\t");
    if (mnemonico) fprintf(f, "%s", mnemonico);
    if (param1) fprintf(f, " %s", param1);
    if (param2) fprintf(f, " %s", param2);
    fprintf(f, "\n");
}

char* novo_rotulo(void) {
    char *b = (char*) malloc(32);
    snprintf(b, 32, "L%d", rotulo_count++);
    return b;
}
