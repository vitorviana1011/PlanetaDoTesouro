# Makefile para Planeta do Tesouro - Jogo em C usando raylib

# Detectar sistema operacional
ifdef OS
  OS := $(strip $(OS))
else
  OS := $(strip $(shell uname))
endif

# Configurações básicas
TARGET = planetadotesouro
SRCS = planetadotesouro.c manipulaArquivos.c
OBJS = $(SRCS:.c=.o)
CC = gcc
CFLAGS = -g -Wall -Wextra -std=c99

# Extensão do binário e comando de limpeza por plataforma
ifeq ($(OS),Windows_NT)
	BINEXT = .exe
	RM = del /Q /F
else
	BINEXT =
	RM = rm -f
endif

# Tentar usar pkg-config primeiro
PKG_CHECK := $(shell pkg-config --exists raylib 2>/dev/null && echo 0 || echo 1)
ifeq ($(PKG_CHECK),0)
	# raylib encontrado via pkg-config
	RAYLIB_CFLAGS := $(shell pkg-config --cflags raylib)
	RAYLIB_LIBS := $(shell pkg-config --libs raylib)
	RAYLIB_STATUS := "Usando raylib via pkg-config"
else
	# Fallback para paths locais
	RAYLIB_STATUS := "raylib não encontrado via pkg-config, usando paths locais..."
	
	# Verificar se includes/raylib.h existe
	ifeq ($(wildcard includes/raylib.h),)
		RAYLIB_MISSING := 1
	endif
	
	RAYLIB_CFLAGS = -Iincludes
	
	# Bibliotecas por plataforma
	ifeq ($(OS),Windows_NT)
		RAYLIB_LIBS = -Llibwin -lraylib -lm -lopengl32 -lgdi32 -lwinmm
	else
		RAYLIB_LIBS = -Llib -lraylib -lGL -lm -lpthread -ldl -lrt -lX11
	endif
endif

# Regras principais
.PHONY: all clean run debug help

all: check-raylib $(TARGET)$(BINEXT)
	@echo "Compilação concluída: $(TARGET)$(BINEXT)"

check-raylib:
ifdef RAYLIB_MISSING
	@echo "ERRO: raylib.h não encontrado em includes/"
	@echo "Instale raylib com: sudo apt install libraylib-dev"
	@echo "Ou certifique-se de que includes/raylib.h existe"
	@exit 1
endif
	@echo $(RAYLIB_STATUS)

$(TARGET)$(BINEXT): $(OBJS)
	$(CC) $(CFLAGS) $(RAYLIB_CFLAGS) $(OBJS) -o $@ $(RAYLIB_LIBS)

%.o: %.c
	$(CC) $(CFLAGS) $(RAYLIB_CFLAGS) -c $< -o $@

run: $(TARGET)$(BINEXT)
	@echo "Executando $(TARGET)..."
	./$(TARGET)$(BINEXT)

clean:
	$(RM) $(TARGET)$(BINEXT) $(OBJS)
	@echo "Limpeza concluída"

help:
	@echo "Makefile para Planeta do Tesouro"
	@echo ""
	@echo "Comandos disponíveis:"
	@echo "  make          - Compila o jogo"
	@echo "  make run      - Compila e executa o jogo"
	@echo "  make clean    - Remove arquivos compilados"
	@echo "  make help     - Mostra esta ajuda"
	@echo ""