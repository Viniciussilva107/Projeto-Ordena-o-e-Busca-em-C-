#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "animal.h"
#include "adotante.h"
#include "adocao.h"
#include "ordenacao_externa.h"


void RegistrarLog(const char* mensagem) {
    FILE *fp = fopen("log.txt", "a");
    if (!fp) return;

    time_t t = time(NULL);
    struct tm tm = *localtime(&t);

    fprintf(fp, "[%02d/%02d/%d %02d:%02d:%02d] %s\n",
            tm.tm_mday, tm.tm_mon + 1, tm.tm_year + 1900,
            tm.tm_hour, tm.tm_min, tm.tm_sec, mensagem);
    fclose(fp);
}

void LimparLog() {
    FILE *fp = fopen("log.txt", "w");
    if (fp) {
        fprintf(fp, "--- LOG REINICIADO ---\n");
        fclose(fp);
        printf("\n>>> Log limpo com sucesso! <<<\n");
    }
}


void Controlador_RealizarAdocao() {
    int idAni, idAdo, lixo;
    char data[12], logMsg[200];

    printf("\n>>> Realizar Adocao <<<\n");

    printf("ID Animal: "); scanf("%d", &idAni);
    long posAni = Animal_BuscaSequencial(idAni, &lixo);

    if (posAni == -1) {
        printf("Erro: Animal nao encontrado.\n");
        return;
    }

    Animal ani = Animal_LerDoDisco(posAni);
    if (ani.status == 1) {
        printf("Erro: O animal '%s' ja foi adotado!\n", ani.nome);
        sprintf(logMsg, "FALHA ADOCAO: Tentativa de adotar animal ID %d (indisponivel).", idAni);
        RegistrarLog(logMsg);
        return;
    }

    printf("Animal selecionado: %s. ID Adotante: ", ani.nome); scanf("%d", &idAdo);
    long posAdo = Adotante_BuscaSequencial(idAdo);

    if (posAdo == -1) {
        printf("Erro: Adotante nao encontrado.\n");
        return;
    }

    printf("Data da adocao (DD/MM/AAAA): ");
    getchar(); fgets(data, 12, stdin); data[strcspn(data, "\n")] = 0;

    Adocao_Salvar(idAni, idAdo, data);
    Animal_SetStatusAdotado(posAni);

    printf("Sucesso! Adoção registrada.\n");

    sprintf(logMsg, "SUCESSO ADOCAO: Animal %d adotado por %d na data %s", idAni, idAdo, data);
    RegistrarLog(logMsg);
}



int main() {
    int op;
    char logMsg[300];

    srand(time(NULL));

    do {
        printf("\n=== SISTEMA DE ADOCAO ===\n");
        printf("1. Cadastrar Animal\n");
        printf("2. Cadastrar Adotante\n");
        printf("3. Listar Bases \n");
        printf("4. Realizar Adocao \n");
        printf("5. Busca Binaria Animal\n");
        printf("6. Busca Sequencial Animal\n");
        printf("7. Relatorio de Adocoes Realizadas\n");
        printf("10. Gerar Massa de Animais \n");
        printf("11. Parte I: Ordenar em Disco (Bubble Sort)\n");
        printf("12. Parte II: Ordenacao Externa \n");
        printf("13. Limpar Arquivo de Log\n");
        printf("14. Gerar Massa de Teste ADOTANTES\n");
        printf("0. Sair\n");

        printf("Opcao: ");
        scanf("%d", &op);

        switch(op) {
            case 1:
                Animal_Cadastrar();
                RegistrarLog("Novo animal cadastrado manualmente.");
                break;

            case 2:
                Adotante_Cadastrar();
                RegistrarLog("Novo adotante cadastrado manualmente.");
                break;

            case 3:
                Animal_Listar();
                Adotante_Listar();
                break;

            case 4:
                Controlador_RealizarAdocao();
                break;

            case 5: {
                int id, comps=0;
                printf("ID para Busca Binaria: "); scanf("%d", &id);

                clock_t t1 = clock();
                long pos = Animal_BuscaBinaria(id, &comps);
                double tempo = ((double)(clock() - t1)) / CLOCKS_PER_SEC;

                if(pos != -1) {
                    Animal a = Animal_LerDoDisco(pos);
                    printf("ENCONTRADO: %s (Raca: %s)\n", a.nome, a.raca);
                } else {
                    printf("Nao encontrado (Verifique se a base esta ordenada).\n");
                }

                printf("Tempo: %f s | Comparacoes: %d\n", tempo, comps);
                sprintf(logMsg, "BUSCA BINARIA ID %d: Tempo %f s | Comps %d", id, tempo, comps);
                RegistrarLog(logMsg);
                break;
            }

            case 6: {
                int id, comps=0;
                printf("ID para Busca Sequencial: "); scanf("%d", &id);

                clock_t t1 = clock();
                long pos = Animal_BuscaSequencial(id, &comps);
                double tempo = ((double)(clock() - t1)) / CLOCKS_PER_SEC;

                if(pos != -1) {
                    Animal a = Animal_LerDoDisco(pos);
                    printf("ENCONTRADO: %s (Raca: %s)\n", a.nome, a.raca);
                } else {
                    printf("Nao encontrado.\n");
                }

                printf("Tempo: %f s | Comparacoes: %d\n", tempo, comps);
                sprintf(logMsg, "BUSCA SEQUENCIAL ID %d: Tempo %f s | Comps %d", id, tempo, comps);
                RegistrarLog(logMsg);
                break;
            }

            case 7:
                Adocao_ListarRelatorio();
                break;

            case 10: {
                int qtd;
                printf("Quantos ANIMAIS gerar? "); scanf("%d", &qtd);
                FILE *fp = fopen("animais.dat", "wb");
                Animal a;
                for(int i=0; i<qtd; i++) {
                    a.id = rand() % (qtd * 10);
                    sprintf(a.nome, "Bicho_%d", i);
                    sprintf(a.especie, "Gen");
                    sprintf(a.raca, "SRD");
                    a.idade = rand() % 15;
                    a.status = 0;
                    fwrite(&a, sizeof(Animal), 1, fp);
                }
                fclose(fp);
                printf("%d Animais Gerados com sucesso.\n", qtd);
                RegistrarLog("Massa de teste de Animais gerada.");
                break;
            }

            case 14: {
                int qtd;
                printf("Quantos ADOTANTES gerar? "); scanf("%d", &qtd);
                Adotante_GerarMassaTeste(qtd); // Chama a função do modulo adotante
                RegistrarLog("Massa de teste de Adotantes gerada.");
                break;
            }

            case 11: {
                clock_t t1 = clock();
                long comps = Animal_OrdenarPorID_Bubble();
                double tempo = ((double)(clock() - t1)) / CLOCKS_PER_SEC;

                printf("Bubble Sort Concluido.\n");
                printf("Tempo: %.4f s | Comparacoes (Trocas/Checks): %ld\n", tempo, comps);

                sprintf(logMsg, "Teste BubbleSort: Tempo %.4f s | Comps %ld", tempo, comps);
                RegistrarLog(logMsg);
                break;
            }

            case 12: {
                clock_t t1 = clock();
                OrdenacaoExterna_Completa();
                double tempo = ((double)(clock() - t1)) / CLOCKS_PER_SEC;

                printf("Ordenacao Externa Concluida.\n");
                printf("Tempo Total: %.4f s\n", tempo);

                sprintf(logMsg, "Teste Ordenacao Externa: Tempo %.4f s", tempo);
                RegistrarLog(logMsg);
                break;
            }

            case 13:
                LimparLog();
                break;
        }

    } while (op != 0);

    return 0;
}
