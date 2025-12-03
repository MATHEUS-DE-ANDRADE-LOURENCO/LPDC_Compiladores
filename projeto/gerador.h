/* *******************************************************************************
 * Matheus de Andrade Lourenço - 10419691
 * Murillo Cardoso Ferreira    - 10418082
 *
 * gerador.h
 * Interface mínima para geração MEPA (stubs)
 *******************************************************************************/
#ifndef GERADOR_H
#define GERADOR_H

/* Inicialização / finalização do emissor MEPA (arquivo) */
/* gera_init: abre/cria o arquivo MEPA indicado por `filename`.
 * gera_close: fecha o arquivo (se aberto).
 */
void gera_init(const char *filename);
void gera_close(void);
/*
 * gera_instr_mepa: emite uma instrução para o arquivo MEPA. Os
 * parâmetros aceitos são: `rotulo` (opcional), `mnemonico` e até
 * dois parâmetros adicionais. Se `out` não estiver definido, a
 * instrução é impressa em `stdout`.
 */
void gera_instr_mepa(char *rotulo, char *mnemonico, char *param1, char *param2);
/* novo_rotulo: retorna uma string alocada dinamicamente com um
 * novo rótulo único. O chamador deve liberar a string. */
char* novo_rotulo(void);

#endif
