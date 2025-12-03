#/*******************************************************************************
 * Matheus de Andrade Lourenço - 10419691
 * Murillo Cardoso Ferreira    - 10418082
 *
 * asdr.c
 * Analisador Sintático (esqueleto)
 ******************************************************************************/
#include <stdio.h>
#include "analex.h"
#include "asdr.h"
#include <stdlib.h>
#include <string.h>
#include "tabsimb.h"
#include "gerador.h"

TInfoAtomo lookahead;


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
		case sIDENT: return "ident";
		case sNUM_INT: return "num_int";
		case sNUM_REAL: return "num_real";
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

void erro_sintatico(const char *msg) {
	/*
	 * Erro sintático: imprime mensagem formatada contendo a linha
	 * atual do token `lookahead`, a descrição esperada (`msg`) e
	 * o token efetivamente encontrado (via `nome_token`).
	 * Em seguida abortamos a compilação com código 1.
	 */
	fprintf(stderr, "Erro sintático na linha %d: %s (encontrado '%s')\n", lookahead.linha, msg, nome_token(lookahead.tipo));
	exit(1);
}

void consumir(TipoAtomo esperado) {
	/*
	 * Consumir: verifica se o token corrente (`lookahead`) corresponde
	 * ao token `esperado`. Se sim, avança para o próximo token via
	 * `obter_atomo()`. Caso contrário, constrói uma mensagem de
	 * erro e delega para `erro_sintatico`.
	 */
	if (lookahead.tipo == esperado) {
		lookahead = obter_atomo();
	} else {
		char buf[256];
		snprintf(buf, sizeof(buf), "esperado '%s'", nome_token(esperado));
		erro_sintatico(buf);
	}
}

/* Implementação recursiva-descendente baseada na EBNF fornecida */

void parse_e(void) {
	/*
	 * parse_e - trata expressões primárias / fatores atômicos:
	 * - constantes (números, strings, caracteres)
	 * - identificadores (variáveis/vetores/ chamadas de função)
	 * Para constantes, empilha o valor com `CRCT` no MEPA.
	 */
	if (lookahead.tipo == sNUM_INT || lookahead.tipo == sNUM_REAL || lookahead.tipo == sSTRING || lookahead.tipo == sCARACTER) {
		/* constantes: empilhar constante */
		if (lookahead.tipo == sNUM_INT) {
			char buf[64]; snprintf(buf, sizeof(buf), "%d", lookahead.lexema.valorInt);
			gera_instr_mepa(NULL, "CRCT", buf, NULL);
		} else if (lookahead.tipo == sNUM_REAL) {
			char buf[64]; snprintf(buf, sizeof(buf), "%f", lookahead.lexema.valorFloat);
			gera_instr_mepa(NULL, "CRCT", buf, NULL);
		} else if (lookahead.tipo == sCARACTER) {
			char buf[8]; snprintf(buf, sizeof(buf), "'%c'", lookahead.lexema.caractere);
			gera_instr_mepa(NULL, "CRCT", buf, NULL);
		} else if (lookahead.tipo == sSTRING) {
			gera_instr_mepa(NULL, "CRCT", lookahead.lexema.string, NULL);
		}
		lookahead = obter_atomo();
	} else if (lookahead.tipo == sIDENT) {
		/* Identificador: checar existência na tabela e gerar acesso */
			/* Identificador - verificar existência e uso */
			char idname[128];
			strncpy(idname, lookahead.lexema.string, sizeof(idname)-1);
			idname[sizeof(idname)-1] = '\0';
			RegistroTS *sym = ts_buscar(idname);
			if (!sym) {
				fprintf(stderr, "Erro semântico na linha %d: identificador '%s' não declarado\n", lookahead.linha, idname);
				exit(1);
			}
			consumir(sIDENT);
			/* uso de identificador: carregar valor (por endereço) */
			char addrbuf[32]; snprintf(addrbuf, sizeof(addrbuf), "%d", sym->endereco);
			gera_instr_mepa(NULL, "CRVL", addrbuf, NULL);
		/* Suportar indexação de vetores: IDENT '[' <exp> ']' */
		if (lookahead.tipo == sABRE_COLCH) {
			if (sym->arr_size == 0) {
				fprintf(stderr, "Erro semântico na linha %d: identificador '%s' não é vetor\n", lookahead.linha, idname);
				exit(1);
			}
			consumir(sABRE_COLCH);
			parse_exp();
			consumir(sFECHA_COLCH);
		} else if (lookahead.tipo == sABRE_PARENT) {
			/* chamada de função */
			if (sym->cat != CAT_FUNC) {
				fprintf(stderr, "Erro semântico na linha %d: '%s' não é uma função\n", lookahead.linha, idname);
				exit(1);
			}
			consumir(sABRE_PARENT);
			int arg_count = 0;
			if (lookahead.tipo != sFECHA_PARENT) {
				parse_exp(); arg_count++;
				while (lookahead.tipo == sVIRG) {
					consumir(sVIRG);
					parse_exp(); arg_count++;
				}
			}
			consumir(sFECHA_PARENT);
			if (arg_count != sym->param_count) {
				fprintf(stderr, "Erro semântico na linha %d: função '%s' esperava %d parâmetro(s), recebeu %d\n", lookahead.linha, idname, sym->param_count, arg_count);
				exit(1);
			}
		}
	} else {
		erro_sintatico("expressão simples esperada");
	}
}

void parse_ftr(void) {
	if (lookahead.tipo == sNOT) {
		consumir(sNOT);
		parse_ftr();
	} else if (lookahead.tipo == sABRE_PARENT) {
		consumir(sABRE_PARENT);
		parse_exp();
		consumir(sFECHA_PARENT);
	} else {
		parse_e();
	}
}

void parse_tmo(void) {
	parse_ftr();
	while (lookahead.tipo == sMULT || lookahead.tipo == sDIV || lookahead.tipo == sAND) {
		TipoAtomo op = lookahead.tipo;
		consumir(op);
		parse_ftr();
		if (op == sMULT) gera_instr_mepa(NULL, "MULT", NULL, NULL);
		else if (op == sDIV) gera_instr_mepa(NULL, "DIVI", NULL, NULL);
		else if (op == sAND) gera_instr_mepa(NULL, "AND", NULL, NULL);
	}
}

void parse_exps(void) {
	parse_tmo();
	while (lookahead.tipo == sSOMA || lookahead.tipo == sSUBTR || lookahead.tipo == sOR) {
		TipoAtomo op = lookahead.tipo;
		consumir(op);
		parse_tmo();
		if (op == sSOMA) gera_instr_mepa(NULL, "SOMA", NULL, NULL);
		else if (op == sSUBTR) gera_instr_mepa(NULL, "SUBT", NULL, NULL);
		else if (op == sOR) gera_instr_mepa(NULL, "OR", NULL, NULL);
	}
}

void parse_exp(void) {
	parse_exps();
	while (lookahead.tipo == sMAIOR || lookahead.tipo == sMAIOR_IGUAL || lookahead.tipo == sMENOR || lookahead.tipo == sMENOR_IGUAL || lookahead.tipo == sIGUAL || lookahead.tipo == sDIFERENTE) {
		TipoAtomo op = lookahead.tipo;
		consumir(op);
		parse_exps();
		switch(op) {
			case sMAIOR: gera_instr_mepa(NULL, "CMMAI", NULL, NULL); break;
			case sMAIOR_IGUAL: gera_instr_mepa(NULL, "CMMAIG", NULL, NULL); break;
			case sMENOR: gera_instr_mepa(NULL, "CMMEN", NULL, NULL); break;
			case sMENOR_IGUAL: gera_instr_mepa(NULL, "CMMEIG", NULL, NULL); break;
			case sIGUAL: gera_instr_mepa(NULL, "CMIG", NULL, NULL); break;
			case sDIFERENTE: gera_instr_mepa(NULL, "CMDIF", NULL, NULL); break;
			default: break;
		}
	}
}

void parse_wr(void) {
	consumir(sWRITE);
	consumir(sABRE_PARENT);
	parse_exp();
	/* imprimir resultado da expressão */
	gera_instr_mepa(NULL, "IMPR", NULL, NULL);
	while (lookahead.tipo == sVIRG) {
		consumir(sVIRG);
		parse_exp();
		gera_instr_mepa(NULL, "IMPR", NULL, NULL);
	}
	consumir(sFECHA_PARENT);
}

void parse_subroutine(void) {
	/* <tipo> <nome>(<parâmetros>) [var ...] [subrot ...] <bco> ; */
	/* armazenar token do tipo antes de consumir */
	TipoAtomo ftype = lookahead.tipo;
	parse_tipo();
	if (lookahead.tipo != sIDENT) erro_sintatico("nome da sub-rotina esperado");
	/* declarar função no escopo atual (global) antes de processar params */
	char fname[128];
	strncpy(fname, lookahead.lexema.string, sizeof(fname)-1);
	fname[sizeof(fname)-1] = '\0';
	consumir(sIDENT);
	/* conserveremos contagem de parâmetros depois de analisar a lista */
	consumir(sABRE_PARENT);
	/* primeiro parse dos parâmetros para contar e armazenar */
	struct { char name[128]; TipoAtomo type; } params[64];
	int param_count = 0;
	if (lookahead.tipo != sFECHA_PARENT) {
		while (1) {
			if (!(lookahead.tipo == sINT || lookahead.tipo == sFLOAT || lookahead.tipo == sCHAR))
				erro_sintatico("tipo esperado em parâmetro");
			int tipo_token = lookahead.tipo;
			parse_tipo();
			if (lookahead.tipo != sIDENT) erro_sintatico("identificador de parâmetro esperado");
			strncpy(params[param_count].name, lookahead.lexema.string, sizeof(params[param_count].name)-1);
			params[param_count].name[sizeof(params[param_count].name)-1] = '\0';
			/* armazenar tipo do parâmetro como token (TipoAtomo) */
			params[param_count].type = tipo_token;
			consumir(sIDENT);
			param_count++;
			if (lookahead.tipo == sVIRG) { consumir(sVIRG); continue; }
			else break;
		}
	}
	consumir(sFECHA_PARENT);
	/* declarar função no escopo atual com param_count */
	RegistroTS *rf = ts_inserir(fname, CAT_FUNC, ftype, 0);
	rf->param_count = param_count;
	/* entrar no scope da função e declarar parâmetros */
	ts_enter_scope();
	for (int i=0;i<param_count;i++) {
		ts_inserir(params[i].name, CAT_PARAM, params[i].type, 0);
	}

	/* variáveis locais */
	while (lookahead.tipo == sVAR) parse_dcl();
	/* emitir label da função e reservar memória local */
	gera_instr_mepa(fname, NULL, NULL, NULL);
	{
		int locals = ts_locals_count_current_scope();
		if (locals > 0) {
			char buf[32]; snprintf(buf, sizeof(buf), "%d", locals);
			gera_instr_mepa(NULL, "AMEM", buf, NULL);
		}
	}
	/* sub-rotinas locais (opcional) */
	while (lookahead.tipo == sSUBROT) parse_subrot();
	parse_bco();
	if (lookahead.tipo == sPONTO_VIRG) consumir(sPONTO_VIRG);
	ts_exit_scope();
}

void parse_subrot(void) {
	consumir(sSUBROT);
	/* pode haver várias sub-rotinas seguidas */
	while (lookahead.tipo == sINT || lookahead.tipo == sFLOAT || lookahead.tipo == sCHAR || lookahead.tipo == sVOID) {
		parse_subroutine();
	}
}

void parse_rd(void) {
	consumir(sREAD);
	consumir(sABRE_PARENT);
	if (lookahead.tipo == sIDENT) {
		char idn[128]; strncpy(idn, lookahead.lexema.string, sizeof(idn)-1); idn[sizeof(idn)-1]='\0';
		RegistroTS *sym = ts_buscar(idn);
		if (!sym) { fprintf(stderr, "Erro semântico na linha %d: identificador '%s' não declarado\n", lookahead.linha, idn); exit(1); }
		consumir(sIDENT);
		char abuf[32]; snprintf(abuf, sizeof(abuf), "%d", sym->endereco);
		gera_instr_mepa(NULL, "LEIT", abuf, NULL);
	} else erro_sintatico("identificador esperado em read");
	consumir(sFECHA_PARENT);
}

void parse_ret(void) {
	consumir(sRETURN);
	parse_exp();
}

void parse_atr(void) {
	if (lookahead.tipo == sIDENT) {
		char idn[128]; strncpy(idn, lookahead.lexema.string, sizeof(idn)-1); idn[sizeof(idn)-1]='\0';
		RegistroTS *sym = ts_buscar(idn);
		if (!sym) { fprintf(stderr, "Erro semântico na linha %d: identificador '%s' não declarado\n", lookahead.linha, idn); exit(1); }
		consumir(sIDENT);
		consumir(sATRIB);
		parse_exp();
		/* armazenar valor em identificador (endereço) */
		char abuf[32]; snprintf(abuf, sizeof(abuf), "%d", sym->endereco);
		gera_instr_mepa(NULL, "ARMZ", abuf, NULL);
	} else {
		erro_sintatico("atribuição deve começar com identificador");
	}
}

void parse_if(void) {
	consumir(sIF);
	consumir(sABRE_PARENT);
	parse_exp();
	consumir(sFECHA_PARENT);
	/* gerar salto condicional */
	char *rot_false = novo_rotulo();
	gera_instr_mepa(NULL, "JMPF", rot_false, NULL);
	consumir(sTHEN);
	parse_cmd();
	if (lookahead.tipo == sELSE) {
		char *rot_end = novo_rotulo();
		gera_instr_mepa(NULL, "JMP", rot_end, NULL);
		gera_instr_mepa(rot_false, NULL, NULL, NULL);
		consumir(sELSE);
		parse_cmd();
		gera_instr_mepa(rot_end, NULL, NULL, NULL);
		free(rot_end);
	} else {
		gera_instr_mepa(rot_false, NULL, NULL, NULL);
	}
	free(rot_false);
}

void parse_fr(void) {
	consumir(sFOR);
	consumir(sABRE_PARENT);
	if (lookahead.tipo == sIDENT) {
		parse_atr();
	}
	consumir(sPONTO_VIRG);
	parse_exp();
	consumir(sPONTO_VIRG);
	if (lookahead.tipo == sIDENT) {
		parse_atr();
	}
	consumir(sFECHA_PARENT);
	parse_cmd();
}

void parse_wh(void) {
	consumir(sWHILE);
	consumir(sABRE_PARENT);
	parse_exp();
	consumir(sFECHA_PARENT);
	parse_cmd();
}

void parse_rpt(void) {
	consumir(sREPEAT);
	while (lookahead.tipo != sUNTIL && lookahead.tipo != sEOF) {
		parse_cmd();
		if (lookahead.tipo == sPONTO_VIRG) consumir(sPONTO_VIRG);
	}
	consumir(sUNTIL);
	consumir(sABRE_PARENT);
	parse_exp();
	consumir(sFECHA_PARENT);
}

void parse_bco(void) {
	consumir(sBEGIN);
	while (lookahead.tipo != sEND && lookahead.tipo != sEOF) {
		parse_cmd();
		if (lookahead.tipo == sPONTO_VIRG) consumir(sPONTO_VIRG);
		else break; /* para evitar loop infinito */
	}
	consumir(sEND);
}

void parse_tipo(void) {
	if (lookahead.tipo == sINT) consumir(sINT);
	else if (lookahead.tipo == sFLOAT) consumir(sFLOAT);
	else if (lookahead.tipo == sCHAR) consumir(sCHAR);
	else if (lookahead.tipo == sVOID) consumir(sVOID);
	else erro_sintatico("tipo esperado");
}

void parse_dcl(void) {
	if (lookahead.tipo == sVAR) {
		consumir(sVAR);
		/* aceitar várias linhas do tipo: <tipo> <id>{, <id>} ; */
		while (lookahead.tipo == sINT || lookahead.tipo == sFLOAT || lookahead.tipo == sCHAR) {
			int tipo_token = lookahead.tipo;
			parse_tipo();
			/* usar o token diretamente como tipo para a tabsimb */
			TipoAtomo decl_type = tipo_token;
			if (lookahead.tipo == sIDENT) {
				/* identificador possivelmente com dimensão */
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
							} else erro_sintatico("tamanho de vetor (inteiro) esperado");
							consumir(sFECHA_COLCH);
						}
						RegistroTS *r = ts_inserir(namebuf, CAT_VAR, decl_type, 0);
						if (r) r->arr_size = arr_size;
					} else {
						erro_sintatico("identificador esperado em declaração");
					}
					if (lookahead.tipo == sVIRG) consumir(sVIRG);
					else break;
				} while (1);
				consumir(sPONTO_VIRG);
			} else erro_sintatico("identificador esperado em declaração");
		}
	}
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
			erro_sintatico("comando inesperado");
	}
}

void parse_ini(void) {
	/* <ini> ::= sPRG <id> ; [<dcl>] <bco> . */
	/* Pula possíveis comentários/ruídos iniciais (ex.: linhas começando com '--') */
	while (lookahead.tipo != sPRG && lookahead.tipo != sEOF) {
		lookahead = obter_atomo();
	}
	if (lookahead.tipo != sPRG) erro_sintatico("palavra-chave 'prg' esperada");
	consumir(sPRG);
	/* registrar nome do programa na tabela de símbolos */
	if (lookahead.tipo == sIDENT) {
		char progname[128];
		strncpy(progname, lookahead.lexema.string, sizeof(progname)-1);
		progname[sizeof(progname)-1] = '\0';
		/* declarar como função/programa com tipo void e 0 parâmetros */
		RegistroTS *r = ts_inserir(progname, CAT_FUNC, sVOID, 0);
		if (r) r->param_count = 0;
		/* iniciar gerador MEPA para o programa */
		gera_instr_mepa(NULL, "INPP", NULL, NULL);
		consumir(sIDENT);
	} else erro_sintatico("identificador (nome do programa) esperado");
	consumir(sPONTO_VIRG);
	while (lookahead.tipo == sVAR) {
		parse_dcl();
	}
	/* reservar memória para variáveis globais (se houver) */
	{
		int g = ts_locals_count_current_scope();
		if (g > 0) {
			char buf[32]; snprintf(buf, sizeof(buf), "%d", g);
			gera_instr_mepa(NULL, "AMEM", buf, NULL);
		}
	}
	if (lookahead.tipo == sSUBROT) {
		parse_subrot();
	}
	parse_bco();
	/* finalizar programa */
	gera_instr_mepa(NULL, "PARA", NULL, NULL);
	consumir(sPONTO);
}