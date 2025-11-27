#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// ---------------------------
// STRUCTS
// ---------------------------

// Estrutura da sala da mansão (Árvore Binária)
typedef struct Sala {
    char nome[50];
    char pista[80];  // pista opcional
    struct Sala* esquerda;
    struct Sala* direita;
} Sala;

// Nó da árvore BST de pistas
typedef struct PistaNode {
    char pista[80];
    struct PistaNode* esquerda;
    struct PistaNode* direita;
} PistaNode;


// ---------------------------
// FUNÇÕES DE CRIAÇÃO
// ---------------------------

// Cria dinamicamente uma sala
Sala* criarSala(char nome[], char pista[]) {
    Sala* nova = (Sala*) malloc(sizeof(Sala));
    strcpy(nova->nome, nome);
    strcpy(nova->pista, pista);
    nova->esquerda = NULL;
    nova->direita = NULL;
    return nova;
}

// Insere pista na BST
PistaNode* inserirPista(PistaNode* raiz, char pista[]) {
    if (raiz == NULL) {
        PistaNode* novo = (PistaNode*) malloc(sizeof(PistaNode));
        strcpy(novo->pista, pista);
        novo->esquerda = NULL;
        novo->direita = NULL;
        return novo;
    }

    if (strcmp(pista, raiz->pista) < 0)
        raiz->esquerda = inserirPista(raiz->esquerda, pista);
    else
        raiz->direita = inserirPista(raiz->direita, pista);

    return raiz;
}

// Exibe pistas em ordem alfabética (in-order)
void exibirPistas(PistaNode* raiz) {
    if (raiz == NULL) return;

    exibirPistas(raiz->esquerda);
    printf(" - %s\n", raiz->pista);
    exibirPistas(raiz->direita);
}


// ---------------------------
// EXPLORAÇÃO DA MANSÃO
// ---------------------------

void explorarSalasComPistas(Sala* atual, PistaNode** arvorePistas) {
    char opcao;

    while (1) {
        printf("\nVocê está na sala: %s\n", atual->nome);

        // Se a sala tiver pista, coletar automaticamente
        if (strlen(atual->pista) > 0) {
            printf("Pista encontrada: %s\n", atual->pista);
            *arvorePistas = inserirPista(*arvorePistas, atual->pista);
        }

        // Opções de movimento
        printf("\nMover para:\n");
        if (atual->esquerda != NULL) printf(" (e) Esquerda -> %s\n", atual->esquerda->nome);
        if (atual->direita != NULL)  printf(" (d) Direita  -> %s\n", atual->direita->nome);
        printf(" (s) Sair da exploração\n");
        printf("Escolha: ");
        scanf(" %c", &opcao);

        if (opcao == 'e' && atual->esquerda != NULL)
            atual = atual->esquerda;
        else if (opcao == 'd' && atual->direita != NULL)
            atual = atual->direita;
        else if (opcao == 's')
            break;
        else
            printf("\nOpção inválida!\n");
    }
}


// ---------------------------
// FUNÇÃO PRINCIPAL
// ---------------------------

int main() {

    // Criando a estrutura fixa da mansão
    Sala* hall     = criarSala("Hall de Entrada", "Pegada suspeita no tapete");
    Sala* salaEst  = criarSala("Sala de Estar", "Copo quebrado no chão");
    Sala* cozinha  = criarSala("Cozinha", "Faca fora do lugar");
    Sala* jardim   = criarSala("Jardim", "");
    Sala* escritorio = criarSala("Escritório", "Documento rasgado na mesa");

    // Montando o mapa
    hall->esquerda = salaEst;
    hall->direita = cozinha;

    salaEst->esquerda = jardim;
    salaEst->direita = escritorio;

    // Árvore de pistas (BST)
    PistaNode* arvorePistas = NULL;

    // Exploração
    printf("=== DETECTIVE QUEST — SISTEMA DE PISTAS ===\n");
    explorarSalasComPistas(hall, &arvorePistas);

    // Exibir pistas coletadas
    printf("\n\nPistas coletadas (ordenadas):\n");
    exibirPistas(arvorePistas);

    printf("\nFim da exploração!\n");
    return 0;
}
