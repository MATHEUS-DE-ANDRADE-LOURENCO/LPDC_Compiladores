# Projeto Prático 2 — LPDC (Compiladores)

Compilador completo para a linguagem **LPD**, com validação **léxica, sintática e semântica**, e geração de código **MEPA (Máquina de Execução de Pascal)**.

---

## 🎯 Objetivo do Projeto

Implementar um compilador capaz de:

- Ler um programa escrito em **LPD**
- Validar:
  - componentes léxicos
  - estrutura sintática (parser)
  - regras semânticas
- Gerar como saída um código equivalente em **MEPA**

---

## 📌 1. Analisador Sintático (Parser)

### ✔ Requisitos
- Implementar um **ASDR (Analisador Sintático Descendente Recursivo)**.
- Basear-se diretamente na **GLC do Apêndice A** da especificação LPD.
- Cada **não-terminal da gramática deve corresponder a uma função em C.**

### ⚠ Tratamento de Erros
- Usar **Modo Pânico** → abortar compilação após falha grave.
- Deve reportar:
  - número da linha
  - mensagem explicativa

### 📤 Resultado Esperado
- Em caso de sucesso:  
  `"Código compilado com sucesso!"`
- Caso contrário: mensagens claras de erro sintático.

---

## 📌 2. Tabela de Símbolos + Análise Semântica

### 🧱 Estrutura Armazenada
Cada identificador deve guardar:
- Categoria (constante, variável, função...)
- Tipo (int, float...)
- Endereço de alocação (na pilha MEPA)

### 💡 Implementação Sugerida
Estruturas possíveis:
- lista encadeada
- árvore
- vetor ordenado
- hash table

### 📌 Regras Semânticas
- Declaração obrigatória antes do uso
- Proibição de variáveis duplicadas no mesmo escopo
- Checagem de tipos em:
  - expressões
  - atribuições

### 📤 Saída
Arquivo `*.ts` contendo o conteúdo final da Tabela de Símbolos.

**Exemplo**

TS[ lex: triang | cat: programa | tip: void | end: -1 ]
TS[ lex: a | cat: variável | tip: int | end: 0 ]
TS[ lex: b | cat: variável | tip: int | end: 1 ]


---

## 📌 3. Geração de Código — MEPA

### 🔧 Método
- Utilizar **Tradução Dirigida à Sintaxe (SDT)** durante o parsing.
- Gerar instruções enquanto reconhece estruturas.

### 📌 Requisitos da Tradução
- Gerenciar pilha (alocação e liberação)
- Expressões em **notação pós-fixa**
- Instruções para:
  - aritmética
  - lógicas
  - relacionais
  - controle de fluxo (`if`, `while`, `repeat`) usando rótulos

### 📤 Saída
Arquivo `*.mepa` com o programa traduzido.

---

## 🗂 Organização do Código

| Arquivo | Função |
|---------|--------|
| `main.c` | ponto de entrada; coordena os módulos |
| `analex.h` | interface do analisador léxico (fornecido) |
| `asdr.c/.h` | implementação do parser e regras gramaticais |
| `tabsimb.c/.h` | gerenciamento da Tabela de Símbolos |
| `gerador.c/.h` | geração formatada das instruções MEPA |

---

## 📦 Entregáveis

- Código completo em **C (.c / .h)**
- Makefile
- Compilação com:

gcc -Wall -Wextra -std=c99

- Executável em **Linux (Ubuntu)**

---

## 📊 Critérios de Avaliação

- Atendimento aos requisitos
- Correção sintática (aceita válidos / rejeita inválidos)
- Validação semântica
- Código MEPA funcional e executável
- Qualidade do código:
- modularização
- legibilidade
- organização

---
