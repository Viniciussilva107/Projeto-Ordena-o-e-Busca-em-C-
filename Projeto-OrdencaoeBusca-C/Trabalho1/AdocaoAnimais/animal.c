#include "animal.h"
#include <stdlib.h>
#include <string.h>

const char *FILE_ANI = "animais.dat";

// Descobre quantos animais existem
long Animal_TamanhoArquivo(FILE *fp) {
    fseek(fp, 0, SEEK_END);
    long tam = ftell(fp);
    return tam / sizeof(Animal);
}

void Animal_Cadastrar() {
    FILE *fp = fopen(FILE_ANI, "ab");
    if (!fp) fp = fopen(FILE_ANI, "wb");

    Animal a;
    printf("\n[Animal] ID: "); scanf("%d", &a.id);
    getchar();
    printf("[Animal] Nome: "); fgets(a.nome, 50, stdin); a.nome[strcspn(a.nome, "\n")] = 0;
    printf("[Animal] Especie: "); fgets(a.especie, 20, stdin); a.especie[strcspn(a.especie, "\n")] = 0;
    printf("[Animal] Raca: "); fgets(a.raca, 20, stdin); a.raca[strcspn(a.raca, "\n")] = 0;
    printf("[Animal] Idade: "); scanf("%d", &a.idade);
    a.status = 0;

    fwrite(&a, sizeof(Animal), 1, fp);
    fclose(fp);
}

void Animal_Listar() {
    FILE *fp = fopen(FILE_ANI, "rb");
    if (!fp) { printf("Arquivo vazio ou inexistente.\n"); return; }
    Animal a;
    printf("\n--- Lista de Animais ---\n");
    while(fread(&a, sizeof(Animal), 1, fp)) {
        printf("ID: %d | Nome: %-15s | Status: %s\n", a.id, a.nome, a.status ? "ADOTADO" : "LIVRE");
    }
    fclose(fp);
}

// Bubble Sort em Disco com contador de comparações
long Animal_OrdenarPorID() {
    FILE *fp = fopen(FILE_ANI, "rb+");
    if (!fp) return 0;

    long qtd = Animal_TamanhoArquivo(fp);
    if (qtd < 2) { fclose(fp); return 0; }

    Animal a1, a2;
    int trocou;
    long contadorComparacoes = 0;

    printf("Ordenando em disco... aguarde...\n");

    for (long i = 0; i < qtd - 1; i++) {
        trocou = 0;
        for (long j = 0; j < qtd - i - 1; j++) {
            // Lê o par J e J+1
            fseek(fp, j * sizeof(Animal), SEEK_SET);
            fread(&a1, sizeof(Animal), 1, fp);
            fread(&a2, sizeof(Animal), 1, fp);

            contadorComparacoes++; // Conta a comparação do if abaixo

            if (a1.id > a2.id) {
                // Troca posições no disco
                fseek(fp, j * sizeof(Animal), SEEK_SET);
                fwrite(&a2, sizeof(Animal), 1, fp); // Escreve menor
                fwrite(&a1, sizeof(Animal), 1, fp); // Escreve maior
                trocou = 1;
            }
        }
        if (!trocou) break;
    }
    fclose(fp);
    return contadorComparacoes;
}

long Animal_BuscaSequencial(int idBusca) {
    FILE *fp = fopen(FILE_ANI, "rb");
    if (!fp) return -1;
    Animal a;
    long pos = -1;
    while(fread(&a, sizeof(Animal), 1, fp)) {
        if (a.id == idBusca) {
            pos = ftell(fp) - sizeof(Animal);
            break;
        }
    }
    fclose(fp);
    return pos;
}

// Busca Binária
long Animal_BuscaBinaria(int idBusca, int *qtdComparacoes) {
    FILE *fp = fopen(FILE_ANI, "rb");
    if (!fp) return -1;

    long inicio = 0, fim = Animal_TamanhoArquivo(fp) - 1, meio;
    Animal a;
    long pos = -1;
    *qtdComparacoes = 0; // Zera o contador externo

    while (inicio <= fim) {
        meio = (inicio + fim) / 2;

        // Pula direto para o registro do meio
        fseek(fp, meio * sizeof(Animal), SEEK_SET);
        fread(&a, sizeof(Animal), 1, fp);

        (*qtdComparacoes)++; // Incrementa comparação

        if (a.id == idBusca) {
            pos = meio * sizeof(Animal);
            break;
        } else if (a.id < idBusca) {
            inicio = meio + 1;
        } else {
            fim = meio - 1;
        }
    }
    fclose(fp);
    return pos;
}

void Animal_SetStatusAdotado(long byteOffset) {
    FILE *fp = fopen(FILE_ANI, "rb+");
    fseek(fp, byteOffset, SEEK_SET);
    Animal a;
    fread(&a, sizeof(Animal), 1, fp);
    a.status = 1; // Muda status
    fseek(fp, byteOffset, SEEK_SET);
    fwrite(&a, sizeof(Animal), 1, fp); // Reescreve
    fclose(fp);
}

Animal Animal_LerDoDisco(long byteOffset) {
    Animal a;
    FILE *fp = fopen(FILE_ANI, "rb");
    fseek(fp, byteOffset, SEEK_SET);
    fread(&a, sizeof(Animal), 1, fp);
    fclose(fp);
    return a;
}
