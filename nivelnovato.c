#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* ----------------------------
   STRUCT DA SALA (NÓ DA ÁRVORE)
   ---------------------------- */
typedef struct Sala {
    char nome[50];
    struct Sala *esq;   // caminho à esquerda
    struct Sala *dir;   // caminho à direita
} Sala;

/* ---------------------------------------
   criarSala(): cria dinamicamente uma sala
   --------------------------------------- */
Sala* criarSala(const char *nome) {
    Sala *nova = (Sala *)malloc(sizeof(Sala));
    strcpy(nova->nome, nome);
    nova->esq = NULL;
    nova->dir = NULL;
    return nova;
}

/* --------------------------------------------------------
   explorarSalas(): permite a navegação pela árvore binária
   -------------------------------------------------------- */
void explorarSalas(Sala *atual) {

    while (1) {
        printf("\nVocê está em: %s\n", atual->nome);

        // Se for nó-folha, termina exploração
        if (atual->esq == NULL && atual->dir == NULL) {
            printf("Você chegou ao fim deste caminho.\n");
            return;
        }

        printf("Escolha o caminho:\n");
        if (atual->esq) printf("  (e) Ir para %s\n", atual->esq->nome);
        if (atual->dir) printf("  (d) Ir para %s\n", atual->dir->nome);
        printf("  (s) Sair da exploração\n");
        printf("Opção: ");

        char op;
        scanf(" %c", &op);

        if (op == 'e' && atual->esq) {
            atual = atual->esq;
        } 
        else if (op == 'd' && atual->dir) {
            atual = atual->dir;
        } 
        else if (op == 's') {
            printf("Exploração encerrada.\n");
            return;
        } 
        else {
            printf("Opção inválida! Tente novamente.\n");
        }
    }
}

/* -------------------------
   main(): monta o mapa fixo
   ------------------------- */
int main() {

    /* Criação automática da mansão */
    Sala *hall            = criarSala("Hall de Entrada");
    Sala *salaEstar       = criarSala("Sala de Estar");
    Sala *corredor        = criarSala("Corredor Principal");
    Sala *cozinha         = criarSala("Cozinha");
    Sala *biblioteca      = criarSala("Biblioteca");
    Sala *jardim          = criarSala("Jardim");
    Sala *porao           = criarSala("Porão");

    /* Estrutura da mansão (árvore binária) */

    hall->esq = salaEstar;
    hall->dir = corredor;

    salaEstar->esq = cozinha;
    salaEstar->dir = biblioteca;

    corredor->esq = jardim;
    corredor->dir = porao;

    printf("=== DETECTIVE QUEST — EXPLORADOR DE MANSÃO ===\n");
    explorarSalas(hall);

    return 0;
}
