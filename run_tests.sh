#!/bin/bash

# Configurações
COMPILER="./projeto/lpdc"
TEST_DIR="testes_do_projeto"
GREEN='\033[0;32m'
RED='\033[0;31m'
YELLOW='\033[1;33m'
NC='\033[0m' # Sem cor

# Garante que o projeto está compilado
echo "--- Recompilando o projeto ---"
cd projeto
make clean > /dev/null
make > /dev/null
if [ $? -ne 0 ]; then
    echo -e "${RED}Erro fatal: O projeto não compilou.${NC}"
    exit 1
fi
cd ..
echo -e "${GREEN}Compilação OK.${NC}\n"

# Contadores
PASS=0
FAIL=0

echo "=========================================="
echo "   RODANDO TESTES DE SUCESSO (Esperado: OK)"
echo "=========================================="

for t in "$TEST_DIR"/*.lpd; do
    if [[ "$t" != *"_erro.lpd" ]]; then
        $COMPILER "$t" > /dev/null 2>&1
        if [ $? -eq 0 ]; then
            echo -e "${GREEN}[PASS]${NC} $(basename "$t")"
            ((PASS++))
        else
            echo -e "${RED}[FAIL]${NC} $(basename "$t") (Deveria ter compilado)"
            ((FAIL++))
        fi
    fi
done

echo ""
echo "=========================================="
echo "   RODANDO TESTES DE ERRO (Esperado: Falhar)"
echo "=========================================="

for t in "$TEST_DIR"/*_erro.lpd; do
    # Captura a saída completa (stdout e stderr)
    OUTPUT=$($COMPILER "$t" 2>&1)
    
    # Verifica se a saída contém "Erro" (Léxico, Sintático ou Semântico)
    if echo "$OUTPUT" | grep -q "Erro"; then
        echo -e "${GREEN}[PASS]${NC} $(basename "$t")"
        # Extrai a linha do erro e imprime em amarelo com indentação
        ERROR_MSG=$(echo "$OUTPUT" | grep "Erro" | head -n 1)
        echo -e "${YELLOW}      └── $ERROR_MSG${NC}"
        ((PASS++))
    else
        echo -e "${RED}[FAIL]${NC} $(basename "$t") (Compilou com sucesso, mas deveria falhar!)"
        ((FAIL++))
    fi
done

echo ""
echo "------------------------------------------"
echo -e "Total de Testes: $((PASS+FAIL))"
echo -e "Passou: ${GREEN}$PASS${NC}"
echo -e "Falhou: ${RED}$FAIL${NC}"
echo "------------------------------------------"

if [ $FAIL -eq 0 ]; then
    exit 0
else
    exit 1
fi