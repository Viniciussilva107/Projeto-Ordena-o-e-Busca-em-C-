#ifndef HASH_H
#define HASH_H

#include "animal.h"

#define TAM_HASH 101

void Hash_Inicializar();
long Hash_Buscar(int idBusca);
int Hash_Inserir(Animal a);
int Hash_Remover(int idRemove);
void Hash_ListarEspacosLivres();

#endif
