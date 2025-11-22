/* *******************************************************************************
 * Matheus de Andrade Lourenço - 1041691
 * Murillo Cardoso Ferreira    - 10418082
 *
 * tabsimb.c
 * Implementação da tabela de símbolos (pilha de escopos)
 *******************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tabsimb.h"

typedef struct Scope {
    RegistroTS *symbols;
    struct Scope *next;
    int alloc_base; /* base de endereço ao entrar no escopo */
} Scope;

static Scope *scope_stack = NULL;
static int next_address = 0;

void ts_init(void) {
    scope_stack = NULL;
    ts_enter_scope();
}

void ts_enter_scope(void) {
    Scope *s = (Scope*) malloc(sizeof(Scope));
    s->symbols = NULL;
    s->next = scope_stack;
    s->alloc_base = next_address;
    scope_stack = s;
}

void ts_exit_scope(void) {
    if (!scope_stack) return;
    RegistroTS *r = scope_stack->symbols;
    while (r) {
        RegistroTS *tmp = r;
        r = r->next;
        free(tmp);
    }
    Scope *tmpS = scope_stack;
    scope_stack = scope_stack->next;
    free(tmpS);
}

static RegistroTS *find_in_scope(Scope *s, const char *lexema) {
    RegistroTS *p = s->symbols;
    while (p) {
        if (strcmp(p->lexema, lexema) == 0) return p;
        p = p->next;
    }
    return NULL;
}

RegistroTS* ts_buscar(char *lexema) {
    for (Scope *s = scope_stack; s; s = s->next) {
        RegistroTS *r = find_in_scope(s, lexema);
        if (r) return r;
    }
    return NULL;
}

static void insert_symbol(RegistroTS *r) {
    if (!scope_stack) return;
    r->next = scope_stack->symbols;
    scope_stack->symbols = r;
}

RegistroTS* ts_inserir(char *lexema, Categoria cat, TipoAtomo tipo, int endereco) {
    (void)endereco; /* parâmetro atualmente não utilizado */
    if (!scope_stack) return NULL;
    if (find_in_scope(scope_stack, lexema)) {
        fprintf(stderr, "Erro semântico: identificador '%s' já declarado no mesmo escopo\n", lexema);
        exit(1);
    }
    RegistroTS *r = (RegistroTS*) malloc(sizeof(RegistroTS));
    strncpy(r->lexema, lexema, sizeof(r->lexema)-1);
    r->lexema[sizeof(r->lexema)-1] = '\0';
    r->cat = cat;
    r->tipo = tipo;
    /* atribuição automática de endereço para variáveis/parâmetros */
    if (cat == CAT_VAR || cat == CAT_PARAM) {
        r->endereco = next_address++;
    } else {
        r->endereco = -1;
    }
    r->arr_size = 0;
    r->param_count = 0;
    r->next = NULL;
    insert_symbol(r);
    return r;
}

int ts_locals_count_current_scope(void) {
    if (!scope_stack) return 0;
    return next_address - scope_stack->alloc_base;
}

void ts_write_file(const char *filename) {
    if (!filename) return;
    FILE *f = fopen(filename, "w");
    if (!f) return;
    int depth = 0;
    for (Scope *s = scope_stack; s; s = s->next) depth++;
    Scope **arr = (Scope**) malloc(sizeof(Scope*) * (depth>0?depth:1));
    int i=0; for (Scope *s = scope_stack; s; s = s->next) arr[i++] = s;
    fprintf(f, "Tabela de Simbolos\n\n");
    for (int d = i-1; d >= 0; d--) {
        fprintf(f, "-- Escopo %d --\n", (i-1)-d);
        RegistroTS *sym = arr[d]->symbols;
        int n=0; for (RegistroTS *p=sym;p;p=p->next) n++;
        if (n==0) { fprintf(f, "(vazio)\n"); continue; }
        RegistroTS **sarr = (RegistroTS**) malloc(sizeof(RegistroTS*)*n);
        int idx=0; for (RegistroTS *p=sym;p;p=p->next) sarr[idx++]=p;
        for (int k=n-1;k>=0;k--) {
            RegistroTS *s = sarr[k];
            fprintf(f, "nome=%s kind=%s type=", s->lexema,
                s->cat==CAT_VAR?"VAR":(s->cat==CAT_FUNC?"FUNC":"PARAM") );
            switch(s->tipo) {
                case sINT: fprintf(f, "int"); break;
                case sFLOAT: fprintf(f, "float"); break;
                case sCHAR: fprintf(f, "char"); break;
                case sVOID: fprintf(f, "void"); break;
                default: fprintf(f, "unknown"); break;
            }
            if (s->arr_size>0) fprintf(f, "[%d]", s->arr_size);
            if (s->cat==CAT_FUNC) fprintf(f, " params=%d", s->param_count);
            fprintf(f, "\n");
        }
        free(sarr);
    }
    free(arr);
    fclose(f);
}
