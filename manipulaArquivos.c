#include <stdio.h>
#include <stdlib.h>
#include "includes/manipulaArquivos.h"

#define RED "\x1b[31m"
#define GREEN "\x1b[32m"

int tamanhoMapa(FILE *arq, int *linhas, int *colunas, int *totalTesouros){
    if(fscanf(arq, "%d %d %d", linhas, colunas, totalTesouros) != 3){
        printf(RED"Erro ao ler dimensões e quantidade de tesouros do mapa.\n");
        return 0;
    }
    return 1;
}

void mostrarMapa(char **mapa, int linhas){
    for (int i = 0; i < linhas; i++) {
        printf("%s\n", mapa[i]);
    }
}

void liberaMapa(Mapa *mapa){
    if(mapa == NULL || mapa->dados == NULL) return;
    
    for(int i = 0; i < mapa->linhas; i++){
        free(mapa->dados[i]);
    }
    free(mapa->dados);
    mapa->dados = NULL;
    mapa->linhas = 0;
    mapa->colunas = 0;
    mapa->totalTesouros = 0;

    printf(GREEN "Memória do mapa liberada com sucesso.\n");
}

Mapa carregaMapa(){
    Mapa mapa = {NULL, 0, 0, 0};

    FILE *arq = fopen("mapas/mapa2.txt", "r");
    if (arq == NULL) {
        printf(RED "Erro ao abrir o arquivo de mapa.\n");
        return mapa;
    }

    // Captura as dimensões e quantidade de tesouros da primeira linha
    if(tamanhoMapa(arq, &mapa.linhas, &mapa.colunas, &mapa.totalTesouros)){
        printf("Mapa carregado: %d linhas x %d colunas, %d tesouros\n", mapa.linhas, mapa.colunas, mapa.totalTesouros);
        
        mapa.dados = malloc(mapa.linhas * sizeof(char*));
        for(int i = 0; i < mapa.linhas; i++){
            mapa.dados[i] = malloc((mapa.colunas + 1) * sizeof(char));
        }

        // Lê o mapa linha por linha
        for(int i = 0; i < mapa.linhas; i++){
            fscanf(arq, "%s", mapa.dados[i]);
        }

        mostrarMapa(mapa.dados, mapa.linhas);
    }
    
    fclose(arq);

    return mapa;
}