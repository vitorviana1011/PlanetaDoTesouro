# Planeta do Tesouro

[![Language](https://img.shields.io/badge/Language-C-blue.svg)](https://en.wikipedia.org/wiki/C_(programming_language))
[![Library](https://img.shields.io/badge/Library-Raylib-green.svg)](https://www.raylib.com/)

Jogo de labirinto 2D em C usando Raylib. O jogador (`@`) navega pelo labirinto coletando múltiplos tesouros (`E`), evitando paredes (`#`).

**Projeto educativo** - Monitoria Fundamentos da Programação 2 (2025/2)

## Funcionalidades

- **Sistema de múltiplas fases** com progressão automática
- **Múltiplos tesouros** por fase com contador em tempo real
- **Verificação de arquivos** - detecta mapas disponíveis automaticamente
- **Transição controlada** entre fases (requer ENTER para avançar)
- **Carregamento dinâmico** de mapas de arquivos texto
- **Movimentação fluida** com setas do teclado
- **Detecção de colisões** e coleta automática de tesouros
- **Interface visual** com status da fase e progresso
- **Mapa centralizado** automaticamente na tela
- **Gerenciamento robusto** de memória
- **Estados do jogo** (menu/jogando/entre fases/completo)

## Elementos do Jogo

| Símbolo | Descrição |
|---------|-----------|
| `@` | Jogador |
| `#` | Parede |
| `.` | Caminho livre |
| `T` | Tesouro |
| `I` | Inimigo |

## Inimigos

Inimigos (`I`) se movem automaticamente em direção ao jogador usando IA simples. Características:

- **Movimento**: Perseguem o jogador horizontalmente, depois verticalmente (0.5s entre movimentos)
- **Colisão**: Reduz vidas do jogador e ativa invencibilidade temporária (pisca)
- **Renderização**: Desenhados por cima do mapa sem apagar tesouros
- **Game Over**: Ocorre quando vidas chegam a zero

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

- **Execute**: `make run`
- **Movimento**: Use as setas do teclado para navegar
- **Objetivo**: Colete todos os tesouros (`T`) de cada fase
- **Interface**: 
  - `Fase: X` - Fase atual
  - `Tesouros: X/Y` - Progresso na fase
- **Progressão**: 
  - Complete uma fase coletando todos os tesouros
  - Aparecer "Fase Completa! Aperte ENTER para continuar"
  - Pressione **ENTER** para avançar para a próxima fase
- **Fim do jogo**: Quando não há mais fases, aparece mensagem de vitória
- **Sair**: **ESC** a qualquer momento

## Formato do Mapa

```
5 5 2
@####
....#
#T#T#
#...#
###I.
```

**Primeira linha**: `linhas colunas quantidade_tesouros`
**Demais linhas**: grid do labirinto usando os símbolos da tabela acima

### Regras do mapa:
- Deve haver exatamente 1 jogador (`@`) por mapa
- Quantidade de tesouros (`E`) deve corresponder ao número especificado
- Mapas são centralizados automaticamente na tela
- Arquivos de mapa seguem o padrão `mapas/mapaN.txt` (N = 1, 2, 3...)
- O jogo verifica automaticamente quais mapas existem

## Comandos Make

```bash
make run      # Compila e executa o jogo
make clean    # Remove arquivos compilados  
make debug    # Inicia debugger (gdb)
make help     # Mostra ajuda do Makefile
```
## Funcionalidades Técnicas

- **Múltiplas fases**: Detecção automática e carregamento sob demanda de mapas
- **Gerenciamento de memória**: Alocação dinâmica com liberação segura
- **Estados do jogo**: JOGANDO → ENTRE_FASES → JOGO_COMPLETO
- **Verificação de arquivos**: Evita erros de abertura de mapas inexistentes

## Conceitos Demonstrados

- **Estruturas e ponteiros**: Arrays 2D, passagem por referência
- **Gerenciamento de memória**: `malloc()`, `free()` 
- **Manipulação de arquivos**: Leitura e verificação de existência
- **Máquina de estados**: Controle de fluxo do jogo
- **Programação gráfica**: Biblioteca Raylib

## Autor

**Vitor Viana** - vitorluizcviana@gmail.com

**Monitoria Fundamentos da Programação 2 - 2025/2**
