#include "adocao.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const char *FILE_REL = "adocoes.dat";

void Adocao_Salvar(int idAni, int idAdo, char* data) {
    FILE *fp = fopen(FILE_REL, "ab");
    Adocao nov;
    nov.idAnimal = idAni;
    nov.idAdotante = idAdo;
    strcpy(nov.data, data);
    fwrite(&nov, sizeof(Adocao), 1, fp);
    fclose(fp);
}

void Adocao_ListarRelatorio() {
    FILE *fp = fopen(FILE_REL, "rb");
    if (!fp) { printf("Nenhuma adocao registrada.\n"); return; }
    Adocao a;
    printf("\n--- Relatorio Geral de Adocoes ---\n");
    while(fread(&a, sizeof(Adocao), 1, fp)) {
        printf("Animal ID [%d] -> Adotante ID [%d] em %s\n", a.idAnimal, a.idAdotante, a.data);
    }
    fclose(fp);
}
