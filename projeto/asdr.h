
#ifndef ASDR_H
#define ASDR_H

/*///////////////////////
    VARIÁVEIS GLOBAIS
      COMPARTILHADAS
 ///////////////////////*/
 /* lookahead: token corrente lido pelo analisador léxico. Todas as
  * rotinas de parsing consultam / consomem esse token durante a
  * análise sintática. */
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
void parse_e(void);
void parse_exp(void);
void parse_exps(void);
void parse_tmo(void);
void parse_ftr(void);
void parse_subrot(void);
void parse_subroutine(void);

/* Funções utilitárias */
void consumir(TipoAtomo esperado);
void erro_sintatico(const char *msg);

#endif