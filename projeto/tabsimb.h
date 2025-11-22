/* *******************************************************************************
 * Matheus de Andrade Lourenço - 1041691
 * Murillo Cardoso Ferreira    - 10418082
 *
 * tabsimb.h
 * Interface da tabela de símbolos (conforme roteiro do professor)
 *******************************************************************************/
#ifndef TABSIMB_H
#define TABSIMB_H

#include "analex.h"

typedef enum { CAT_VAR, CAT_FUNC, CAT_PARAM } Categoria;

typedef struct RegistroTS {
    char lexema[128];
    Categoria cat;
    TipoAtomo tipo; /* usa TipoAtomo de analex.h */
    int endereco; /* não usado por enquanto */
    int arr_size; /* 0 se não for vetor */
    int param_count; /* para funções */
    struct RegistroTS *next;
} RegistroTS;

/* Inicialização da tabela */
void ts_init(void);
/* Gerenciamento de escopos */
void ts_enter_scope(void);
void ts_exit_scope(void);
/* número de variáveis/alocações no escopo atual (para AMEM) */
int ts_locals_count_current_scope(void);
/* Inserção/Busca */
RegistroTS* ts_inserir(char *lexema, Categoria cat, TipoAtomo tipo, int endereco);
RegistroTS* ts_buscar(char *lexema);
/* Exportar tabela */
void ts_write_file(const char *filename);

#endif
