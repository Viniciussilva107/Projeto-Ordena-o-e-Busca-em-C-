#ifndef ORDENACAO_EXTERNA_H
#define ORDENACAO_EXTERNA_H

#include "animal.h"

#define TAM_MEMORIA 5

typedef struct {
    Animal animal;
    int congelado;
} ElementoMemoria;

typedef struct {
    Animal animal;
    int indiceArquivo;
    int fimDeArquivo;
} NoVencedor;

int GerarParticoes_SelecaoSubstituicao();
void Intercalacao_ArvoreVencedores(int qtdParticoes);
void OrdenacaoExterna_Completa();

#endif
