# Planeta do Tesouro

[![Language](https://img.shields.io/badge/Language-C-blue.svg)](https://en.wikipedia.org/wiki/C_(programming_language))
[![Library](https://img.shields.io/badge/Library-Raylib-green.svg)](https://www.raylib.com/)

Jogo de labirinto 2D em C usando Raylib. O jogador (`@`) navega pelo labirinto coletando múltiplos tesouros (`E`), evitando paredes (`#`).

**Projeto educativo** - Monitoria Fundamentos da Programação 2 (2025/2)

## Funcionalidades

- Sistema de múltiplos tesouros com contador
- Carregamento de mapas de arquivos texto
- Movimentação com setas do teclado
- Detecção de colisões e coleta automática
- Interface visual com status do jogo
- Gerenciamento automático de memória
- Estados do jogo (jogando/completo)

## Elementos do Jogo

| Símbolo | Descrição |
|---------|-----------|
| `@` | Jogador |
| `#` | Parede |
| `.` | Caminho livre |
| `E` | Tesouro |

## Instalação

**Pré-requisitos:** GCC, Make, Raylib (incluído)

```bash
git clone https://github.com/vitorviana1011/PlanetaDoTesouro
cd PlanetaDoTesouro
make run
```

**Instalação manual do Raylib (Ubuntu/Debian):**
```bash
sudo apt install libraylib-dev
```

## Como Jogar

- Execute: `make run`
- **Setas do teclado** para movimento do jogador
- **Objetivo**: coletar todos os tesouros (`E`) do mapa
- **Interface**: contador "Tesouros: X/Y" no canto superior esquerdo
- **Vitória**: mensagem de parabéns quando todos os tesouros forem coletados
- **ESC** para sair a qualquer momento

## Formato do Mapa

```
5 5 2
@####
....#
#E#E#
#...#
###..
```

**Primeira linha**: `linhas colunas quantidade_tesouros`
**Demais linhas**: grid do labirinto usando os símbolos da tabela acima

### Regras do mapa:
- Deve haver exatamente 1 jogador (`@`)
- Quantidade de tesouros (`E`) deve corresponder ao número especificado
- Mapa é centralizado automaticamente na tela

## Comandos Make

```bash
make run      # Compila e executa
make clean    # Remove arquivos compilados  
make debug    # Debugger (gdb)
make help     # Mostra ajuda do Makefile
```

## Autor

**Vitor Viana** - vitorluizcviana@gmail.com

**Monitoria Fundamentos da Programação 2 - 2025/2**
