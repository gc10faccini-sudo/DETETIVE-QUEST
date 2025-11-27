/*
 Detective Quest — Nível Mestre (versão completa de coleta + hash + julgamento)
 Autor: Enigma Studios (exemplo)
 Compilar: gcc detective_mestre.c -o detective_mestre
 Execução: ./detective_mestre
*/

/* Bibliotecas */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAXNOME 64
#define MAXPISTA 128
#define TABLE_SIZE 53   // tamanho da tabela hash (número primo para diminuir colisões)

/* -----------------------------
   STRUCTS
   ----------------------------- */

/* Sala: nó da árvore binária (mapa da mansão) */
typedef struct Sala {
    char nome[MAXNOME];
    char pista[MAXPISTA];   // pista associada à sala (pode ser string vazia)
    struct Sala *esq;
    struct Sala *dir;
} Sala;

/* Nó da BST para pistas coletadas */
typedef struct PistaNode {
    char pista[MAXPISTA];
    struct PistaNode *esq;
    struct PistaNode *dir;
} PistaNode;

/* Nó da tabela hash (encadeamento) */
typedef struct HashNode {
    char pista[MAXPISTA];           // chave
    char suspeito[MAXNOME];         // valor
    struct HashNode *prox;
} HashNode;

/* Tabela hash global (array de ponteiros) */
HashNode *hashTable[TABLE_SIZE] = { NULL };

/* -----------------------------
   DECLARAÇÕES DE FUNÇÕES
   ----------------------------- */

/* criarSala: cria dinamicamente um cômodo com nome e pista */
Sala* criarSala(const char *nome, const char *pista);

/* explorarSalas: navega pela árvore e ativa o sistema de coleta de pistas */
void explorarSalas(Sala *inicio, PistaNode **raizPistas);

/* inserirPista: insere (ou ignora se já existir) uma pista na BST */
PistaNode* inserirPista(PistaNode *raiz, const char *pista);

/* adicionarPista: wrapper que checa duplicata e imprime mensagem */
void adicionarPista(PistaNode **raiz, const char *pista);

/* exibirPistas: imprime a BST em ordem alfabética (in-order) */
void exibirPistas(PistaNode *raiz);

/* liberarPistas: libera memória da BST */
void liberarPistas(PistaNode *raiz);

/* Hash helpers */
unsigned long hashFunction(const char *str);
void inserirNaHash(const char *pista, const char *suspeito);
const char* encontrarSuspeito(const char *pista);
void liberarHash();

/* verificarSuspeitoFinal: conta ocorrências e retorna 1 se >= 2, 0 caso contrário */
int verificarSuspeitoFinal(PistaNode *raiz, const char *acusado);

/* Auxiliares */
int pistaExiste(PistaNode *raiz, const char *pista);
int contarParaSuspeito(PistaNode *raiz, const char *acusado);

/* -----------------------------
   IMPLEMENTAÇÃO
   ----------------------------- */

/* criarSala: cria dinamicamente um cômodo com nome e pista */
Sala* criarSala(const char *nome, const char *pista) {
    Sala *s = (Sala*) malloc(sizeof(Sala));
    if (!s) {
        fprintf(stderr, "Erro de alocação!\n");
        exit(EXIT_FAILURE);
    }
    strncpy(s->nome, nome, MAXNOME-1);
    s->nome[MAXNOME-1] = '\0';
    if (pista) {
        strncpy(s->pista, pista, MAXPISTA-1);
        s->pista[MAXPISTA-1] = '\0';
    } else {
        s->pista[0] = '\0';
    }
    s->esq = s->dir = NULL;
    return s;
}

/* inserirPista: insere na BST em ordem alfabética (sem duplicatas) */
PistaNode* inserirPista(PistaNode *raiz, const char *pista) {
    if (raiz == NULL) {
        PistaNode *n = (PistaNode*) malloc(sizeof(PistaNode));
        if (!n) { fprintf(stderr, "Erro de alocação!\n"); exit(EXIT_FAILURE); }
        strncpy(n->pista, pista, MAXPISTA-1);
        n->pista[MAXPISTA-1] = '\0';
        n->esq = n->dir = NULL;
        return n;
    }
    int cmp = strcmp(pista, raiz->pista);
    if (cmp < 0) raiz->esq = inserirPista(raiz->esq, pista);
    else if (cmp > 0) raiz->dir = inserirPista(raiz->dir, pista);
    // se cmp == 0: duplicata -> não insere
    return raiz;
}

/* pistaExiste: verifica se a pista já está na BST */
int pistaExiste(PistaNode *raiz, const char *pista) {
    if (!raiz) return 0;
    int cmp = strcmp(pista, raiz->pista);
    if (cmp == 0) return 1;
    else if (cmp < 0) return pistaExiste(raiz->esq, pista);
    else return pistaExiste(raiz->dir, pista);
}

/* adicionarPista: insere pista na árvore de pistas se ainda não existente */
void adicionarPista(PistaNode **raiz, const char *pista) {
    if (!pista || pista[0] == '\0') return; // nada a adicionar
    if (pistaExiste(*raiz, pista)) {
        printf("Pista já coletada anteriormente: \"%s\"\n", pista);
    } else {
        *raiz = inserirPista(*raiz, pista);
        printf("Pista coletada: \"%s\"\n", pista);
    }
}

/* exibirPistas: imprime em ordem alfabética (in-order traversal) */
void exibirPistas(PistaNode *raiz) {
    if (!raiz) return;
    exibirPistas(raiz->esq);
    printf(" - %s\n", raiz->pista);
    exibirPistas(raiz->dir);
}

/* liberarPistas: libera nós da BST (recursivamente) */
void liberarPistas(PistaNode *raiz) {
    if (!raiz) return;
    liberarPistas(raiz->esq);
    liberarPistas(raiz->dir);
    free(raiz);
}

/* -----------------------------
   HASH: implementação simples (djb2) + encadeamento
   ----------------------------- */

/* hashFunction: gera índice a partir de string (djb2) */
unsigned long hashFunction(const char *str) {
    unsigned long hash = 5381;
    int c;
    while ((c = (unsigned char)*str++))
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
    return hash % TABLE_SIZE;
}

/* inserirNaHash: insere associação pista -> suspeito */
void inserirNaHash(const char *pista, const char *suspeito) {
    if (!pista || !suspeito) return;
    unsigned long idx = hashFunction(pista);
    HashNode *newNode = (HashNode*) malloc(sizeof(HashNode));
    if (!newNode) { fprintf(stderr, "Erro de alocação!\n"); exit(EXIT_FAILURE); }
    strncpy(newNode->pista, pista, MAXPISTA-1);
    newNode->pista[MAXPISTA-1] = '\0';
    strncpy(newNode->suspeito, suspeito, MAXNOME-1);
    newNode->suspeito[MAXNOME-1] = '\0';
    newNode->prox = hashTable[idx];
    hashTable[idx] = newNode;
}

/* encontrarSuspeito: retorna ponteiro para o nome do suspeito correspondente à pista (ou NULL) */
const char* encontrarSuspeito(const char *pista) {
    if (!pista || pista[0] == '\0') return NULL;
    unsigned long idx = hashFunction(pista);
    HashNode *p = hashTable[idx];
    while (p) {
        if (strcmp(p->pista, pista) == 0) return p->suspeito;
        p = p->prox;
    }
    return NULL;
}

/* liberarHash: libera todos os nós da hash table */
void liberarHash() {
    for (int i = 0; i < TABLE_SIZE; ++i) {
        HashNode *p = hashTable[i];
        while (p) {
            HashNode *t = p->prox;
            free(p);
            p = t;
        }
        hashTable[i] = NULL;
    }
}

/* -----------------------------
   EXPLORAÇÃO: navegação interativa (e/d/s)
   ----------------------------- */

/* explorarSalas: navega pela árvore e ativa o sistema de pistas */
void explorarSalas(Sala *inicio, PistaNode **raizPistas) {
    if (!inicio) return;

    Sala *atual = inicio;
    char opcao;
    printf("\nIniciando exploração a partir de: %s\n", inicio->nome);

    while (1) {
        printf("\nVocê está em: %s\n", atual->nome);

        /* exibe pista da sala (se houver) e coleta automaticamente */
        if (strlen(atual->pista) > 0) {
            printf(" Pista disponível: \"%s\"\n", atual->pista);
            adicionarPista(raizPistas, atual->pista);
        } else {
            printf(" Não há pista visível nesta sala.\n");
        }

        /* opções de movimentação (mostrando nomes dos destinos se existirem) */
        printf("\nOpções:\n");
        if (atual->esq) printf(" (e) Esquerda  -> %s\n", atual->esq->nome);
        if (atual->dir) printf(" (d) Direita   -> %s\n", atual->dir->nome);
        printf(" (s) Sair da exploração e ir ao julgamento\n");
        printf("Escolha: ");
        scanf(" %c", &opcao);
        opcao = tolower((unsigned char)opcao);

        if (opcao == 'e') {
            if (atual->esq) atual = atual->esq;
            else printf("Não existe caminho à esquerda.\n");
        } else if (opcao == 'd') {
            if (atual->dir) atual = atual->dir;
            else printf("Não existe caminho à direita.\n");
        } else if (opcao == 's') {
            printf("Exploração finalizada pelo jogador.\n");
            break;
        } else {
            printf("Opção inválida. Use 'e', 'd' ou 's'.\n");
        }
    }
}

/* -----------------------------
   VERIFICAÇÃO DO SUSPEITO FINAL
   ----------------------------- */

/* contarParaSuspeito: percorre a BST e conta quantas pistas apontam para 'acusado' */
int contarParaSuspeito(PistaNode *raiz, const char *acusado) {
    if (!raiz) return 0;
    int count = 0;
    const char *s = encontrarSuspeito(raiz->pista);
    if (s && strcmp(s, acusado) == 0) count = 1;
    return count + contarParaSuspeito(raiz->esq, acusado) + contarParaSuspeito(raiz->dir, acusado);
}

/* verificarSuspeitoFinal: retorna 1 se há pelo menos 2 pistas indicando o acusado */
int verificarSuspeitoFinal(PistaNode *raiz, const char *acusado) {
    int total = contarParaSuspeito(raiz, acusado);
    printf("\nNúmero de pistas que apontam para \"%s\": %d\n", acusado, total);
    return (total >= 2) ? 1 : 0;
}

/* -----------------------------
   FUNÇÃO MAIN: montagem do mapa, hash, exploração e julgamento
   ----------------------------- */

int main() {

    /* ---------- montar mapa fixo da mansão (árvore binária) ---------- */
    Sala *hall        = criarSala("Hall de Entrada", "Pegada estranha no tapete");
    Sala *salaEstar   = criarSala("Sala de Estar", "Copo quebrado com resíduo vermelho");
    Sala *corredor    = criarSala("Corredor Principal", "");
    Sala *cozinha     = criarSala("Cozinha", "Faca suja escondida");
    Sala *biblioteca  = criarSala("Biblioteca", "Marca de tinta no livro antigo");
    Sala *jardim      = criarSala("Jardim", "Pegada molhada perto da estaca");
    Sala *escritorio  = criarSala("Escritório", "Documento rasgado com assinatura");
    Sala *porão       = criarSala("Porão", "Rastro de lama na escada");
    Sala *varanda     = criarSala("Varanda", "Cigarro apagado com cheiro peculiar");

    /* montar conexões (árvore binária) */
    hall->esq = salaEstar;
    hall->dir = corredor;

    salaEstar->esq = biblioteca;
    salaEstar->dir = cozinha;

    corredor->esq = jardim;
    corredor->dir = escritorio;

    cozinha->esq = porão;      // exemplo: caminho adicional como filho esquerdo
    jardim->dir = varanda;    // jardim tem só filho direito

    /* ---------- criar associações pista -> suspeito (hash) ---------- */
    inserirNaHash("Pegada estranha no tapete", "Sr. Black");
    inserirNaHash("Copo quebrado com resíduo vermelho", "Sra. White");
    inserirNaHash("Faca suja escondida", "Jardineiro");
    inserirNaHash("Marca de tinta no livro antigo", "Bibliotecária");
    inserirNaHash("Pegada molhada perto da estaca", "Sr. Black");
    inserirNaHash("Documento rasgado com assinatura", "Advogado");
    inserirNaHash("Rastro de lama na escada", "Jardineiro");
    inserirNaHash("Cigarro apagado com cheiro peculiar", "Sra. White");
    // NOTA: Algumas pistas apontam para mesmos suspeitos (ex.: Sr. Black tem 2 pistas)

    /* ---------- BST de pistas coletadas (inicialmente vazia) ---------- */
    PistaNode *arvorePistas = NULL;

    /* ---------- Exploração interativa ---------- */
    printf("=== DETECTIVE QUEST — MODO INVESTIGAÇÃO ===\n");
    printf("Instruções: 'e' = esquerda, 'd' = direita, 's' = sair e ir ao julgamento\n");
    explorarSalas(hall, &arvorePistas);

    /* ---------- Exibir pistas coletadas ---------- */
    printf("\n--- Pistas coletadas (ordenadas) ---\n");
    if (arvorePistas == NULL) {
        printf("Nenhuma pista foi coletada.\n");
    } else {
        exibirPistas(arvorePistas);
    }

    /* ---------- Fase de acusação ---------- */
    char acusado[MAXNOME];
    printf("\nDigite o nome do suspeito que deseja acusar (ex.: Sr. Black): ");
    // limpar buffer e ler linha
    getchar(); // absorve '\n' remanescente
    if (!fgets(acusado, sizeof(acusado), stdin)) acusado[0] = '\0';
    // remover newline
    acusado[strcspn(acusado, "\r\n")] = 0;

    if (strlen(acusado) == 0) {
        printf("Nenhum suspeito indicado. Encerrando.\n");
    } else {
        int ok = verificarSuspeitoFinal(arvorePistas, acusado);
        if (ok) {
            printf("\nVEREDICTO: Há evidências suficientes. \"%s\" é apontado como culpado.\n", acusado);
            printf("Parabéns, sua investigação encontrou provas!\n");
        } else {
            printf("\nVEREDICTO: Pistas insuficientes para responsabilizar \"%s\".\n", acusado);
            printf("Recomendação: continue a investigação e reúna mais evidências.\n");
        }
    }

    /* ---------- limpeza de memória ---------- */
    liberarPistas(arvorePistas);
    liberarHash();
    // liberar salas (árvore de salas)
    // liberar manualmente cada sala alocada (simples porque mapa fixo)
    free(hall); free(salaEstar); free(corredor); free(cozinha);
    free(biblioteca); free(jardim); free(escritorio); free(porão); free(varanda);

    printf("\nPrograma encerrado.\n");
    return 0;
}
