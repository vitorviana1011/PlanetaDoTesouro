# Planeta do Tesouro

[![Language](https://img.shields.io/badge/Language-C-blue.svg)](https://en.wikipedia.org/wiki/C_(programming_language))
[![Library](https://img.shields.io/badge/Library-Raylib-green.svg)](https://www.raylib.com/)

Jogo de labirinto 2D em C usando Raylib. O jogador (`@`) navega pelo labirinto até encontrar o tesouro (`E`), evitando paredes (`#`).

**Projeto educativo** - Monitoria Fundamentos da Programação 2 (2025/2)

## Funcionalidades

- Carregamento de mapas de arquivos texto
- Movimentação com setas do teclado
- Detecção de colisões
- Gerenciamento automático de memória

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
- Setas do teclado para movimento
- Objetivo: alcançar o tesouro (`E`)
- ESC para sair

## Exemplo de Formato do Mapa

```
5 5
@####
....#
#.#.#
#...#
###.E
```

Primeira linha: dimensões (`linhas colunas`). Demais linhas: grid do labirinto.

## Comandos Make

```bash
make run      # Compila e executa
make clean    # Remove arquivos compilados  
make debug    # Debugger (gdb)
```

## Autor

**Vitor Viana** - vitorluizcviana@gmail.com

**Monitoria FD2 2025/2**
