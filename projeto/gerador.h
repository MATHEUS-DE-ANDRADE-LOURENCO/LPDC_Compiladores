/* *******************************************************************************
 * Matheus de Andrade Lourenço - 1041691
 * Murillo Cardoso Ferreira    - 10418082
 *
 * gerador.h
 * Interface mínima para geração MEPA (stubs)
 *******************************************************************************/
#ifndef GERADOR_H
#define GERADOR_H

/* Inicialização / finalização do emissor MEPA (arquivo) */
void gera_init(const char *filename);
void gera_close(void);
/* Emite instrução MEPA para o arquivo aberto */
void gera_instr_mepa(char *rotulo, char *mnemonico, char *param1, char *param2);
char* novo_rotulo(void);

#endif
