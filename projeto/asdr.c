/* *******************************************************************************
 * Matheus de Andrade Lourenço - 10419691
 * Murillo Cardoso Ferreira    - 10418082
 *
 * asdr.c
 * Analisador Sintático + Semântico (Versão Final: Validação de Retorno e Índices Negativos)
 ******************************************************************************/
#include <stdio.h>
#include "analex.h"
#include "asdr.h"
#include <stdlib.h>
#include <string.h>
#include "tabsimb.h"
#include "gerador.h"

TInfoAtomo lookahead;
static TipoAtomo current_func_type = sVOID;
static int func_has_return = 0; /* Flag para verificar se houve return */

/* Protótipos */
TipoAtomo parse_exp(void);
TipoAtomo parse_exps(void);
TipoAtomo parse_tmo(void);
TipoAtomo parse_ftr(void);
TipoAtomo parse_e(void);

/* --- Utilitários --- */

static const char *nome_token(TipoAtomo t) {
    switch(t) {
        case sPRG: return "prg";
        case sVAR: return "var";
        case sSUBROT: return "subrot";
        case sRETURN: return "return";
        case sINT: return "int";
        case sFLOAT: return "float";
        case sCHAR: return "char";
        case sVOID: return "void";
        case sBEGIN: return "begin";
        case sEND: return "end";
        case sWRITE: return "write";
        case sREAD: return "read";
        case sIF: return "if";
        case sTHEN: return "then";
        case sELSE: return "else";
        case sFOR: return "for";
        case sWHILE: return "while";
        case sREPEAT: return "repeat";
        case sUNTIL: return "until";
        case sAND: return "and";
        case sOR: return "or";
        case sNOT: return "not";
        case sIDENT: return "identificador";
        case sNUM_INT: return "inteiro";
        case sNUM_REAL: return "real";
        case sCARACTER: return "char";
        case sSTRING: return "string";
        case sATRIB: return "<-";
        case sSOMA: return "+";
        case sSUBTR: return "-";
        case sMULT: return "*";
        case sDIV: return "/";
        case sMAIOR: return ">";
        case sMAIOR_IGUAL: return ">=";
        case sMENOR: return "<";
        case sMENOR_IGUAL: return "<=";
        case sIGUAL: return "==";
        case sDIFERENTE: return "!=";
        case sABRE_PARENT: return "(";
        case sFECHA_PARENT: return ")";
        case sABRE_COLCH: return "[";
        case sFECHA_COLCH: return "]";
        case sPONTO: return ".";
        case sVIRG: return ",";
        case sPONTO_VIRG: return ";";
        case sEOF: return "EOF";
        case sERRO_LEXICO: return "ERRO_LEXICO";
        default: return "<token desconhecido>";
    }
}

TInfoAtomo safe_obter_atomo(void) {
    TInfoAtomo t = obter_atomo();
    if (t.tipo == sERRO_LEXICO) {
        /* Ignora erros léxicos óbvios de comentários mal formados (--) no meio do código se desejado,
           mas para rigor, aborta. */
        fprintf(stderr, "Erro Léxico na linha %d: caractere inválido ou string não terminada.\n", t.linha);
        exit(1);
    }
    return t;
}

void erro_sintatico(const char *msg) {
    if (lookahead.tipo == sEOF) {
        fprintf(stderr, "Erro sintático na linha %d: Fim de arquivo inesperado.\n", lookahead.linha);
        fprintf(stderr, "   Possível causa: String não fechada (\"), falta de 'end.' ou parênteses abertos.\n");
    } else {
        fprintf(stderr, "Erro sintático na linha %d: %s (encontrado '%s')\n", 
                lookahead.linha, msg, nome_token(lookahead.tipo));
    }
    exit(1);
}

void erro_semantico(const char *msg) {
    fprintf(stderr, "Erro semântico na linha %d: %s\n", lookahead.linha, msg);
    exit(1);
}

void consumir(TipoAtomo esperado) {
    if (lookahead.tipo == esperado) {
        lookahead = safe_obter_atomo();
    } else {
        char buf[256];
        snprintf(buf, sizeof(buf), "esperado '%s'", nome_token(esperado));
        erro_sintatico(buf);
    }
}

void check_types(TipoAtomo t1, TipoAtomo t2) {
    if (t1 == t2) return;
    if ((t1 == sINT && t2 == sFLOAT) || (t1 == sFLOAT && t2 == sINT)) return;
    char buf[128];
    snprintf(buf, sizeof(buf), "tipos incompatíveis (%s e %s)", nome_token(t1), nome_token(t2));
    erro_semantico(buf);
}

/* --- Analisador de Expressões --- */

TipoAtomo parse_e(void) {
    TipoAtomo tipo_ret = sVOID;

    if (lookahead.tipo == sNUM_INT) {
        char buf[64]; snprintf(buf, sizeof(buf), "%d", lookahead.lexema.valorInt);
        gera_instr_mepa(NULL, "CRCT", buf, NULL);
        lookahead = safe_obter_atomo();
        return sINT;
    } 
    else if (lookahead.tipo == sNUM_REAL) {
        char buf[64]; snprintf(buf, sizeof(buf), "%f", lookahead.lexema.valorFloat);
        gera_instr_mepa(NULL, "CRCT", buf, NULL);
        lookahead = safe_obter_atomo();
        return sFLOAT;
    } 
    else if (lookahead.tipo == sCARACTER) {
        char buf[8]; snprintf(buf, sizeof(buf), "'%c'", lookahead.lexema.caractere);
        gera_instr_mepa(NULL, "CRCT", buf, NULL);
        lookahead = safe_obter_atomo();
        return sCHAR;
    } 
    else if (lookahead.tipo == sSTRING) {
        gera_instr_mepa(NULL, "CRCT", lookahead.lexema.string, NULL);
        lookahead = safe_obter_atomo();
        return sSTRING;
    } 
    else if (lookahead.tipo == sIDENT) {
        char idname[128];
        strncpy(idname, lookahead.lexema.string, sizeof(idname)-1);
        idname[sizeof(idname)-1] = '\0';
        RegistroTS *sym = ts_buscar(idname);
        if (!sym) {
            char msg[256]; snprintf(msg, sizeof(msg), "identificador '%s' não declarado", idname);
            erro_semantico(msg);
        }
        consumir(sIDENT);
        
        if (lookahead.tipo == sABRE_COLCH) {
            /* Indexação (Lado Direito) */
            if (sym->arr_size == 0) erro_semantico("esta variável não é um vetor");
            consumir(sABRE_COLCH);
            
            /* CORREÇÃO TESTE 15: Detectar sinal de menos (-) explicitamente */
            if (lookahead.tipo == sSUBTR) {
                erro_semantico("indexação inválida: vetores começam em 1 (encontrado número negativo)");
            }
            if (lookahead.tipo == sNUM_INT && lookahead.lexema.valorInt < 1) {
                erro_semantico("indexação inválida: vetores começam em 1 (encontrado 0)");
            }

            TipoAtomo t_idx = parse_exp();
            if (t_idx != sINT) erro_semantico("índice de vetor deve ser inteiro");
            consumir(sFECHA_COLCH);
            
            char addrbuf[32]; snprintf(addrbuf, sizeof(addrbuf), "%d", sym->endereco);
            gera_instr_mepa(NULL, "CRVL", addrbuf, NULL); 
            tipo_ret = sym->tipo;
        } 
        else if (lookahead.tipo == sABRE_PARENT) {
            if (sym->cat != CAT_FUNC) erro_semantico("tentativa de chamar variável como função");
            consumir(sABRE_PARENT);
            
            int arg_count = 0;
            if (lookahead.tipo != sFECHA_PARENT) {
                TipoAtomo t_arg = parse_exp();
                if (arg_count < sym->param_count) check_types(sym->param_types[arg_count], t_arg);
                arg_count++;
                while (lookahead.tipo == sVIRG) {
                    consumir(sVIRG);
                    t_arg = parse_exp();
                    if (arg_count < sym->param_count) check_types(sym->param_types[arg_count], t_arg);
                    arg_count++;
                }
            }
            consumir(sFECHA_PARENT);
            
            if (arg_count != sym->param_count) {
                char msg[256]; snprintf(msg, sizeof(msg), "função '%s' espera %d args, recebeu %d", idname, sym->param_count, arg_count);
                erro_semantico(msg);
            }
            
            gera_instr_mepa(NULL, "CHPR", idname, "0"); 
            tipo_ret = sym->tipo;
        } 
        else {
            if (sym->cat == CAT_FUNC) erro_semantico("nome de função usado sem parênteses");
            char addrbuf[32]; snprintf(addrbuf, sizeof(addrbuf), "%d", sym->endereco);
            gera_instr_mepa(NULL, "CRVL", addrbuf, NULL);
            tipo_ret = sym->tipo;
        }
    } else {
        erro_sintatico("expressão simples esperada");
    }
    return tipo_ret;
}

TipoAtomo parse_ftr(void) {
    if (lookahead.tipo == sNOT) {
        consumir(sNOT);
        TipoAtomo t = parse_ftr();
        gera_instr_mepa(NULL, "NEGA", NULL, NULL);
        return t;
    } else if (lookahead.tipo == sABRE_PARENT) {
        consumir(sABRE_PARENT);
        TipoAtomo t = parse_exp();
        consumir(sFECHA_PARENT);
        return t;
    } else {
        return parse_e();
    }
}

TipoAtomo parse_tmo(void) {
    TipoAtomo t1 = parse_ftr();
    while (lookahead.tipo == sMULT || lookahead.tipo == sDIV || lookahead.tipo == sAND) {
        TipoAtomo op = lookahead.tipo;
        consumir(op);
        TipoAtomo t2 = parse_ftr();
        check_types(t1, t2);
        if (op == sMULT) gera_instr_mepa(NULL, "MULT", NULL, NULL);
        else if (op == sDIV) gera_instr_mepa(NULL, "DIVI", NULL, NULL);
        else if (op == sAND) gera_instr_mepa(NULL, "CONJ", NULL, NULL);
    }
    return t1;
}

TipoAtomo parse_exps(void) {
    TipoAtomo t1 = parse_tmo();
    while (lookahead.tipo == sSOMA || lookahead.tipo == sSUBTR || lookahead.tipo == sOR) {
        TipoAtomo op = lookahead.tipo;
        consumir(op);
        TipoAtomo t2 = parse_tmo();
        check_types(t1, t2);
        if (op == sSOMA) gera_instr_mepa(NULL, "SOMA", NULL, NULL);
        else if (op == sSUBTR) gera_instr_mepa(NULL, "SUBT", NULL, NULL);
        else if (op == sOR) gera_instr_mepa(NULL, "DISJ", NULL, NULL);
    }
    return t1;
}

TipoAtomo parse_exp(void) {
    TipoAtomo t1 = parse_exps();
    while (lookahead.tipo == sMAIOR || lookahead.tipo == sMAIOR_IGUAL || 
           lookahead.tipo == sMENOR || lookahead.tipo == sMENOR_IGUAL || 
           lookahead.tipo == sIGUAL || lookahead.tipo == sDIFERENTE) {
        TipoAtomo op = lookahead.tipo;
        consumir(op);
        TipoAtomo t2 = parse_exps();
        check_types(t1, t2);
        switch(op) {
            case sMAIOR: gera_instr_mepa(NULL, "CMMA", NULL, NULL); break;
            case sMAIOR_IGUAL: gera_instr_mepa(NULL, "CMAG", NULL, NULL); break;
            case sMENOR: gera_instr_mepa(NULL, "CMME", NULL, NULL); break;
            case sMENOR_IGUAL: gera_instr_mepa(NULL, "CMEG", NULL, NULL); break;
            case sIGUAL: gera_instr_mepa(NULL, "CMIG", NULL, NULL); break;
            case sDIFERENTE: gera_instr_mepa(NULL, "CMDG", NULL, NULL); break;
            default: break;
        }
        t1 = sINT;
    }
    return t1;
}

/* --- Comandos --- */

void parse_wr(void) {
    consumir(sWRITE);
    consumir(sABRE_PARENT);
    parse_exp();
    gera_instr_mepa(NULL, "IMPR", NULL, NULL);
    while (lookahead.tipo == sVIRG) {
        consumir(sVIRG);
        parse_exp();
        gera_instr_mepa(NULL, "IMPR", NULL, NULL);
    }
    consumir(sFECHA_PARENT);
}

void parse_rd(void) {
    consumir(sREAD);
    consumir(sABRE_PARENT);
    if (lookahead.tipo == sIDENT) {
        char idn[128]; strncpy(idn, lookahead.lexema.string, sizeof(idn)-1);
        RegistroTS *sym = ts_buscar(idn);
        if (!sym) erro_semantico("variável não declarada em read");
        if (sym->cat != CAT_VAR) erro_semantico("read espera variável");
        
        consumir(sIDENT);
        
        if (lookahead.tipo == sABRE_COLCH) {
            if (sym->arr_size == 0) erro_semantico("variável não é vetor");
            consumir(sABRE_COLCH);
            
            /* CORREÇÃO TESTE 15 */
            if (lookahead.tipo == sSUBTR) {
                erro_semantico("indexação inválida: vetores começam em 1 (encontrado número negativo)");
            }
            if (lookahead.tipo == sNUM_INT && lookahead.lexema.valorInt < 1) {
                erro_semantico("indexação inválida: vetores começam em 1");
            }
            parse_exp();
            consumir(sFECHA_COLCH);
        }

        gera_instr_mepa(NULL, "LEIT", NULL, NULL);
        char abuf[32]; snprintf(abuf, sizeof(abuf), "%d", sym->endereco);
        gera_instr_mepa(NULL, "ARMZ", abuf, NULL);
    } else erro_sintatico("identificador esperado em read");
    consumir(sFECHA_PARENT);
}

void parse_ret(void) {
    consumir(sRETURN);
    if (current_func_type == sVOID) erro_semantico("função void não pode retornar valor");
    
    TipoAtomo t = parse_exp();
    check_types(current_func_type, t);
    
    func_has_return = 1; /* Marca que o retorno foi encontrado */
    gera_instr_mepa(NULL, "RTPR", "0", "0"); 
}

void parse_atr(void) {
    if (lookahead.tipo == sIDENT) {
        char idn[128]; strncpy(idn, lookahead.lexema.string, sizeof(idn)-1);
        RegistroTS *sym = ts_buscar(idn);
        if (!sym) {
            char msg[256]; snprintf(msg, sizeof(msg), "variável '%s' não declarada", idn);
            erro_semantico(msg);
        }
        
        consumir(sIDENT);
        
        if (lookahead.tipo == sABRE_COLCH) {
             /* Atribuição em Vetor */
             if (sym->arr_size == 0) erro_semantico("variável não é vetor");
             consumir(sABRE_COLCH);
             
             /* CORREÇÃO TESTE 15 */
             if (lookahead.tipo == sSUBTR) {
                 erro_semantico("indexação inválida: vetores começam em 1 (encontrado número negativo)");
             }
             if (lookahead.tipo == sNUM_INT && lookahead.lexema.valorInt < 1) {
                 erro_semantico("indexação inválida: vetores começam em 1 (encontrado 0)");
             }
             
             parse_exp(); 
             consumir(sFECHA_COLCH);
        }
        
        consumir(sATRIB);
        TipoAtomo t_expr = parse_exp();
        check_types(sym->tipo, t_expr);
        
        char abuf[32]; snprintf(abuf, sizeof(abuf), "%d", sym->endereco);
        gera_instr_mepa(NULL, "ARMZ", abuf, NULL);
    }
}

/* --- Controle de Fluxo --- */
void parse_if(void) {
    consumir(sIF);
    consumir(sABRE_PARENT);
    parse_exp();
    consumir(sFECHA_PARENT);
    char *rot_false = novo_rotulo();
    gera_instr_mepa(NULL, "DSVF", rot_false, NULL);
    consumir(sTHEN);
    parse_cmd();
    if (lookahead.tipo == sELSE) {
        char *rot_end = novo_rotulo();
        gera_instr_mepa(NULL, "DSVS", rot_end, NULL);
        gera_instr_mepa(rot_false, "NADA", NULL, NULL);
        consumir(sELSE);
        parse_cmd();
        gera_instr_mepa(rot_end, "NADA", NULL, NULL);
        free(rot_end);
    } else {
        gera_instr_mepa(rot_false, "NADA", NULL, NULL);
    }
    free(rot_false);
}

void parse_fr(void) {
    consumir(sFOR);
    consumir(sABRE_PARENT);
    if (lookahead.tipo == sIDENT) parse_atr();
    consumir(sPONTO_VIRG);
    char *rot_inicio = novo_rotulo();
    char *rot_fim = novo_rotulo();
    gera_instr_mepa(rot_inicio, "NADA", NULL, NULL);
    parse_exp();
    gera_instr_mepa(NULL, "DSVF", rot_fim, NULL);
    consumir(sPONTO_VIRG);
    if (lookahead.tipo == sIDENT) parse_atr(); 
    consumir(sFECHA_PARENT);
    parse_cmd();
    gera_instr_mepa(NULL, "DSVS", rot_inicio, NULL);
    gera_instr_mepa(rot_fim, "NADA", NULL, NULL);
    free(rot_inicio); free(rot_fim);
}

void parse_wh(void) {
    char *rot_inicio = novo_rotulo();
    char *rot_fim = novo_rotulo();
    gera_instr_mepa(rot_inicio, "NADA", NULL, NULL);
    consumir(sWHILE);
    consumir(sABRE_PARENT);
    parse_exp();
    consumir(sFECHA_PARENT);
    gera_instr_mepa(NULL, "DSVF", rot_fim, NULL);
    parse_cmd();
    gera_instr_mepa(NULL, "DSVS", rot_inicio, NULL);
    gera_instr_mepa(rot_fim, "NADA", NULL, NULL);
    free(rot_inicio); free(rot_fim);
}

void parse_rpt(void) {
    char *rot_inicio = novo_rotulo();
    gera_instr_mepa(rot_inicio, "NADA", NULL, NULL);
    consumir(sREPEAT);
    while (lookahead.tipo != sUNTIL && lookahead.tipo != sEOF) {
        parse_cmd();
        if (lookahead.tipo == sPONTO_VIRG) consumir(sPONTO_VIRG);
    }
    consumir(sUNTIL);
    consumir(sABRE_PARENT);
    parse_exp();
    consumir(sFECHA_PARENT);
    gera_instr_mepa(NULL, "DSVF", rot_inicio, NULL);
    free(rot_inicio);
}

/* --- Declarações --- */
void parse_tipo(void) {
    if (lookahead.tipo == sINT) consumir(sINT);
    else if (lookahead.tipo == sFLOAT) consumir(sFLOAT);
    else if (lookahead.tipo == sCHAR) consumir(sCHAR);
    else if (lookahead.tipo == sVOID) consumir(sVOID);
    else erro_sintatico("esperado um tipo (int, float, char, void)");
}

void parse_dcl(void) {
    if (lookahead.tipo == sVAR) {
        consumir(sVAR);
        while (lookahead.tipo == sINT || lookahead.tipo == sFLOAT || lookahead.tipo == sCHAR) {
            int tipo_token = lookahead.tipo;
            parse_tipo();
            TipoAtomo decl_type = tipo_token;
            if (lookahead.tipo == sIDENT) {
                do {
                    if (lookahead.tipo == sIDENT) {
                        char namebuf[128];
                        strncpy(namebuf, lookahead.lexema.string, sizeof(namebuf)-1);
                        namebuf[sizeof(namebuf)-1] = '\0';
                        consumir(sIDENT);
                        int arr_size = 0;
                        if (lookahead.tipo == sABRE_COLCH) {
                            consumir(sABRE_COLCH);
                            if (lookahead.tipo == sNUM_INT) {
                                arr_size = lookahead.lexema.valorInt;
                                consumir(sNUM_INT);
                            } else erro_sintatico("tamanho do vetor deve ser inteiro");
                            consumir(sFECHA_COLCH);
                        }
                        RegistroTS *r = ts_inserir(namebuf, CAT_VAR, decl_type, 0);
                        if (r) r->arr_size = arr_size;
                    } else erro_sintatico("esperado nome da variável");
                    if (lookahead.tipo == sVIRG) consumir(sVIRG);
                    else break;
                } while (1);
                consumir(sPONTO_VIRG);
            } else erro_sintatico("esperado nome da variável");
        }
    }
}

void parse_subroutine(void) {
    TipoAtomo ftype = lookahead.tipo;
    parse_tipo();
    char fname[128]; strncpy(fname, lookahead.lexema.string, sizeof(fname)-1);
    fname[sizeof(fname)-1] = '\0';
    consumir(sIDENT);
    consumir(sABRE_PARENT);
    
    struct { char name[128]; TipoAtomo type; } temp_params[64];
    int param_count = 0;
    if (lookahead.tipo != sFECHA_PARENT) {
        while (1) {
            if (param_count >= 64) erro_semantico("muitos parâmetros");
            temp_params[param_count].type = lookahead.tipo;
            parse_tipo();
            strncpy(temp_params[param_count].name, lookahead.lexema.string, 127);
            consumir(sIDENT);
            param_count++;
            if (lookahead.tipo == sVIRG) { consumir(sVIRG); continue; }
            else break;
        }
    }
    consumir(sFECHA_PARENT);
    RegistroTS *rf = ts_inserir(fname, CAT_FUNC, ftype, 0);
    rf->param_count = param_count;
    for(int i=0;i<param_count;i++) rf->param_types[i]=temp_params[i].type;
    
    current_func_type = ftype;
    func_has_return = 0; /* CORREÇÃO TESTE 6: Reinicia flag para esta função */
    
    ts_enter_scope();
    for (int i=0;i<param_count;i++) ts_inserir(temp_params[i].name, CAT_PARAM, temp_params[i].type, 0);
    while (lookahead.tipo == sVAR) parse_dcl();
    
    gera_instr_mepa(fname, "ENPR", "1", NULL);
    int locals = ts_locals_count_current_scope();
    if (locals > 0) { char b[32]; snprintf(b,32,"%d",locals); gera_instr_mepa(NULL,"AMEM",b,NULL); }
    while (lookahead.tipo == sSUBROT) parse_subrot();
    parse_bco();
    
    /* CORREÇÃO TESTE 6: Verificar se função não-void retornou algo */
    if (ftype != sVOID && !func_has_return) {
        char msg[256];
        snprintf(msg, sizeof(msg), "Sub-rotina '%s' deve retornar um valor (comando return ausente)", fname);
        erro_semantico(msg);
    }

    if (ftype == sVOID) gera_instr_mepa(NULL, "RTPR", "0", "0");
    ts_exit_scope();
    if (lookahead.tipo == sPONTO_VIRG) consumir(sPONTO_VIRG);
    current_func_type = sVOID;
}

void parse_subrot(void) {
    consumir(sSUBROT);
    while (lookahead.tipo == sINT || lookahead.tipo == sFLOAT || lookahead.tipo == sCHAR || lookahead.tipo == sVOID) {
        parse_subroutine();
    }
}

void parse_bco(void) {
    consumir(sBEGIN);
    while (lookahead.tipo != sEND && lookahead.tipo != sEOF) {
        parse_cmd();
        if (lookahead.tipo == sPONTO_VIRG) consumir(sPONTO_VIRG);
        else break;
    }
    consumir(sEND);
}

void parse_cmd(void) {
    switch (lookahead.tipo) {
        case sWRITE: parse_wr(); break;
        case sREAD: parse_rd(); break;
        case sIF: parse_if(); break;
        case sFOR: parse_fr(); break;
        case sWHILE: parse_wh(); break;
        case sREPEAT: parse_rpt(); break;
        case sBEGIN: parse_bco(); break;
        case sRETURN: parse_ret(); break;
        case sIDENT: parse_atr(); break;
        default:
            if (lookahead.tipo == sSUBTR) {
                fprintf(stderr, "Erro sintático na linha %d: encontrado '-'.\n   Nota: Comentários '--' não são padrão (use chaves), ou há um erro de expressão.\n", lookahead.linha);
                exit(1);
            }
            erro_sintatico("comando inesperado");
    }
}

void parse_ini(void) {
    while (lookahead.tipo != sPRG && lookahead.tipo != sEOF) lookahead = obter_atomo();
    if (lookahead.tipo == sEOF) erro_sintatico("Arquivo vazio ou sem 'prg'");
    consumir(sPRG);
    if (lookahead.tipo == sIDENT) {
        char p[128]; strncpy(p, lookahead.lexema.string, 127); p[127]='\0';
        ts_inserir(p, CAT_FUNC, sVOID, 0);
        gera_instr_mepa(NULL, "INPP", NULL, NULL);
        consumir(sIDENT);
    } else erro_sintatico("esperado nome do programa");
    consumir(sPONTO_VIRG);
    char *rot = novo_rotulo();
    gera_instr_mepa(NULL, "DSVS", rot, NULL);
    while (lookahead.tipo == sVAR) parse_dcl();
    int g = ts_locals_count_current_scope();
    if (g>0) { char b[32]; snprintf(b,32,"%d",g); gera_instr_mepa(NULL,"AMEM",b,NULL); }
    if (lookahead.tipo == sSUBROT) parse_subrot();
    gera_instr_mepa(rot, "NADA", NULL, NULL); free(rot);
    parse_bco();
    gera_instr_mepa(NULL, "PARA", NULL, NULL);
    consumir(sPONTO);
}