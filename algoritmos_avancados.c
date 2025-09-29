#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TAM_HASH 10
#define MAX_STR 100

// ------------------- STRUCTS -------------------

typedef struct Sala {
    char nome[MAX_STR];
    struct Sala* esquerda;
    struct Sala* direita;
} Sala;

typedef struct PistaBST {
    char pista[MAX_STR];
    struct PistaBST* esquerda;
    struct PistaBST* direita;
} PistaBST;

typedef struct HashItem {
    char pista[MAX_STR];
    char suspeito[MAX_STR];
    struct HashItem* proximo;
} HashItem;

typedef struct HashTable {
    HashItem* itens[TAM_HASH];
} HashTable;

// ------------------- FUNÇÕES DE HASH -------------------

// Função de hash simples baseada em soma de caracteres
int hash(char* chave) {
    int soma = 0;
    for (int i = 0; chave[i] != '\0'; i++) {
        soma += chave[i];
    }
    return soma % TAM_HASH;
}

// Insere uma pista e seu suspeito associado na tabela hash
void inserirNaHash(HashTable* tabela, char* pista, char* suspeito) {
    int indice = hash(pista);
    HashItem* novo = (HashItem*) malloc(sizeof(HashItem));
    strcpy(novo->pista, pista);
    strcpy(novo->suspeito, suspeito);
    novo->proximo = tabela->itens[indice];
    tabela->itens[indice] = novo;
}

// Busca o suspeito associado a uma pista
char* encontrarSuspeito(HashTable* tabela, char* pista) {
    int indice = hash(pista);
    HashItem* atual = tabela->itens[indice];
    while (atual) {
        if (strcmp(atual->pista, pista) == 0) {
            return atual->suspeito;
        }
        atual = atual->proximo;
    }
    return NULL;
}

// ------------------- FUNÇÕES DE PISTAS (BST) -------------------

// Insere uma pista na BST de pistas coletadas
PistaBST* inserirPista(PistaBST* raiz, char* pista) {
    if (!raiz) {
        PistaBST* nova = (PistaBST*) malloc(sizeof(PistaBST));
        strcpy(nova->pista, pista);
        nova->esquerda = nova->direita = NULL;
        return nova;
    }
    if (strcmp(pista, raiz->pista) < 0)
        raiz->esquerda = inserirPista(raiz->esquerda, pista);
    else if (strcmp(pista, raiz->pista) > 0)
        raiz->direita = inserirPista(raiz->direita, pista);
    return raiz;
}

// Lista as pistas em ordem
void listarPistasEmOrdem(PistaBST* raiz) {
    if (raiz) {
        listarPistasEmOrdem(raiz->esquerda);
        printf(" - %s\n", raiz->pista);
        listarPistasEmOrdem(raiz->direita);
    }
}

// Conta quantas pistas apontam para um suspeito
int contarPistasDoSuspeito(PistaBST* raiz, HashTable* tabela, char* suspeito) {
    if (!raiz) return 0;
    int conta = 0;
    char* s = encontrarSuspeito(tabela, raiz->pista);
    if (s && strcmp(s, suspeito) == 0) conta = 1;
    return conta +
           contarPistasDoSuspeito(raiz->esquerda, tabela, suspeito) +
           contarPistasDoSuspeito(raiz->direita, tabela, suspeito);
}

// ------------------- FUNÇÕES DA MANSÃO -------------------

// Cria uma nova sala com nome
Sala* criarSala(char* nome) {
    Sala* nova = (Sala*) malloc(sizeof(Sala));
    strcpy(nova->nome, nome);
    nova->esquerda = nova->direita = NULL;
    return nova;
}

// Retorna a pista associada a uma sala
char* obterPista(char* nomeSala) {
    if (strcmp(nomeSala, "Biblioteca") == 0) return "Pegada de barro";
    if (strcmp(nomeSala, "Cozinha") == 0) return "Faca ensanguentada";
    if (strcmp(nomeSala, "Sala de Estar") == 0) return "Copo quebrado";
    if (strcmp(nomeSala, "Quarto") == 0) return "Perfume forte";
    if (strcmp(nomeSala, "Porão") == 0) return "Luvas pretas";
    return NULL;
}

// Exploração interativa das salas da mansão
void explorarSalas(Sala* atual, PistaBST** pistas, HashTable* hash) {
    if (!atual) return;

    printf("\nVocê está na sala: %s\n", atual->nome);

    char* pista = obterPista(atual->nome);
    if (pista) {
        printf("Você encontrou uma pista: %s\n", pista);
        *pistas = inserirPista(*pistas, pista);
    }

    char escolha;
    printf("Escolha para onde ir (e: esquerda, d: direita, s: sair): ");
    scanf(" %c", &escolha);

    if (escolha == 'e') {
        explorarSalas(atual->esquerda, pistas, hash);
    } else if (escolha == 'd') {
        explorarSalas(atual->direita, pistas, hash);
    } else if (escolha == 's') {
        printf("Encerrando a exploração.\n");
    } else {
        printf("Opção inválida!\n");
        explorarSalas(atual, pistas, hash);
    }
}

// ------------------- JULGAMENTO FINAL -------------------

// Verifica se há evidências suficientes contra o suspeito acusado
void verificarSuspeitoFinal(PistaBST* pistas, HashTable* tabela) {
    char acusado[MAX_STR];
    printf("\n=== FASE FINAL: ACUSAÇÃO ===\n");
    printf("Pistas coletadas:\n");
    listarPistasEmOrdem(pistas);

    printf("\nDigite o nome do suspeito que deseja acusar: ");
    scanf(" %[^\n]", acusado);

    int evidencias = contarPistasDoSuspeito(pistas, tabela, acusado);
    printf("\nPistas que apontam para %s: %d\n", acusado, evidencias);

    if (evidencias >= 2) {
        printf("ACUSAÇÃO BEM-SUCEDIDA! %s é CULPADO(A)!\n", acusado);
    } else {
        printf("ACUSAÇÃO FRACA. %s provavelmente é inocente.\n", acusado);
    }
}

// ------------------- MAIN -------------------

int main() {
    // Construindo a mansão
    Sala* entrada = criarSala("Sala de Estar");
    entrada->esquerda = criarSala("Biblioteca");
    entrada->direita = criarSala("Cozinha");
    entrada->esquerda->esquerda = criarSala("Quarto");
    entrada->direita->direita = criarSala("Porão");

    // Inicializa estruturas de pistas
    PistaBST* pistasColetadas = NULL;

    // Criação da tabela hash
    HashTable* hash = (HashTable*) malloc(sizeof(HashTable));
    for (int i = 0; i < TAM_HASH; i++) hash->itens[i] = NULL;

    // Associação de pistas a suspeitos
    inserirNaHash(hash, "Pegada de barro", "Carlos");
    inserirNaHash(hash, "Faca ensanguentada", "Ana");
    inserirNaHash(hash, "Copo quebrado", "Marcos");
    inserirNaHash(hash, "Perfume forte", "Ana");
    inserirNaHash(hash, "Luvas pretas", "Carlos");

    printf("=== BEM-VINDO AO JOGO DE DETETIVE ===\n");
    printf("Explore a mansão e colete pistas para encontrar o culpado.\n");

    explorarSalas(entrada, &pistasColetadas, hash);
    verificarSuspeitoFinal(pistasColetadas, hash);

    return 0;
}

