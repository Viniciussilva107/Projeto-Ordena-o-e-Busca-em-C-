#include "ordenacao_externa.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

const char *ARQ_ENTRADA = "animais.dat";
const char *ARQ_SAIDA_FINAL = "animais_ordenado_ext.dat";

void RefazHeap(ElementoMemoria *v, int esq, int dir) {
    int i = esq;
    int j = i * 2 + 1;
    ElementoMemoria aux = v[i];
    while (j <= dir) {
        if (j < dir) {
            if (v[j].congelado < v[j+1].congelado ||
               (v[j].congelado == v[j+1].congelado && v[j].animal.id > v[j+1].animal.id)) {
                j++;
            }
        }
        if (aux.congelado < v[j].congelado ||
           (aux.congelado == v[j].congelado && aux.animal.id <= v[j].animal.id)) {
            break;
        }
        v[i] = v[j];
        i = j;
        j = i * 2 + 1;
    }
    v[i] = aux;
}

void ConstroiHeap(ElementoMemoria *v, int n) {
    int esq = (n / 2) - 1;
    while (esq >= 0) {
        RefazHeap(v, esq, n - 1);
        esq--;
    }
}

int GerarParticoes_SelecaoSubstituicao() {
    FILE *fp = fopen(ARQ_ENTRADA, "rb");
    if (!fp) return 0;

    int qtdParticoes = 0;
    char nomeParticao[20];
    FILE *out = NULL;
    ElementoMemoria memoria[TAM_MEMORIA];
    int n = 0, congelados = 0;

    Animal aux;
    while (n < TAM_MEMORIA && fread(&aux, sizeof(Animal), 1, fp)) {
        memoria[n].animal = aux;
        memoria[n].congelado = 0;
        n++;
    }
    ConstroiHeap(memoria, n);

    while (n > 0) {
        if (!out || congelados == n) {
            if (out) fclose(out);
            if (congelados == n) {
                for (int i = 0; i < n; i++) memoria[i].congelado = 0;
                congelados = 0;
                ConstroiHeap(memoria, n);
            }
            sprintf(nomeParticao, "particao_%d.dat", qtdParticoes);
            out = fopen(nomeParticao, "wb");
            qtdParticoes++;
        }

        Animal menor = memoria[0].animal;
        fwrite(&menor, sizeof(Animal), 1, out);

        if (fread(&aux, sizeof(Animal), 1, fp)) {
            memoria[0].animal = aux;
            if (aux.id < menor.id) {
                memoria[0].congelado = 1;
                congelados++;
            } else {
                memoria[0].congelado = 0;
            }
        } else {
            memoria[0] = memoria[n-1];
            n--;
            if (n > 0 && memoria[0].congelado) congelados--;
        }
        if (n > 0) RefazHeap(memoria, 0, n-1);
    }
    if (out) fclose(out);
    fclose(fp);
    return qtdParticoes;
}

void Intercalacao_ArvoreVencedores(int qtdParticoes) {
    if (qtdParticoes == 0) return;
    FILE **arquivos = (FILE**) malloc(qtdParticoes * sizeof(FILE*));
    char nomeParticao[20];
    for (int i = 0; i < qtdParticoes; i++) {
        sprintf(nomeParticao, "particao_%d.dat", i);
        arquivos[i] = fopen(nomeParticao, "rb");
    }
    FILE *saida = fopen(ARQ_SAIDA_FINAL, "wb");
    NoVencedor *folhas = (NoVencedor*) malloc(qtdParticoes * sizeof(NoVencedor));

    for (int i = 0; i < qtdParticoes; i++) {
        if (fread(&folhas[i].animal, sizeof(Animal), 1, arquivos[i])) {
            folhas[i].indiceArquivo = i;
            folhas[i].fimDeArquivo = 0;
        } else {
            folhas[i].fimDeArquivo = 1;
            folhas[i].animal.id = INT_MAX;
        }
    }
    int arquivosAbertos = qtdParticoes;
    while (arquivosAbertos > 0) {
        int idxVencedor = -1;
        int menorID = INT_MAX;
        for (int i = 0; i < qtdParticoes; i++) {
            if (!folhas[i].fimDeArquivo && folhas[i].animal.id < menorID) {
                menorID = folhas[i].animal.id;
                idxVencedor = i;
            }
        }
        if (idxVencedor == -1) break;

        fwrite(&folhas[idxVencedor].animal, sizeof(Animal), 1, saida);

        if (!fread(&folhas[idxVencedor].animal, sizeof(Animal), 1, arquivos[idxVencedor])) {
            folhas[idxVencedor].fimDeArquivo = 1;
            folhas[idxVencedor].animal.id = INT_MAX;
            arquivosAbertos--;
        }
    }
    fclose(saida);
    for (int i = 0; i < qtdParticoes; i++) {
        fclose(arquivos[i]);
        sprintf(nomeParticao, "particao_%d.dat", i);
        remove(nomeParticao);
    }
    free(arquivos);
    free(folhas);
    remove(ARQ_ENTRADA);
    rename(ARQ_SAIDA_FINAL, ARQ_ENTRADA);
}

void OrdenacaoExterna_Completa() {
    printf("--- Iniciando Ordenacao Externa ---\n");
    int qtd = GerarParticoes_SelecaoSubstituicao();
    printf("   -> Foram geradas %d particoes.\n", qtd);
    Intercalacao_ArvoreVencedores(qtd);
    printf("--- Ordenacao Externa Concluida ---\n");
}
