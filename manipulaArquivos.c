#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include "includes/inimigo.h"
#include "includes/manipulaArquivos.h"

#define RED_TEXT "\x1b[31m"
#define GREEN_TEXT "\x1b[32m"
#define RESET "\x1b[0m"

bool arquivoExiste(const char *caminho) {
    return access(caminho, F_OK) == 0;
}

int tamanhoMapa(FILE *arq, int *linhas, int *colunas){
    if(fscanf(arq, "%d %d", linhas, colunas) != 2){
        printf(RED_TEXT"Erro ao ler dimensões do mapa.\n"RESET);
        return 0;
    }
    return 1;
}

void mostrarMapa(char **mapa, int linhas){
    for (int i = 0; i < linhas; i++) {
        printf("%s\n", mapa[i]);
    }
}

void processarPortais(Mapa *mapa) {
    mapa->totalPortais = 0;
    
    // Array temporário para armazenar posições de cada portal
    struct {
        int x[2], y[2]; // Máximo 2 posições por número
        int count;
    } temp[10] = {0}; // Para portais '0' a '9'
    
    // Primeira passada: coletar todas as posições dos portais
    for (int i = 0; i < mapa->linhas; i++) {
        for (int j = 0; j < mapa->colunas; j++) {
            char tile = mapa->dados[i][j];
            if (tile >= '1' && tile <= '9') {
                int idx = tile - '0';
                if (temp[idx].count < 2) {
                    temp[idx].x[temp[idx].count] = j;
                    temp[idx].y[temp[idx].count] = i;
                    temp[idx].count++;
                }
            }
        }
    }
    
    // Segunda passada: criar pares válidos (que têm exatamente 2 portais)
    for (int i = 1; i <= 9; i++) {
        if (temp[i].count == 2 && mapa->totalPortais < 9) {
            mapa->portais[mapa->totalPortais].x1 = temp[i].x[0];
            mapa->portais[mapa->totalPortais].y1 = temp[i].y[0];
            mapa->portais[mapa->totalPortais].x2 = temp[i].x[1];
            mapa->portais[mapa->totalPortais].y2 = temp[i].y[1];
            mapa->portais[mapa->totalPortais].numero = '0' + i;
            mapa->totalPortais++;
        }
    }
    
    printf("Portais encontrados: %d pares\n", mapa->totalPortais);
}

void contarElementosMapa(Mapa *mapa, int *totalTesouros, int *totalInimigos, Inimigo **inimigos) {
    *totalTesouros = 0;
    *totalInimigos = 0;
    
    // Garantir que inimigos seja NULL se não houver inimigos
    if (*inimigos != NULL) {
        free(*inimigos);
        *inimigos = NULL;
    }

    for (int i = 0; i < mapa->linhas; i++) {
        for (int j = 0; j < mapa->colunas; j++) {
            if (mapa->dados[i][j] == 'T') {
                (*totalTesouros)++;
            } else if (mapa->dados[i][j] == 'I') {
                (*totalInimigos)++;
                *inimigos = realloc(*inimigos, (*totalInimigos) * sizeof(Inimigo));
                if (*inimigos == NULL) {
                    printf("ERRO: Falha na alocação de memória para inimigos!\n");
                    *totalInimigos = 0;
                    return;
                }
                (*inimigos)[(*totalInimigos) - 1].x = j;
                (*inimigos)[(*totalInimigos) - 1].y = i;
                (*inimigos)[(*totalInimigos) - 1].velocidade = 0.5;
                (*inimigos)[(*totalInimigos) - 1].ultimoMovimento = 0.0;
                (*inimigos)[(*totalInimigos) - 1].direcao = 0; // Iniciar virado para direita
            }
        }
    }
    
    // Processar portais após contar outros elementos
    processarPortais(mapa);
}

void liberaMapa(Mapa *mapa){
    if(mapa == NULL) {
        printf("AVISO: Tentativa de liberar mapa nulo\n");
        return;
    }
    
    if(mapa->dados == NULL) {
        printf("AVISO: Dados do mapa já são nulos\n");
        return;
    }
    
    printf("Liberando mapa %dx%d...\n", mapa->linhas, mapa->colunas);
    
    for(int i = 0; i < mapa->linhas; i++){
        if(mapa->dados[i] != NULL){
            free(mapa->dados[i]);
            mapa->dados[i] = NULL;
        }
    }
    free(mapa->dados);
    mapa->dados = NULL;
    mapa->linhas = 0;
    mapa->colunas = 0;
    mapa->totalTesouros = 0;
    mapa->totalInimigos = 0;
    mapa->totalPortais = 0;
    
    // Limpar array de portais
    for(int i = 0; i < 9; i++){
        mapa->portais[i].x1 = 0;
        mapa->portais[i].y1 = 0;
        mapa->portais[i].x2 = 0;
        mapa->portais[i].y2 = 0;
        mapa->portais[i].numero = '0';
    }

    printf(GREEN_TEXT "Memória do mapa liberada com sucesso.\n" RESET);
}

Mapa carregaMapa(int fase, Inimigo **inimigos) {
    Mapa mapa = {0}; // Inicialização completa com zeros
    char caminho[50];
    snprintf(caminho, sizeof(caminho), "mapas/mapa%d.txt", fase);

    // Verifica se o arquivo existe antes de tentar abrir
    if (arquivoExiste(caminho)) {
        printf(GREEN_TEXT "Arquivo '%s' encontrado.\n Carregando mapa...\n" RESET, caminho);
    } else {
        printf(RED_TEXT "Acabou as fases ou o arquivo '%s' não foi encontrado.\n" RESET, caminho);
        Mapa mapaVazio = {0};
        return mapaVazio;
    }

    FILE *arq = fopen(caminho, "r");
    if (arq == NULL) {
        printf(RED_TEXT "Erro ao abrir o arquivo '%s'.\n" RESET, caminho);
        printf("O arquivo existe mas não pode ser lido.\n");
        return mapa;
    }

    // Captura as dimensões da primeira linha
    if(tamanhoMapa(arq, &mapa.linhas, &mapa.colunas)){
        printf("Mapa carregado: %d linhas x %d colunas\n", mapa.linhas, mapa.colunas);
        
        // Validar tamanho máximo permitido
        if(mapa.linhas > MAX_LINHAS || mapa.colunas > MAX_COLUNAS){
            printf(RED_TEXT "ERRO: Mapa muito grande! Máximo permitido: %dx%d\n" RESET, MAX_LINHAS, MAX_COLUNAS);
            printf("Tamanho atual: %dx%d\n", mapa.linhas, mapa.colunas);
            fclose(arq);
            Mapa mapaVazio = {0};
            return mapaVazio;
        }
        
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

    contarElementosMapa(&mapa, &mapa.totalTesouros, &mapa.totalInimigos, inimigos);
    printf("Total de Tesouros: %d\n", mapa.totalTesouros);
    printf("Total de Inimigos: %d\n", mapa.totalInimigos);
    
    fclose(arq);

    return mapa;
}

void listarMapasDisponiveis(void) {
    printf("Verificando mapas disponíveis:\n");
    
    for (int i = 1; i <= 10; i++) {
        char caminho[50];
        snprintf(caminho, sizeof(caminho), "mapas/mapa%d.txt", i);
        
        if (arquivoExiste(caminho)) {
            printf(GREEN_TEXT "✓ %s - Disponível\n" RESET, caminho);
        } else {
            printf("✗ %s - Não encontrado\n", caminho);
        }
    }
    printf("\n");
}


void iniciarCronometro(Cronometro *cronometro) {
    cronometro->inicioJogo = GetTime();
    cronometro->faseAtual = 0;
    cronometro->tempoTotal = 0.0;
    
    // Limpar tempos das fases
    for (int i = 0; i < MAX_FASES; i++) {
        cronometro->temposPorFase[i] = 0.0;
    }
    
    printf("Cronômetro iniciado.\n");
}

void iniciarFase(Cronometro *cronometro, int numeroFase) {
    tocarSomStart();
    cronometro->inicioFase = GetTime();
    cronometro->faseAtual = numeroFase;
}

void finalizarFase(Cronometro *cronometro) {
    if (cronometro->faseAtual > 0 && cronometro->faseAtual <= MAX_FASES) {
        cronometro->fimFase = GetTime();
        cronometro->tempoFase = cronometro->fimFase - cronometro->inicioFase;
        cronometro->temposPorFase[cronometro->faseAtual - 1] = cronometro->tempoFase;
        
        printf("Fase %d completa em %.2f segundos.\n", 
               cronometro->faseAtual, cronometro->tempoFase);
    }
}

void finalizarJogo(Cronometro *cronometro) {
    double tempoFinal = GetTime();
    cronometro->tempoTotal = tempoFinal - cronometro->inicioJogo;
    
    printf(GREEN_TEXT "\nJogo completo!\n" RESET);
    printf("Tempo total: %.2f segundos\n", cronometro->tempoTotal);
    
    // Exibir tempos por fase
    printf("\nTempos por fase:\n");
    for (int i = 0; i < MAX_FASES; i++) {
        if (cronometro->temposPorFase[i] > 0) {
            printf("   Fase %d: %.2f segundos\n", i + 1, cronometro->temposPorFase[i]);
        }
    }
}

void salvarRecorde(Cronometro *cronometro, const char *nomeJogador) {
    Recorde novoRecorde = {0};
    
    // Usar o nome fornecido ou nome padrão se estiver vazio
    if (nomeJogador != NULL && strlen(nomeJogador) > 0) {
        strncpy(novoRecorde.nomeJogador, nomeJogador, MAX_NOME - 1);
        novoRecorde.nomeJogador[MAX_NOME - 1] = '\0'; // Garantir terminação
    } else {
        time_t agora = time(NULL);
        snprintf(novoRecorde.nomeJogador, MAX_NOME, "Jogador_%ld", agora % 10000);
    }
    
    // Copiar dados do cronômetro
    novoRecorde.tempoTotal = cronometro->tempoTotal;
    for (int i = 0; i < MAX_FASES; i++) {
        novoRecorde.temposPorFase[i] = cronometro->temposPorFase[i];
        if (cronometro->temposPorFase[i] > 0) {
            novoRecorde.fasesCompletas = i + 1;
        }
    }
    
    // Data e hora atual
    time_t agora = time(NULL);
    strftime(novoRecorde.dataHora, sizeof(novoRecorde.dataHora), 
             "%d/%m/%Y %H:%M:%S", localtime(&agora));
    
    // Salvar no arquivo binário
    FILE *arquivo = fopen("ranking.bin", "ab");
    if (arquivo != NULL) {
        fwrite(&novoRecorde, sizeof(Recorde), 1, arquivo);
        fclose(arquivo);
    }
}

void exibirRanking(void) {
    FILE *arquivo = fopen("ranking.bin", "rb");
    if (arquivo == NULL) {
        printf("Nenhum tempo registrado ainda.\n");
        return;
    }
    
    Recorde recordes[MAX_RECORDES];
    int totalRecordes = 0;
    
    // Ler todos os recordes do arquivo binário
    while (fread(&recordes[totalRecordes], sizeof(Recorde), 1, arquivo) == 1 && 
           totalRecordes < MAX_RECORDES) {
        totalRecordes++;
    }
    
    fclose(arquivo);
    
    // Ordenar por tempo (bubble sort simples)
    for (int i = 0; i < totalRecordes - 1; i++) {
        for (int j = 0; j < totalRecordes - i - 1; j++) {
            if (recordes[j].tempoTotal > recordes[j + 1].tempoTotal) {
                Recorde temp = recordes[j];
                recordes[j] = recordes[j + 1];
                recordes[j + 1] = temp;
            }
        }
    }
    
    // Exibir ranking completo
    printf(GREEN_TEXT "\nRANKING DE TEMPOS - PLANETA DO TESOURO\n" RESET);
    printf("═══════════════════════════════════════════════════════════════\n");
    printf("Pos | Nome              | Tempo Total | Fases | Data/Hora        \n");
    printf("════════════════════════════════════════════════════════════════\n");
    
    for (int i = 0; i < totalRecordes && i < 10; i++) {
        printf("%2d  | %-16s | %8.2fs   |  %2d   | %s\n", 
               i + 1, 
               recordes[i].nomeJogador,
               recordes[i].tempoTotal,
               recordes[i].fasesCompletas,
               recordes[i].dataHora);
    }
    
    if (totalRecordes > 0) {
        printf("════════════════════════════════════════════════════════════════\n");
        
        // Exibir detalhes do melhor tempo
        printf("\n🏆 MELHOR TEMPO - DETALHES:\n");
        printf("Jogador: %s\n", recordes[0].nomeJogador);
        printf("Tempo Total: %.2f segundos\n", recordes[0].tempoTotal);
        printf("Fases Completas: %d\n", recordes[0].fasesCompletas);
        printf("Data: %s\n", recordes[0].dataHora);
        
        printf("\nTempos por fase:\n");
        for (int i = 0; i < recordes[0].fasesCompletas; i++) {
            if (recordes[0].temposPorFase[i] > 0) {
                printf("   Fase %d: %.2f segundos\n", i + 1, recordes[0].temposPorFase[i]);
            }
        }
        
        // Calcular estatísticas gerais
        if (totalRecordes >= 3) {
            double tempoMedio = 0;
            for (int i = 0; i < (totalRecordes > 5 ? 5 : totalRecordes); i++) {
                tempoMedio += recordes[i].tempoTotal;
            }
            tempoMedio /= (totalRecordes > 5 ? 5 : totalRecordes);
            
            printf("\n📊 ESTATÍSTICAS DO TOP %d:\n", (totalRecordes > 5 ? 5 : totalRecordes));
            printf("Tempo médio: %.2f segundos\n", tempoMedio);
            printf("Melhor tempo: %.2f segundos (%s)\n", recordes[0].tempoTotal, recordes[0].nomeJogador);
            if (totalRecordes > 1) {
                printf("Diferença para 2º lugar: %.2f segundos\n", 
                       recordes[1].tempoTotal - recordes[0].tempoTotal);
            }
        }
    }
    
    printf("═══════════════════════════════════════════════════════════════\n");
}

void gerarRelatorioSpeedrun(Cronometro *cronometro) {
    FILE *relatorio = fopen("relatorio_ultimo_jogo.txt", "w");
    if (relatorio == NULL) {
        printf("Erro ao gerar relatório.\n");
        return;
    }
    
    time_t agora = time(NULL);
    char dataHora[100];
    strftime(dataHora, sizeof(dataHora), "%d/%m/%Y às %H:%M:%S", localtime(&agora));
    
    fprintf(relatorio, "RELATÓRIO DE JOGO - PLANETA DO TESOURO\n");
    fprintf(relatorio, "═══════════════════════════════════════════════════\n\n");
    fprintf(relatorio, "Data/Hora: %s\n\n", dataHora);
    
    fprintf(relatorio, "RESUMO GERAL:\n");
    fprintf(relatorio, "   Tempo Total: %.2f segundos\n", cronometro->tempoTotal);
    fprintf(relatorio, "   Fases Completas: %d\n", cronometro->faseAtual);
    
    double tempoMedio = 0;
    int fasesComTempo = 0;
    
    fprintf(relatorio, "\nDETALHES POR FASE:\n");
    for (int i = 0; i < MAX_FASES; i++) {
        if (cronometro->temposPorFase[i] > 0) {
            fprintf(relatorio, "   Fase %d: %.2f segundos\n", i + 1, cronometro->temposPorFase[i]);
            tempoMedio += cronometro->temposPorFase[i];
            fasesComTempo++;
        }
    }
    
    if (fasesComTempo > 0) {
        tempoMedio /= fasesComTempo;
        fprintf(relatorio, "\nESTATÍSTICAS:\n");
        fprintf(relatorio, "   Tempo médio por fase: %.2f segundos\n", tempoMedio);
        
        // Encontrar melhor e pior fase
        double melhorTempo = cronometro->temposPorFase[0];
        double piorTempo = cronometro->temposPorFase[0];
        int melhorFase = 1, piorFase = 1;
        
        for (int i = 0; i < MAX_FASES; i++) {
            if (cronometro->temposPorFase[i] > 0) {
                if (cronometro->temposPorFase[i] < melhorTempo) {
                    melhorTempo = cronometro->temposPorFase[i];
                    melhorFase = i + 1;
                }
                if (cronometro->temposPorFase[i] > piorTempo) {
                    piorTempo = cronometro->temposPorFase[i];
                    piorFase = i + 1;
                }
            }
        }
        
        fprintf(relatorio, "   Melhor fase: Fase %d (%.2f segundos)\n", melhorFase, melhorTempo);
        fprintf(relatorio, "   Pior fase: Fase %d (%.2f segundos)\n", piorFase, piorTempo);
    }
    
    fprintf(relatorio, "\nObrigado por jogar!\n");
    fprintf(relatorio, "═══════════════════════════════════════════════════\n");
    
    fclose(relatorio);
    
    printf("Relatório salvo em 'relatorio_ultimo_jogo.txt'.\n");
}