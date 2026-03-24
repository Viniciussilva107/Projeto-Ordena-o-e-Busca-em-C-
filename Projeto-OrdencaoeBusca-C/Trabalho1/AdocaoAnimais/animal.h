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
} Animal;

void Animal_Cadastrar();
void Animal_Listar();

long Animal_OrdenarPorID();

long Animal_BuscaSequencial(int idBusca);

long Animal_BuscaBinaria(int idBusca, int *qtdComparacoes);

void Animal_SetStatusAdotado(long byteOffset);
Animal Animal_LerDoDisco(long byteOffset);

#endif
