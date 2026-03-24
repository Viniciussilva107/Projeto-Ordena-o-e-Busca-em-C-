#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "animal.h"
#include "adotante.h"
#include "adocao.h"
#include "ordenacao_externa.h"
#include "hash.h"


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
    int idAni, idAdo;
    char data[12];

    printf("\n>>> Realizar Adocao <<<\nID Animal: ");
    scanf("%d", &idAni);

    long posAni = Hash_Buscar(idAni);
    if (posAni == -1) {
        printf("Erro: Animal nao encontrado ou deletado.\n");
        return;
    }

    Animal ani = Animal_LerDoDisco(posAni);
    if (ani.status == 1) {
        printf("Erro: O animal '%s' ja foi adotado!\n", ani.nome);
        return;
    }

    printf("Animal selecionado: %s. ID Adotante: ", ani.nome);
    scanf("%d", &idAdo);

    long posAdo = Adotante_BuscaSequencial(idAdo);
    if (posAdo == -1) {
        printf("Erro: Adotante nao encontrado.\n");
        return;
    }

    printf("Data da adocao (DD/MM/AAAA): ");
    getchar();
    fgets(data, 12, stdin);
    data[strcspn(data, "\n")] = 0;

    Adocao_Salvar(idAni, idAdo, data);
    Animal_SetStatusAdotado(posAni);

    printf("Sucesso! Adocao registrada.\n");
    RegistrarLog("ADOCAO: SUCESSO");
}


int main() {
    int op;
    char logMsg[300];
    srand(time(NULL));

    Hash_Inicializar();

    do {
        printf("\n=== SISTEMA DE ADOCAO ===\n");

        printf("1. Inserir Animal \n");
        printf("2. Buscar Animal \n");
        printf("3. Remover Animal \n");
        printf("4. Exibir Espacos Livres \n");

        printf("5. Cadastrar Adotante\n");
        printf("6. Realizar Adocao\n");
        printf("7. Relatorio de Adocoes\n");
        printf("8. Listar Todos os Cadastros\n");

        printf("10. Gerar Massa de Teste ANIMAIS \n");
        printf("11. Gerar Massa de Teste ADOTANTES\n");
        printf("12. Parte I: Ordenar em Disco (Bubble Sort)\n");
        printf("13. Parte II: Ordenacao Externa\n");
        printf("14. Busca Binaria Animal (Requer base ordenada)\n");
        printf("15. Busca Sequencial Animal\n");
        printf("16. Limpar Arquivo de Log\n");

        printf("0. Sair\n");
        scanf("%d", &op);

        switch(op) {
            case 1: {
                Animal a;
                printf("\n[Novo Animal via Hash]\nID: "); scanf("%d", &a.id); getchar();
                printf("Nome: "); fgets(a.nome, 50, stdin); a.nome[strcspn(a.nome, "\n")] = 0;
                printf("Especie: "); fgets(a.especie, 20, stdin); a.especie[strcspn(a.especie, "\n")] = 0;
                printf("Raca: "); fgets(a.raca, 20, stdin); a.raca[strcspn(a.raca, "\n")] = 0;
                printf("Idade: "); scanf("%d", &a.idade);

                a.status = 0;
                a.prox = -1;

                if (Hash_Inserir(a)) {
                    printf("Inserido com sucesso via Hash!\n");
                    RegistrarLog("Insercao Hash SUCESSO");
                } else {
                    printf("Erro: ID ja existe na base!\n");
                }
                break;
            }

            case 2: {
                int id;
                printf("ID para Buscar (Hash): "); scanf("%d", &id);

                clock_t t1 = clock();
                long pos = Hash_Buscar(id);
                double tempo = ((double)(clock() - t1)) / CLOCKS_PER_SEC;

                if (pos != -1) {
                    Animal a = Animal_LerDoDisco(pos);
                    printf("\nACHOU -> Nome: %s | Idade: %d | Raca: %s\n", a.nome, a.idade, a.raca);
                    printf("Encontrado no Byte Fisico: %ld em %.6f s\n", pos, tempo);
                } else {
                    printf("Nao encontrado na Tabela Hash.\n");
                }
                break;
            }

            case 3: {
                int id;
                printf("ID para Remover: "); scanf("%d", &id);
                if (Hash_Remover(id)) {
                    printf("Removido com sucesso. Espaco liberado e adicionado na L.E.D.!\n");
                    RegistrarLog("Remocao Hash SUCESSO");
                } else {
                    printf("Erro: ID nao encontrado.\n");
                }
                break;
            }

            case 4:
                Hash_ListarEspacosLivres();
                break;

            case 5:
                Adotante_Cadastrar();
                break;

            case 6:
                Controlador_RealizarAdocao();
                break;

            case 7:
                Adocao_ListarRelatorio();
                break;

            case 8:
                Animal_Listar();
                Adotante_Listar();
                break;

            case 10: {
                int qtd;
                printf("Quantos ANIMAIS gerar? "); scanf("%d", &qtd);

                FILE *fp = fopen("animais.dat", "wb"); fclose(fp);
                remove("hash.dat");
                remove("led.dat");
                Hash_Inicializar();

                printf("Gerando e inserindo via Hash...\n");
                Animal a;
                for(int i=0; i<qtd; i++) {
                    a.id = rand() % (qtd * 10);
                    sprintf(a.nome, "Bicho_%d", i);
                    strcpy(a.especie, "Teste");
                    strcpy(a.raca, "SRD");
                    a.idade = rand() % 15 + 1;
                    a.status = 0;
                    a.prox = -1;

                    Hash_Inserir(a);
                }
                printf("%d Animais Gerados na Tabela Hash com sucesso.\n", qtd);
                RegistrarLog("Massa de Teste de Animais gerada");
                break;
            }

            case 11: {
                int qtd;
                printf("Quantos ADOTANTES gerar? "); scanf("%d", &qtd);
                Adotante_GerarMassaTeste(qtd);
                break;
            }

            case 12: {
                clock_t t1 = clock();
                long comps = Animal_OrdenarPorID_Bubble();
                double tempo = ((double)(clock() - t1)) / CLOCKS_PER_SEC;

                printf("\nBubble Sort Concluido.\n");
                printf("Tempo: %.4f s | Comparacoes (Trocas): %ld\n", tempo, comps);

                sprintf(logMsg, "BubbleSort: Tempo %.4f s | Comps %ld", tempo, comps);
                RegistrarLog(logMsg);

                printf("AVISO: A ordenacao fisica destruiu os ponteiros da Tabela Hash!\n");
                break;
            }

            case 13: {
                clock_t t1 = clock();
                OrdenacaoExterna_Completa();
                double tempo = ((double)(clock() - t1)) / CLOCKS_PER_SEC;

                printf("\nOrdenacao Externa Concluida.\n");
                printf("Tempo Total: %.4f s\n", tempo);

                sprintf(logMsg, "Ordenacao Externa: Tempo %.4f s", tempo);
                RegistrarLog(logMsg);

                printf("AVISO: A ordenacao fisica destruiu os ponteiros da Tabela Hash!\n");
                break;
            }

            case 14: {
                int id, comps=0;
                printf("ID Busca Binaria: "); scanf("%d", &id);
                clock_t t1 = clock();
                long pos = Animal_BuscaBinaria(id, &comps);
                double tempo = ((double)(clock() - t1)) / CLOCKS_PER_SEC;
                if(pos!=-1) printf("Achou no byte %ld!\n", pos); else printf("Nao achou.\n");
                printf("Tempo: %.6f s | Comparacoes: %d\n", tempo, comps);
                break;
            }

            case 15: {
                int id, comps=0;
                printf("ID Busca Sequencial: "); scanf("%d", &id);
                clock_t t1 = clock();
                long pos = Animal_BuscaSequencial(id, &comps);
                double tempo = ((double)(clock() - t1)) / CLOCKS_PER_SEC;
                if(pos!=-1) printf("Achou no byte %ld!\n", pos); else printf("Nao achou.\n");
                printf("Tempo: %.6f s | Comparacoes: %d\n", tempo, comps);
                break;
            }

            case 16:
                LimparLog();
                break;

            case 0:
                printf("Saindo do sistema...\n");
                break;

            default:
                printf("Opcao invalida!\n");
        }
    } while (op != 0);

    return 0;
}
