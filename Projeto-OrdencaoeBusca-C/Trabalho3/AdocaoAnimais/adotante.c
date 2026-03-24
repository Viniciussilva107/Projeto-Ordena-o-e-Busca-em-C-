#include "adotante.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const char *FILE_ADO = "adotantes.dat";

void Adotante_Cadastrar() {
    FILE *fp = fopen(FILE_ADO, "ab");
    if (!fp) fp = fopen(FILE_ADO, "wb");
    Adotante ad;
    printf("\n[Adotante] ID: "); scanf("%d", &ad.id); getchar();
    printf("[Adotante] Nome: "); fgets(ad.nome, 50, stdin); ad.nome[strcspn(ad.nome, "\n")] = 0;
    printf("[Adotante] Contato: "); fgets(ad.contato, 50, stdin); ad.contato[strcspn(ad.contato, "\n")] = 0;
    fwrite(&ad, sizeof(Adotante), 1, fp);
    fclose(fp);
}

void Adotante_Listar() {
    FILE *fp = fopen(FILE_ADO, "rb");
    if (!fp) return;
    Adotante ad;
    printf("\n--- Lista de Adotantes ---\n");
    while(fread(&ad, sizeof(Adotante), 1, fp)) {
        printf("ID: %d | Nome: %s\n", ad.id, ad.nome);
    }
    fclose(fp);
}

long Adotante_BuscaSequencial(int idBusca) {
    FILE *fp = fopen(FILE_ADO, "rb");
    if (!fp) return -1;
    Adotante ad;
    long pos = -1;
    while(fread(&ad, sizeof(Adotante), 1, fp)) {
        if (ad.id == idBusca) {
            pos = ftell(fp) - sizeof(Adotante);
            break;
        }
    }
    fclose(fp);
    return pos;
}

void Adotante_GerarMassaTeste(int quantidade) {
    FILE *fp = fopen(FILE_ADO, "wb");
    if (!fp) return;
    Adotante ad;
    for(int i = 0; i < quantidade; i++) {
        ad.id = rand() % (quantidade * 10);
        sprintf(ad.nome, "Adotante_%d", i);
        sprintf(ad.contato, "email_%d@teste.com", i);
        fwrite(&ad, sizeof(Adotante), 1, fp);
    }
    fclose(fp);
    printf("Concluido! %d adotantes gerados.\n", quantidade);
}
