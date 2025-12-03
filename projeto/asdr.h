#ifndef ASDR_H
#define ASDR_H

#include "analex.h" /* Necessário para conhecer TipoAtomo */

/*///////////////////////
    VARIÁVEIS GLOBAIS
      COMPARTILHADAS
 ///////////////////////*/
 extern TInfoAtomo lookahead;

/*///////////////////////
        PROTÓTIPOS
 ///////////////////////*/
void parse_ini(void);
void parse_dcl(void);
void parse_tipo(void);
void parse_bco(void);
void parse_cmd(void);
void parse_wr(void);
void parse_rd(void);
void parse_if(void);
void parse_fr(void);
void parse_wh(void);
void parse_rpt(void);
void parse_atr(void);
void parse_ret(void);

/* Funções que agora retornam TipoAtomo para validação semântica */
TipoAtomo parse_e(void);
TipoAtomo parse_exp(void);
TipoAtomo parse_exps(void);
TipoAtomo parse_tmo(void);
TipoAtomo parse_ftr(void);

void parse_subrot(void);
void parse_subroutine(void);

/* Funções utilitárias */
void consumir(TipoAtomo esperado);
void erro_sintatico(const char *msg);
void erro_semantico(const char *msg); /* Adicionado também */

#endif