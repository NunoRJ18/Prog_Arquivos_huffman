#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//==============Tipo de lista para a montagem da Arvore de Huffman==================//

typedef struct node {
    char letra;
    int frequencia;
    struct node* prox;
    struct node* esq;
    struct node* dir;
}node;

//==============Gera a frequencia dos Caracteres==================//

int* frequencia(char* input, int n) {

    //Inicializamos o vetor das frequencias com zeros e do tamanho do vetor dos caracteres

    int* freq = calloc(n, sizeof(int));

    FILE* f = fopen("entrada.txt", "r");

    //Contamos cada ocorrencia de cada caracter:

    char c;
    while (fscanf(f, "%c", &c) != EOF) {
        for (int i=0; i<n; i++) {
            if (c == input[i]) {
                freq[i]++;
                break;
            }
        }
    }
    return freq;
}

//==============Insere elementos na lista==================//

node* insere_lista(node* head, char c, int f) {
    node* aux = malloc(sizeof(node));
    aux->letra = c;
    aux->frequencia = f;
    aux->dir = NULL;
    aux->esq = NULL;
    aux->prox = head;
    return aux;
}

void imprime(node* head) {
    node* aux = head;

    while(aux!=NULL) {
        printf("%c:%d  ", aux->letra, aux->frequencia);
        aux = aux->prox;
    }
}

//==============Gera a Lista Ordenada==================//

node* monta_lista(char* input, int* freq, int n) {

    //Ordenando os vetores para a criação da lista:

    for(int i = 0; i < n; i++){
        int im = i;
        for(int j = i; j < n; j++)
            if(freq[j] < freq[im])
                im = j;

        int tmp = freq[i];
        char tmpc = input[i];

        freq[i] = freq[im];
        input[i] = input[im];

        freq[im] = tmp;
        input[im] = tmpc;
    }

    //Criação da lista ordenada:
    node* head = NULL;
    for (int i=n-1; i>=0; i--) {
        if (freq[i]!=0)
            head = insere_lista(head, input[i], freq[i]);
    }
    return head;
}

//==============Gera a Arvore de Huffman==================//

node* gera_arvore(node* head) {

    //Criando o nó auxiliar que recebe os dois nós com as menores frequencias:
    node* aux = malloc(sizeof(node));
    aux->letra = '+';
    aux->dir = head->prox;
    aux->esq = head;
    aux->frequencia = aux->dir->frequencia + aux->esq->frequencia;
    //Movendo o head caso existam mais de dois nós:
    if (head->prox->prox != NULL) {
        head = head->prox->prox;

        node* aux1 = head;

        while (aux1->prox != NULL && aux1->prox->frequencia <= aux->frequencia) {
            aux1 = aux1->prox;
        }
        if (aux1->prox != NULL) {
            aux->prox = aux1->prox;
            aux1->prox = aux;
            aux->dir->prox=NULL;
            aux->esq->prox=NULL;
        } else {
            aux->prox = NULL;
            aux1->prox = aux;
            aux->dir->prox=NULL;
            aux->esq->prox=NULL;
        }

        return head;
    } else {
        head = aux;
        head->prox = NULL;
        head->dir->prox=NULL;
        head->esq->prox=NULL;
        return head;
    }
}

void imprimir_pre_ordem(node* a){
    if(a!=NULL){
        printf("%d ",a->frequencia);
        imprimir_pre_ordem(a->esq);
        imprimir_pre_ordem(a->dir);
    }
}

//==============Altura da Arvore==================//

int altura(node* head) {
    int esq, dir;

    if (head==NULL)
        return -1;
    else {
        esq = altura(head->esq)+1;
        dir = altura(head->dir)+1;

        if (esq>dir)
            return esq;
        else
            return dir;
    }
}

//==============Aloca a Tabela de Codificação==================//

char** aloca_tabela(int colunas, int n) {
    char** tabela = malloc(n*sizeof(char*));

    for (int i=0; i<n; i++) {
         tabela[i] = calloc(colunas, sizeof(char));
    }
    return tabela;
}

//==============Gera a Tabela de Codificação==================//

void gera_tabela(node* head, char* input, char** tabela, int n, char* caminho, int colunas) {
    char esquerda[colunas], direita[colunas];

    if (head->esq==NULL && head->dir==NULL) {
        for (int i=0; i<n; i++) {
            if (head->letra == input[i]) {
                strcpy(tabela[i], caminho);
            }
        }
    } else{
        strcpy(esquerda, caminho);
        strcpy(direita, caminho);

        strcat(esquerda, "0");
        strcat(direita, "1");

        gera_tabela(head->esq, input, tabela, n, esquerda, colunas);
        gera_tabela(head->dir, input, tabela, n, direita, colunas);
    }
}

//==============Gera a Árvore de Huffman==================//

node* arvore_huffman(char* input, int* freq, int n) {

    //Primeiramente é necessária uma lista encadeada ordenada em relação as frequencias:
    node* head = monta_lista(input, freq, n);

    //Geração da arvore:
    while (head->prox != NULL) {
        head = gera_arvore(head);
    }
    return head;
}

void imprime_tabela(char** tabela, int* freq, char* input, int n) {
    puts("Tabela de Codificacao:");

    for (int i=0; i<n; i++) {
        if (freq[i]!=0)
            printf("\t%c: %s\n", input[i], tabela[i]);
    }
}

void codifica(char** tabela, char* input, int n) {
    FILE* e = fopen("entrada.txt", "r");
    FILE* s = fopen("codificado.txt", "w");

    //Passamos pelo arquivo codificamos os caracteres de acordo com a tabela,
    //escrevendo-os no arquivo de saida já codificados

    char c;

    while (fscanf(e, "%c", &c)!=EOF) {
        for (int i=0; i<n; i++) {
            if (input[i]==c) {
                fprintf(s, "%s", tabela[i]);
                break;
            }
        }
    }
    fclose(e);
    fclose(s);
}

void decodifica(node* head) {
    FILE* cod = fopen("codificado.txt", "r");
    FILE* dec = fopen("decodificado.txt", "w");

    //Para decodificarmos um arquivo comprimido pelo metodo de huffman, precisamos de sua tabela ou de sua arvore
    //Para a logica do programa, optei por utilizar a decodificacao pela arvore, por ser mais simples

    char c;
    node* aux = head;
    while (fscanf(cod, "%c", &c)!=EOF) {
        if (c == '0')
            aux = aux->esq;
        else
            aux = aux->dir;

        if (aux->esq == NULL && aux->dir == NULL) {
            fprintf(dec, "%c", aux->letra);
            aux = head;
        }
    }
}

int main(void) {
    int x=0;
    int n=62;           //n é o número de caracteres únicos que teremos no arquivo
    char input[n];

    //Incluindo os caracteres que serão comprimidos:

    for (int i=48; i<58; i++) {
        input[x] = i;
        x++;
    }

    for (int i=65; i<91; i++) {
        input[x] = i;
        x++;
    }

    for (int i=97; i<123; i++) {
        input[x] = i;
        x++;
    }

    //Ao todo temos os numeros de 0 a 9 e o alfabeto tanto maiúsculo quanto minúsculo

    //Obtemos a frequencia de cada caracter no arquivo:

    int* freq = frequencia(input, n);

    //Obtemos a arvore de huffman:

    node* head = arvore_huffman(input, freq, n);

    int colunas = altura(head)+1;

    //Alocamos e geramos nossa tabela de codificacao:

    char ** tabela = aloca_tabela(colunas, n);
    gera_tabela(head, input, tabela, n, "", colunas);
    imprime_tabela(tabela, freq, input, n);

    //Codificamos o texto em um arquivo (codificado)

    codifica(tabela, input, n);

    //Descondificamos o codigo em outro arquivo (doscodificado)

    decodifica(head);

}