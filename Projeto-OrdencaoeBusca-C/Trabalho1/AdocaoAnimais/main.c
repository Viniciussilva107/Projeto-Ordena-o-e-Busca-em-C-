#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "animal.h"
#include "adotante.h"
#include "adocao.h"

void GerarMassaDeTeste() {
    FILE *fp = fopen("animais.dat", "wb");
    Animal a;

    printf("Gerando 50 animais aleatorios...\n");
    for (int i = 0; i < 50; i++) {
        a.id = rand() % 1000;
        sprintf(a.nome, "Animal_Teste_%d", i);
        sprintf(a.especie, "Generico");
        sprintf(a.raca, "SRD");
        a.idade = i % 15;
        a.status = 0;

        fwrite(&a, sizeof(Animal), 1, fp);
    }
    fclose(fp);
    printf("Sucesso! 50 animais gerados (desordenados).\n");
}

 //Escreve no log.txt
void RegistrarLog(const char* mensagem) {
    FILE *fp = fopen("log.txt", "a");
    if (!fp) return;

    time_t t = time(NULL);
    struct tm tm = *localtime(&t);

    fprintf(fp, "[%02d/%02d/%d %02d:%02d:%02d] %s\n",
            tm.tm_mday, tm.tm_mon + 1, tm.tm_year + 1900,
            tm.tm_hour, tm.tm_min, tm.tm_sec,
            mensagem);
    fclose(fp);
}

void LimparLog() {
    FILE *fp = fopen("log.txt", "w");

    if (fp == NULL) {
        printf("Erro ao tentar limpar o log.\n");
        return;
    }

    fprintf(fp, "--- LOG REINICIADO ---\n");

    fclose(fp);
    printf("\n>>> Arquivo de Log foi limpo com sucesso! <<<\n");
}

void Controlador_RealizarAdocao() {
    int idAni, idAdo;
    char data[12];
    char logMsg[200];

    printf("\n>>> Processo de Adocao <<<\n");

    printf("ID do Animal: "); scanf("%d", &idAni);
    long posAni = Animal_BuscaSequencial(idAni);

    if (posAni == -1) {
        printf("Erro: Animal nao encontrado.\n");
        return;
    }

    Animal aniObj = Animal_LerDoDisco(posAni);
    if (aniObj.status == 1) {
        printf("Erro: O animal '%s' ja foi adotado!\n", aniObj.nome);
        sprintf(logMsg, "FALHA ADOCAO: Tentativa de adotar animal ID %d ja adotado.", idAni);
        RegistrarLog(logMsg);
        return;
    }

    printf("Animal: %s. ID do Adotante: ", aniObj.nome);
    scanf("%d", &idAdo);

    long posAdo = Adotante_BuscaSequencial(idAdo);
    if (posAdo == -1) {
        printf("Erro: Adotante nao cadastrado.\n");
        return;
    }

    printf("Data (DD/MM/AAAA): ");
    getchar(); fgets(data, 12, stdin); data[strcspn(data, "\n")] = 0;

    Adocao_Salvar(idAni, idAdo, data);
    Animal_SetStatusAdotado(posAni);

    printf("Sucesso! Adoção realizada.\n");

    sprintf(logMsg, "SUCESSO ADOCAO: Animal %d adotado por %d.", idAni, idAdo);
    RegistrarLog(logMsg);
}

int main() {
    int op;
    char logMsg[256];

    do {
        printf("\n=== SISTEMA DE ADOCAO ===\n");
        printf("1. Cadastrar Animal\n");
        printf("2. Cadastrar Adotante\n");
        printf("3. Listar Bases\n");
        printf("4. Ordenar Animais (Log de Performance)\n");
        printf("5. Buscar Animal Binaria (Log de Performance)\n");
        printf("6. Realizar Adocao\n");
        printf("7. Relatorio de Adocoes\n");
        printf("8. Gerar 50 animais aleatorios\n");
        printf("9. Limpar Arquivo de Log\n");
        printf("0. Sair\n");
        printf("Opcao: ");
        scanf("%d", &op);

        switch(op) {
            case 1:
                Animal_Cadastrar();
                RegistrarLog("Novo animal cadastrado.");
                break;
            case 2:
                Adotante_Cadastrar();
                RegistrarLog("Novo adotante cadastrado.");
                break;
            case 3:
                Animal_Listar();
                Adotante_Listar();
                break;
            case 4: {
                clock_t t_inicio = clock();
                long comps = Animal_OrdenarPorID();
                clock_t t_fim = clock();

                double tempo = ((double)(t_fim - t_inicio)) / CLOCKS_PER_SEC;

                printf("\nConcluido! Tempo: %.4fs | Comparacoes: %ld\n", tempo, comps);

                sprintf(logMsg, "[PERFORMANCE] Ordenacao BubbleSort: Tempo=%.4fs, Comparacoes=%ld", tempo, comps);
                RegistrarLog(logMsg);
                break;
            }
            case 5: {
                int id, numComps = 0;
                printf("ID para busca: "); scanf("%d", &id);

                clock_t t_inicio = clock();
                long pos = Animal_BuscaBinaria(id, &numComps);
                clock_t t_fim = clock();

                double tempo = ((double)(t_fim - t_inicio)) / CLOCKS_PER_SEC;

                if (pos != -1) {
                    Animal a = Animal_LerDoDisco(pos);
                    printf("Encontrado: %s\n", a.nome);
                } else {
                    printf("Nao encontrado (Verifique se ordenou antes).\n");
                }
                printf("Tempo: %.6fs | Comparacoes: %d\n", tempo, numComps);

                sprintf(logMsg, "[PERFORMANCE] Busca Binaria ID %d: Tempo=%.6fs, Comparacoes=%d, Achou=%s",
                        id, tempo, numComps, (pos!=-1)?"SIM":"NAO");
                RegistrarLog(logMsg);
                break;
            }
            case 6: Controlador_RealizarAdocao(); break;
            case 7: Adocao_ListarRelatorio(); break;
            case 8: GerarMassaDeTeste(); break;
            case 9: LimparLog();

            break;

        }
    } while (op != 0);

    return 0;
}
