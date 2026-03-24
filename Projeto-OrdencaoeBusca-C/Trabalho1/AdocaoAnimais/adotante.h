#ifndef ADOTANTE_H
#define ADOTANTE_H

typedef struct {
    int id;
    char nome[50];
    char contato[50];
} Adotante;

void Adotante_Cadastrar();
void Adotante_Listar();
long Adotante_BuscaSequencial(int idBusca);

#endif
