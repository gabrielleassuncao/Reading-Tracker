//Dupla: Gabrielle Assunção e Geovana Silva

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    int id;
    char nome[100];
    int data, mes, ano;
    float avaliacao;
} Livro;

typedef struct no {
    Livro dado;
    struct no *ant;
    struct no *prox;
} No;

typedef struct {
    No *inicio;
    No *fim;
    int tam;
} ListaCDE;

// Funcoes de manipulacao
void cria(ListaCDE *l);
int insere(ListaCDE *l, Livro dado);
int insereOrdenado(ListaCDE *l, Livro dado);
int retira(ListaCDE *l, Livro dado);

// Funcoes de visualizacao
int estaVazia(ListaCDE l);
Livro getInicio(ListaCDE l);
Livro getFim(ListaCDE l);
int getTamanho(ListaCDE l);
void mostra(ListaCDE l);
void estrelas(float avaliacao);

// Funcoes de arquivo
int escreveLivro(FILE *f, Livro *livro);
int leLivro(FILE *f, Livro *livro);

int main() {
    ListaCDE l;
    Livro dado;
    int sucesso, opcao;
    int quantidade;
    int id = 0;
    char filename[50] = "livrosLidos.txt";

    cria(&l);

    FILE *f = fopen(filename, "a+");
    if (f == NULL) {
        printf("Falha em abrir arquivo... ");
        exit(1);
    }

    printf("--------------READING TRACKER--------------\n");
    printf("------------------------------------------\n\n");

    rewind(f);
    while (leLivro(f, &dado) > 0) {
        insereOrdenado(&l, dado);
        id = dado.id + 1; // Atualiza o próximo ID
    }

    do {
        printf("--------------MENU PRINCIPAL-------------\n");
        printf("1. Adicionar novo livro\n");
        printf("2. Remover livro\n");
        printf("3. Sair\n");
        printf("Escolha uma opcao: ");
        scanf("%d", &opcao);
        printf("------------------------------------------\n\n");
        system("cls");


        switch (opcao) {
            case 1:
                printf("Quantos livros voce deseja inserir em sua lista? ");
                scanf("%d", &quantidade);

                for (int i = 0; i < quantidade; i++) { // Insere o livro, data e geramos seu id
                    printf("\nDigite o nome do livro lido: ");
                    scanf(" %[^\n]s", dado.nome);
                    printf("\nDigite consecutivamente o dia, o mes e o ano de termino da leitura: ");
                    scanf("%d %d %d", &dado.data, &dado.mes, &dado.ano);

                    // Validando a avaliacao
                    do {
                        printf("\nDigite a avaliacao do livro (0 a 5): ");
                        scanf("%f", &dado.avaliacao);
                        if (dado.avaliacao < 0 || dado.avaliacao > 5) {
                            printf("\nAvaliacao invalida! Deve ser um numero entre 0 e 5.\n");
                        }
                    } while (dado.avaliacao < 0 || dado.avaliacao > 5);

                    dado.id = id++;
                    insereOrdenado(&l, dado);
                    escreveLivro(f, &dado); // Escreve o livro no arquivo
                }

                if (!estaVazia(l)) { // Exibindo a lista desde o primeiro elemento
                    printf("\n-------------------------------------------------\n");
                    mostra(l);
                }
                break;

            case 2:
                if (estaVazia(l)) {
                    printf("\nA lista está vazia, não há livros para remover.\n");
                    break;
                }
                mostra(l);
                printf("\nQual o id do livro que deseja remover? ");
                scanf("%d", &dado.id);
                sucesso = retira(&l, dado); // Remove o livro solicitado pelo usuario
                if (!sucesso)
                    printf("FALHA EM REMOVER: %d\n\n", dado.id);
                else {
                    printf("Livro removido com sucesso!\n");
                }
                mostra(l); // Exibe a lista atualizada
                break;

            case 3:
                printf("Saindo...\n");
                break;

            default:
                printf("Opcao invalida! Tente novamente.\n");
        }
    } while (opcao != 3);

    // Liberando memoria...
    while (!estaVazia(l)) {
        sucesso = retira(&l, getInicio(l)); // Retirando sempre o primeiro elemento
    }
    fclose(f);
    return 0;
}

void cria(ListaCDE *l) {
    l->inicio = NULL;
    l->fim = NULL;
    l->tam = 0;
}

int insereOrdenado(ListaCDE *l, Livro dado) {
    No *aux = (No *)malloc(sizeof(No));
    if (aux == NULL)
        return 0;

    aux->ant = NULL;
    aux->prox = NULL;
    aux->dado = dado;
    l->tam++;

    if (l->inicio == NULL) {
        l->inicio = aux;
        l->fim = aux;
        aux->ant = l->fim;
        aux->prox = l->inicio;
        return 1;
    }

    if (dado.id < l->inicio->dado.id) {
        aux->ant = l->fim;
        aux->prox = l->inicio;
        l->inicio->ant = aux;
        l->inicio = aux;
        l->fim->prox = l->inicio;
        return 1;
    }

    if (dado.id > l->fim->dado.id) {
        aux->ant = l->fim;
        aux->prox = l->inicio;
        l->fim->prox = aux;
        l->fim = aux;
        l->inicio->ant = l->fim;
        return 1;
    }

    No *anterior = l->inicio;
    No *atual = anterior->prox;

    while (dado.id > atual->dado.id && atual != l->inicio) {
        anterior = atual;
        atual = atual->prox;
    }

    aux->ant = anterior;
    aux->prox = atual;
    anterior->prox = aux;
    atual->ant = aux;

    return 1;
}

int retira(ListaCDE *l, Livro dado) {
    No *aux;

    if (l->inicio == NULL) {
        printf("\nLista vazia\n");
        return 0;
    }

    // Caso 1: lista unitaria
    if ((dado.id == l->inicio->dado.id) && (l->inicio == l->fim)) {
        aux = l->inicio; // aux aponta para o no a ser removido
        l->inicio = NULL; // inicio aponta para NULL
        l->fim = NULL; // fim aponta para NULL
        free(aux); // libera a memoria
        l->tam--;

        return 1;
    }

    // Caso 2: removendo primeiro elemento
    if (dado.id == l->inicio->dado.id) {
        aux = l->inicio; // aux aponta para o no que vou remover
        l->inicio = aux->prox; // inicio aponta para o novo inicio (o segundo elemento)
        l->inicio->ant = l->fim; // o anterior do novo inicio aponta para o fim
        l->fim->prox = l->inicio; // o proximo do fim aponta para o novo inicio
        free(aux); // libero memoria
        l->tam--;

        return 1;
    }

    // Caso 3: removendo ultimo elemento
    if (dado.id == l->fim->dado.id) {
        aux = l->fim; // aux aponta para o no que vou remover
        l->fim = aux->ant; // fim aponta para o novo fim (penultimo elemento)
        l->fim->prox = l->inicio; // o proximo do novo fim aponta para o inicio
        l->inicio->ant = l->fim; // o anterior do inicio aponta para o novo fim
        free(aux); // libero a memoria
        l->tam--;

        return 1;
    }

    // Caso 4: removendo um elemento no meio
    aux = l->inicio->prox; // aux aponta para o segundo elemento

    // Percorro a lista ate achar o elemento, ou chegar no fim
    while ((aux != l->inicio && dado.id != aux->dado.id)) {
        aux = aux->prox;
    }
    if (aux == l->inicio) { // Se cheguei ao fim, nao encontrei o elemento
        printf("\nO elemento %d nao esta na lista.\n", dado.id);
        return 0;
    }

    // Se encontrei o elemento, removo ele
    aux->prox->ant = aux->ant; // o anterior do pr�ximo a aux aponta para o elemento anterior a aux
    aux->ant->prox = aux->prox; // o pr�ximo do anterior a aux aponta para o pr�ximo elemento em rela��o a aux
    free(aux); // libero a mem�ria
    l->tam--;

    return 1;
}

int estaVazia(ListaCDE l) {
    return (l.inicio == NULL);
}

Livro getInicio(ListaCDE l) {
    return l.inicio->dado;
}

Livro getFim(ListaCDE l) {
    return l.fim->dado;
}

int getTamanho(ListaCDE l) {
    return l.tam;
}

void estrelas(float avaliacao) {
    // Convertendo a avalia��o num�rica em estrelas
    for (int i = 0; i < avaliacao; i++) {
        printf("*"); // Estrela preenchida
    }
    for (int i = avaliacao; i < 5; i++) {
        printf(" "); // Estrela vazia
    }

    printf("\n");
}

void mostra(ListaCDE l) {
    No *aux;

    if (l.inicio == NULL) { // lista vazia, n�o mostra nada
        printf("Lista vazia!\n");
        return;
    }

    aux = l.inicio;

    do {
        printf("\nID: %d\nTitulo: %s\nTermino: %d/%d/%d\nAvaliacao: \n", aux->dado.id, aux->dado.nome, aux->dado.data, aux->dado.mes, aux->dado.ano);
        estrelas(aux->dado.avaliacao);
        aux = aux->prox;
    } while (aux != l.inicio);
}

// Escreve os dados do livro no arquivo, retorna a quantidade de itens escritos
int escreveLivro(FILE *f, Livro *dado) {
     return fprintf(f, "%d %s %d %d %d %.2f\n", dado->id, dado->nome, dado->data, dado->mes, dado->ano, dado->avaliacao);
}

// Le os dados do livro do arquivo, retorna a quantidade de itens lidos
int leLivro(FILE *f, Livro *dado) {
    return fscanf(f, "%d %s %d %d %d %f\n", &dado->id, &dado->nome, &dado->data, &dado->mes, &dado->ano, &dado->avaliacao);
}
