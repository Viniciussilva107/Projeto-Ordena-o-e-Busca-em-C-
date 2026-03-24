#include "animal.h"
#include <stdlib.h>
#include <string.h>

const char *FILE_ANI = "animais.dat";

long Animal_TamanhoArquivo(FILE *fp) {
    long pos = ftell(fp);
    fseek(fp, 0, SEEK_END);
    long tam = ftell(fp);
    fseek(fp, pos, SEEK_SET);
    return tam / sizeof(Animal);
}

void Animal_Cadastrar() {
    FILE *fp = fopen(FILE_ANI, "ab");
    if (!fp) fp = fopen(FILE_ANI, "wb");

    Animal a;
    printf("\n[Animal] ID: "); scanf("%d", &a.id); getchar();
    printf("[Animal] Nome: "); fgets(a.nome, 50, stdin); a.nome[strcspn(a.nome, "\n")] = 0;
    printf("[Animal] Especie: "); fgets(a.especie, 20, stdin); a.especie[strcspn(a.especie, "\n")] = 0;
    printf("[Animal] Raca: "); fgets(a.raca, 20, stdin); a.raca[strcspn(a.raca, "\n")] = 0;
    printf("[Animal] Idade: "); scanf("%d", &a.idade);
    a.status = 0;
    a.prox = -1;

    fwrite(&a, sizeof(Animal), 1, fp);
    fclose(fp);
}

void Animal_Listar() {
    FILE *fp = fopen(FILE_ANI, "rb");
    if (!fp) { printf("Base vazia.\n"); return; }
    Animal a;
    printf("\n--- Lista de Animais ---\n");
    while(fread(&a, sizeof(Animal), 1, fp)) {
        if(a.status != -1)
            printf("ID: %d | Nome: %-10s | Status: %s\n", a.id, a.nome, a.status ? "ADOTADO" : "LIVRE");
    }
    fclose(fp);
}

long Animal_BuscaSequencial(int idBusca, int *qtdComparacoes) {
    FILE *fp = fopen(FILE_ANI, "rb");
    if (!fp) return -1;
    Animal a;
    long pos = -1;
    *qtdComparacoes = 0;
    while(fread(&a, sizeof(Animal), 1, fp)) {
        (*qtdComparacoes)++;
        if (a.id == idBusca && a.status != -1) {
            pos = ftell(fp) - sizeof(Animal);
            break;
        }
    }
    fclose(fp);
    return pos;
}

long Animal_BuscaBinaria(int idBusca, int *qtdComparacoes) {
    FILE *fp = fopen(FILE_ANI, "rb");
    if (!fp) return -1;
    long inicio = 0, fim = Animal_TamanhoArquivo(fp) - 1, meio;
    Animal a;
    long pos = -1;
    *qtdComparacoes = 0;

    while (inicio <= fim) {
        meio = (inicio + fim) / 2;
        fseek(fp, meio * sizeof(Animal), SEEK_SET);
        fread(&a, sizeof(Animal), 1, fp);
        (*qtdComparacoes)++;

        if (a.id == idBusca && a.status != -1) {
            pos = meio * sizeof(Animal);
            break;
        } else if (a.id < idBusca) inicio = meio + 1;
        else fim = meio - 1;
    }
    fclose(fp);
    return pos;
}

Animal Animal_LerDoDisco(long byteOffset) {
    Animal a;
    FILE *fp = fopen(FILE_ANI, "rb");
    fseek(fp, byteOffset, SEEK_SET);
    fread(&a, sizeof(Animal), 1, fp);
    fclose(fp);
    return a;
}

void Animal_SetStatusAdotado(long byteOffset) {
    FILE *fp = fopen(FILE_ANI, "rb+");
    fseek(fp, byteOffset, SEEK_SET);
    Animal a;
    fread(&a, sizeof(Animal), 1, fp);
    a.status = 1;
    fseek(fp, byteOffset, SEEK_SET);
    fwrite(&a, sizeof(Animal), 1, fp);
    fclose(fp);
}

long Animal_OrdenarPorID_Bubble() {
    FILE *fp = fopen(FILE_ANI, "rb+");
    if (!fp) return 0;
    long qtd = Animal_TamanhoArquivo(fp);
    if (qtd < 2) { fclose(fp); return 0; }
    Animal a1, a2;
    int trocou;
    long comps = 0;
    for (long i = 0; i < qtd - 1; i++) {
        trocou = 0;
        for (long j = 0; j < qtd - i - 1; j++) {
            fseek(fp, j * sizeof(Animal), SEEK_SET);
            fread(&a1, sizeof(Animal), 1, fp);
            fread(&a2, sizeof(Animal), 1, fp);
            comps++;
            if (a1.id > a2.id) {
                fseek(fp, j * sizeof(Animal), SEEK_SET);
                fwrite(&a2, sizeof(Animal), 1, fp);
                fwrite(&a1, sizeof(Animal), 1, fp);
                trocou = 1;
            }
        }
        if (!trocou) break;
    }
    fclose(fp);
    return comps;
}
