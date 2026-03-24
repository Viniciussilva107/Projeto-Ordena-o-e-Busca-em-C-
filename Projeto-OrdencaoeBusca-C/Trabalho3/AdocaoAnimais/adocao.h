#ifndef ADOCAO_H
#define ADOCAO_H

typedef struct {
    int idAnimal;
    int idAdotante;
    char data[12];
} Adocao;

void Adocao_Salvar(int idAni, int idAdo, char* data);
void Adocao_ListarRelatorio();

#endif
