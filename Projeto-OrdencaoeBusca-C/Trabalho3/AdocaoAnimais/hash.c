#include "hash.h"
#include <stdio.h>
#include <stdlib.h>

const char *ARQ_DADOS_H = "animais.dat";
const char *ARQ_HASH = "hash.dat";
const char *ARQ_LED = "led.dat";

long LerCabecalhoLED() {
    FILE *f = fopen(ARQ_LED, "rb");
    long topo = -1;
    if (f) { fread(&topo, sizeof(long), 1, f); fclose(f); }
    return topo;
}

void SalvarCabecalhoLED(long offset) {
    FILE *f = fopen(ARQ_LED, "wb");
    fwrite(&offset, sizeof(long), 1, f);
    fclose(f);
}

long LerIndiceHash(int h) {
    FILE *f = fopen(ARQ_HASH, "rb");
    long offset = -1;
    if(f){
        fseek(f, h * sizeof(long), SEEK_SET);
        fread(&offset, sizeof(long), 1, f);
        fclose(f);
    }
    return offset;
}

void SalvarIndiceHash(int h, long offset) {
    FILE *f = fopen(ARQ_HASH, "rb+");
    if(f){
        fseek(f, h * sizeof(long), SEEK_SET);
        fwrite(&offset, sizeof(long), 1, f);
        fclose(f);
    }
}

void Hash_Inicializar() {
    FILE *f1 = fopen(ARQ_DADOS_H, "ab"); if (f1) fclose(f1);

    FILE *f2 = fopen(ARQ_HASH, "rb");
    if (!f2) {
        f2 = fopen(ARQ_HASH, "wb");
        long vazio = -1;
        for (int i = 0; i < TAM_HASH; i++) fwrite(&vazio, sizeof(long), 1, f2);
    }
    if (f2) fclose(f2);

    FILE *f3 = fopen(ARQ_LED, "rb");
    if (!f3) {
        f3 = fopen(ARQ_LED, "wb");
        long vazio = -1;
        fwrite(&vazio, sizeof(long), 1, f3);
    }
    if (f3) fclose(f3);
}

long Hash_Buscar(int idBusca) {
    int h = idBusca % TAM_HASH;
    long offset_atual = LerIndiceHash(h);

    while (offset_atual != -1) {
        Animal a = Animal_LerDoDisco(offset_atual);
        if (a.id == idBusca && a.status != -1) return offset_atual;
        offset_atual = a.prox;
    }
    return -1;
}

int Hash_Inserir(Animal a) {
    if (Hash_Buscar(a.id) != -1) return 0;

    long offset_livre;
    long cabecalho_led = LerCabecalhoLED();

    if (cabecalho_led != -1) {
        offset_livre = cabecalho_led;
        Animal removido = Animal_LerDoDisco(offset_livre);
        SalvarCabecalhoLED(removido.prox);
    } else {
        FILE *fa = fopen(ARQ_DADOS_H, "ab");
        fseek(fa, 0, SEEK_END);
        offset_livre = ftell(fa);
        fclose(fa);
    }

    int h = a.id % TAM_HASH;
    long topo_lista = LerIndiceHash(h);
    a.prox = topo_lista;
    a.status = 0;

    FILE *fa = fopen(ARQ_DADOS_H, "rb+");
    fseek(fa, offset_livre, SEEK_SET);
    fwrite(&a, sizeof(Animal), 1, fa);
    fclose(fa);

    SalvarIndiceHash(h, offset_livre);
    return 1;
}

int Hash_Remover(int idRemove) {
    int h = idRemove % TAM_HASH;
    long offset_atual = LerIndiceHash(h);
    long offset_anterior = -1;

    while (offset_atual != -1) {
        Animal a = Animal_LerDoDisco(offset_atual);

        if (a.id == idRemove && a.status != -1) {
            if (offset_anterior == -1) {
                SalvarIndiceHash(h, a.prox);
            } else {
                Animal ant = Animal_LerDoDisco(offset_anterior);
                ant.prox = a.prox;
                FILE *fa = fopen(ARQ_DADOS_H, "rb+");
                fseek(fa, offset_anterior, SEEK_SET);
                fwrite(&ant, sizeof(Animal), 1, fa);
                fclose(fa);
            }

            long cabecalho_led = LerCabecalhoLED();
            a.status = -1;
            a.prox = cabecalho_led;

            FILE *fa2 = fopen(ARQ_DADOS_H, "rb+");
            fseek(fa2, offset_atual, SEEK_SET);
            fwrite(&a, sizeof(Animal), 1, fa2);
            fclose(fa2);

            SalvarCabecalhoLED(offset_atual);
            return 1;
        }
        offset_anterior = offset_atual;
        offset_atual = a.prox;
    }
    return 0;
}

void Hash_ListarEspacosLivres() {
    long offset = LerCabecalhoLED();
    printf("\n--- Lista de Espacos Disponiveis (LED) ---\n");
    if (offset == -1) {
        printf("Nenhum espaco vazio. Proxima insercao no fim do arquivo.\n");
        return;
    }
    int buracos = 0;
    while (offset != -1) {
        Animal a = Animal_LerDoDisco(offset);
        printf("-> Buraco no byte: %ld\n", offset);
        offset = a.prox;
        buracos++;
    }
    printf("Total de espacos reaproveitaveis: %d\n", buracos);
}
