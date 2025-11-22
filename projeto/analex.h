
#ifndef ANALEX_H
#define ANALEX_H

/*///////////////////////
        DEFINIÇÕES
 ///////////////////////*/
typedef enum {
    // Programa
    sPRG,
    // Seção de variáveis
    sVAR,
    // Sub-rotinas
    sSUBROT, sRETURN,
    // Tipos primitivos
    sINT, sFLOAT, sCHAR, sVOID,
    // Bloco de comandos
    sBEGIN, sEND, 
    // Entrada e Saída
    sWRITE, sREAD,
    // Comandos de controle (condicionais e iterativos)
    sIF, sTHEN, sELSE,
    sFOR, sWHILE, sREPEAT, sUNTIL,
    // Operadores lógcos
    sAND, sOR, sNOT,
    // Identificadores (criações do usuário)
    sIDENT,
    // Constantes
    sNUM_INT, sNUM_REAL, sCARACTER, sSTRING,
    // Operador de atribuição
    sATRIB,
    // Operadores aritméticos
    sSOMA, sSUBTR, sMULT, sDIV,
    // Operadores relacionais
    sMAIOR, sMAIOR_IGUAL,
    sMENOR, sMENOR_IGUAL,
    sIGUAL, sDIFERENTE,
    // Outros símbolos
    sABRE_PARENT, sFECHA_PARENT,
    sABRE_COLCH, sFECHA_COLCH,
    sPONTO, sVIRG, sPONTO_VIRG,
    // Sinalizadores (flags)
    sEOF, sERRO_LEXICO
} TipoAtomo;

typedef union {
    int valorInt;
    float valorFloat;
    char caractere;
    char string[256];
} ValorLexema;

typedef struct {
    TipoAtomo tipo;
    ValorLexema lexema;
    int linha;
} TInfoAtomo;

/*///////////////////////
    VARIÁVEIS GLOBAIS
      COMPARTILHADAS
 ///////////////////////*/
extern FILE *fonte;

/*///////////////////////
        PROTÓTIPOS
 ///////////////////////*/
TInfoAtomo obter_atomo(void);

#endif