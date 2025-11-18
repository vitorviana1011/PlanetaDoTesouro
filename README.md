# 🏴‍☠️ Planeta do Tesouro

[![Language](https://img.shields.io/badge/Language-C-blue.svg)](https://en.wikipedia.org/wiki/C_(programming_language))
[![Library](https://img.shields.io/badge/Library-Raylib-green.svg)](https://www.raylib.com/)
[![Platform](https://img.shields.io/badge/Platform-Linux%20%7C%20Windows-lightgrey.svg)](#instalação)

Um jogo de labirinto 2D desenvolvido em C utilizando a biblioteca **Raylib**. O jogador controla um personagem (`@`) que deve navegar por um labirinto em busca do tesouro (`E`), evitando obstáculos representados por paredes (`#`).

## 📋 Sobre o Projeto

Este projeto foi desenvolvido como material educativo para a **monitoria da disciplina Fundamentos da Programação 2 - 2025/2**, servindo como exemplo prático de:

- 🎮 Desenvolvimento de jogos em C
- 📚 Uso da biblioteca Raylib
- 🗂️ Manipulação de arquivos
- 🧠 Estruturas de dados e algoritmos
- 💾 Gerenciamento de memória dinâmica

## 🚀 Funcionalidades

- ✅ Carregamento de mapas a partir de arquivos de texto
- ✅ Movimentação do jogador com as setas do teclado
- ✅ Detecção de colisões com paredes
- ✅ Interface gráfica simples e intuitiva
- ✅ Sistema de coordenadas baseado em grid
- ✅ Gerenciamento automático de memória

## 🎯 Elementos do Jogo

| Símbolo | Descrição |
|---------|-----------|
| `@` | Jogador (posição inicial) |
| `#` | Parede (obstáculo) |
| `.` | Espaço livre (caminho) |
| `E` | Tesouro (objetivo) |

## 🛠️ Instalação

### Pré-requisitos

- **Compilador GCC**
- **Make**
- **Raylib** (incluído no projeto ou via package manager)

### Opção 1: Usando bibliotecas incluídas (Recomendado)

```bash
# Clone o repositório
git clone [URL_DO_SEU_REPOSITORIO]
cd PlanetaDoTesouro

# Compile e execute
make run
```

### Opção 2: Instalando Raylib via package manager

#### Ubuntu/Debian:
```bash
sudo apt update
sudo apt install libraylib-dev

# Clone e compile
git clone [URL_DO_SEU_REPOSITORIO]
cd PlanetaDoTesouro
make run
```

#### Fedora:
```bash
sudo dnf install raylib-devel
```

#### Arch Linux:
```bash
sudo pacman -S raylib
```

## 🎮 Como Jogar

1. **Execute o jogo**: `make run`
2. **Use as setas do teclado** para mover o jogador:
   - `↑` - Mover para cima
   - `↓` - Mover para baixo
   - `←` - Mover para a esquerda
   - `→` - Mover para a direita
3. **Objetivo**: Navegue pelo labirinto até alcançar o tesouro (`E`)
4. **Pressione ESC** ou feche a janela para sair

## 📁 Estrutura do Projeto

```
PlanetaDoTesouro/
├── 📄 planetadotesouro.c      # Arquivo principal do jogo
├── 📄 manipulaArquivos.c      # Funções de manipulação de arquivos
├── 📄 manipulaArquivos.h      # Header com declarações
├── 📄 Makefile                # Sistema de build
├── 📄 README.md               # Este arquivo
├── 📄 .gitignore              # Arquivos ignorados pelo Git
├── 📂 mapas/                  # Mapas do jogo
│   └── 📄 mapa1.txt          # Mapa principal (5x5)
├── 📂 includes/               # Headers do Raylib
│   ├── 📄 raylib.h
│   ├── 📄 raymath.h
│   └── 📄 rlgl.h
├── 📂 lib/                    # Bibliotecas Linux
│   └── 📄 libraylib.a
└── 📂 libwin/                 # Bibliotecas Windows
    └── 📄 libraylib.a
```

## 🗺️ Formato do Mapa

Os mapas são arquivos de texto simples com o seguinte formato:

```
5 5
@####
....#
#.#.#
#...#
###.E
```

- **Primeira linha**: `linhas colunas` (dimensões do mapa)
- **Linhas seguintes**: Grid do labirinto usando os símbolos descritos acima

## 🔧 Comandos Make Disponíveis

```bash
make          # Compila o jogo
make run      # Compila e executa
make debug    # Inicia com debugger (gdb)
make clean    # Remove arquivos compilados
make help     # Mostra ajuda
```

## 🧩 Arquitetura do Código

### Principais Estruturas:

```c
typedef struct {
    char **dados;    // Grid do mapa
    int linhas;      // Número de linhas
    int colunas;     // Número de colunas
} Mapa;

typedef struct {
    int x;           // Coordenada X (coluna)
    int y;           // Coordenada Y (linha)
} Jogador;
```

### Principais Funções:

- `carregaMapa()` - Carrega mapa do arquivo
- `encontrarJogador()` - Encontra posição inicial do '@'
- `movimentarJogador()` - Processa movimento com validação
- `liberaMapa()` - Libera memória alocada

## 🎓 Conceitos Demonstrados

- **Programação em C**: Estruturas, ponteiros, arrays dinâmicos
- **Raylib**: Inicialização, loop principal, entrada de teclado
- **Manipulação de Arquivos**: `fopen()`, `fscanf()`, `fclose()`
- **Gerenciamento de Memória**: `malloc()`, `free()`
- **Algoritmos**: Detecção de colisão, validação de movimento
- **Estruturas de Dados**: Arrays 2D, estruturas customizadas

## 👨‍💻 Autor

**Vitor Viana** - [vitorluizcviana@gmail.com](mailto:vitorluizcviana@gmail.com)

## 📚 Recursos de Aprendizado

- [Documentação Raylib](https://www.raylib.com/cheatsheet/cheatsheet.html)
- [Tutorial Raylib em C](https://www.raylib.com/examples.html)
- [Referência da Linguagem C](https://en.cppreference.com/w/c)

## 🤝 Contribuições

Este é um projeto educativo. Sugestões e melhorias são bem-vindas através de issues ou pull requests!

## 📄 Licença

Projeto desenvolvido para fins educacionais - Monitoria FP2 2025/2

---

**🎮 Divirta-se explorando o Planeta do Tesouro!** 🏴‍☠️
