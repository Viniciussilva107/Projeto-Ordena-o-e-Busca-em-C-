#ifndef ANIMAL_H
#define ANIMAL_H

#include <stdio.h>

typedef struct {
    int id;
    char nome[50];
    char especie[20];
    char raca[20];
    int idade;
    int status;
    long prox;
} Animal;

void Animal_Cadastrar();
void Animal_Listar();
long Animal_BuscaSequencial(int idBusca, int *qtdComparacoes);
long Animal_BuscaBinaria(int idBusca, int *qtdComparacoes);
Animal Animal_LerDoDisco(long byteOffset);
void Animal_SetStatusAdotado(long byteOffset);
long Animal_TamanhoArquivo(FILE *fp);
long Animal_OrdenarPorID_Bubble();

#endif
