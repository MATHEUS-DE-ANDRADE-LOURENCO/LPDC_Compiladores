/* *******************************************************************************
 * Matheus de Andrade Lourenço - 10419691
 * Murillo Cardoso Ferreira    - 10418082
 *
 * tabsimb.h
 * Interface da tabela de símbolos (conforme roteiro do professor)
 *******************************************************************************/
#ifndef TABSIMB_H
#define TABSIMB_H

#include "analex.h"

#define MAX_PARAMS 64
typedef enum { CAT_VAR, CAT_FUNC, CAT_PARAM } Categoria;

typedef struct RegistroTS {
    char lexema[128];
    Categoria cat;
    TipoAtomo tipo;       /* Tipo da variável ou retorno da função */
    int endereco;         /* Endereço relativo na pilha */
    int arr_size;         /* Tamanho se for vetor (0 = simples) */
    int param_count;      /* Quantidade de parâmetros (se for função) */
    TipoAtomo param_types[MAX_PARAMS]; /* Lista de tipos dos parâmetros */
    struct RegistroTS *next;
} RegistroTS;

/* Inicialização da tabela */
/* ts_init: prepara a estrutura de tabela de símbolos e cria o
 * escopo global inicial. Deve ser chamada antes da análise. */
void ts_init(void);
/* Gerenciamento de escopos */
void ts_enter_scope(void);
void ts_exit_scope(void);
/* número de variáveis/alocações no escopo atual (para AMEM) */
int ts_locals_count_current_scope(void);
/* Inserção/Busca */
/* ts_inserir: insere um novo identificador no escopo atual. Retorna
 * o registro criado (ou NULL em caso de erro). */
RegistroTS* ts_inserir(char *lexema, Categoria cat, TipoAtomo tipo, int endereco);
/* ts_buscar: procura um identificador percorrendo a pilha de escopos
 * (do escopo mais interno para o mais externo). Retorna o registro
 * encontrado ou NULL caso não exista. */
RegistroTS* ts_buscar(char *lexema);
/* Exportar tabela */
void ts_write_file(const char *filename);

#endif
