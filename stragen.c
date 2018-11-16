#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<time.h>

/* Para compilar:
 *  gcc stragen.c -o stragen.x -Wall -Wextra -g -O0 -DDEBUG=1 */

#define LIN 600
#define DIM 6
#define Q 10

#ifndef DEBUG
#define DEBUG 0
#endif

typedef struct c_nodos
{
    int n;
    struct c_nodos *prox;
} t_nodos;

typedef struct c_conexoes
{
    int n1, n2;
    struct c_conexoes *prox;
} t_conexoes;

/*prototipos*/
void entrada(float b[LIN][DIM]);
void normaliza_entrada(float b[LIN][DIM], float w[LIN][DIM]);
float minmax(float b[LIN][DIM], int j, char vez);
void grupos_homogeneos(float w[LIN][DIM], float v1[LIN][4], float v2[LIN][2]);
void inicializa(t_nodos **nodos, t_conexoes **conexoes);
void insere_conexao(t_conexoes **conexoes, int x, int y);
void insere_nodo(t_nodos **nodos, int x);
void imprime(t_nodos *nodos, t_conexoes *conexoes);
void libera(t_nodos **nodos, t_conexoes **conexoes);
int balbuciamento_motor(int p, float w[LIN][DIM], int a[LIN], int t);

/*************/

int main(void)
{
    float b[LIN][DIM]={}, w[LIN][DIM]={};
    float v1[LIN][4], v2[LIN][2];
    int t=0;
    int i, a[LIN], p;
    t_nodos *nodos = NULL;
    t_conexoes *conexoes = NULL;
    srand(time(NULL));

    entrada(b);

    normaliza_entrada(b, w);

    inicializa(&nodos, &conexoes);
    /* ate aqui os numeros estao normalizados*/

    /* Wi = [wi1 ... wiD]; 1<=i<=L
     * Wi = [X1 Y1 X2 Y2 Ang1 Ang2]
     * V1 = [[X1 Y1] [X2 Y2]]
     * V2 = [[Ang1] [Ang2]]
     * Wi = [V1 V2]
     * Dividir o vetor W em 2 grupos homogenos v1, v2*/
    grupos_homogeneos(w, v1, v2);

    /* Selecionar o criterio de vizinhanca:
     * - Distancia euclidiana entre as posicoes Vn = v1 
     */

    for(i=0; i<LIN; i++)
        a[i] = 1;

    /*treinamento da rede*/
    while(t < LIN)
    {
        /*p == padrao; a == vetor utilizado para saber se o padrao ja foi utilizado, se ja recebe 0*/
        p = balbuciamento_motor(p, w, a, t); /* p == padrao, a == vetor utilizado para saber se o padrao*/

        t++;
    }

    if(DEBUG) imprime(nodos, conexoes);
    libera(&nodos, &conexoes);

    return EXIT_SUCCESS;
}

int balbuciamento_motor(int p, float w[LIN][DIM], int a[LIN], int t)
{
    int i, j;
    int aux, fi[Q];
    float d, min;

    if(t == 0)
        aux = rand()%LIN;
    else
    {
        for(i=0; i<Q; i++)
        {
            fi[i] = rand()%LIN;
            if(DEBUG) printf("f1[%d]:%d\n", i, fi[i]);
            while(a[fi[i]] == 0)
                fi[i] = rand()%LIN;

            if(DEBUG) printf("f2[%d]:%d\n", i, fi[i]);
        }
        for(i=0; i<Q; i++)
        {
            d = 0;
            for(j=0; j<DIM; j++)
            {
                if(DEBUG) printf("w[%d][%d]:%f - w[%d][%d]:%f\n", p, j, w[p][j], fi[i], j, w[fi[i]][j]);
                d += pow(w[p][j] - w[fi[i]][j], 2);
            }
            d = sqrt(d);
            if(DEBUG) printf("d[%d]:%f\n", t, d);
            if(i==0)
            {
                min = d;
                aux = fi[i];
            }
            if(d < min)
            {
                min = d;
                aux = fi[i];
            }
        }
    }
    a[aux] = 0; /*colocar que o indice aux ja foi utilizado*/

    return aux;
}

void libera(t_nodos **nodos, t_conexoes **conexoes)
{
    t_nodos *aux1 = *nodos, *ant1=NULL;
    t_conexoes *aux2 = *conexoes, *ant2=NULL;

    while(aux1!=NULL)
    {
        ant1 = aux1;
        aux1 = aux1->prox;
        free(ant1);
    }

    while(aux2!=NULL)
    {
        ant2 = aux2;
        aux2 = aux2->prox;
        free(ant2);
    }

}

void imprime(t_nodos *nodos, t_conexoes *conexoes)
{
    t_nodos *aux1 = nodos;
    t_conexoes *aux2 = conexoes;

    while(aux1!=NULL)
    {
        printf("%d -> ", aux1->n);
        aux1 = aux1->prox;
    }
    printf("NULL\n");

    while(aux2!=NULL)
    {
        printf("%d,%d -> ", aux2->n1, aux2->n2);
        aux2 = aux2->prox;
    }
    printf("NULL\n");
}

void inicializa(t_nodos **nodos, t_conexoes **conexoes)
{
    int i, j;

    i = rand()%LIN;
    j = rand()%LIN;

    insere_nodo(nodos, i);
    insere_nodo(nodos, j);

    insere_conexao(conexoes, i, j);

    return;
}

void insere_conexao(t_conexoes **conexoes, int x, int y)
{
    t_conexoes *aux = *conexoes;
    t_conexoes *ant = NULL;

    while(aux != NULL)
    {
        ant = aux;
        aux = aux->prox;
    }
    aux = malloc(sizeof(t_conexoes));
    aux->n1 = x;
    aux->n2 = y;
    aux->prox = NULL;

    if(ant == NULL)
        *conexoes = aux;
    else
        ant->prox = aux;

    return;
}

void insere_nodo(t_nodos **nodos, int x)
{
    t_nodos *aux = *nodos;
    t_nodos *ant = NULL;

    while(aux != NULL)
    {
        ant = aux;
        aux = aux->prox;
    }
    aux = malloc(sizeof(t_nodos));
    aux->n = x;
    aux->prox = NULL;

    if(ant == NULL)
        *nodos = aux;
    else
        ant->prox = aux;

    return;
}

void grupos_homogeneos(float w[LIN][DIM], float v1[LIN][4], float v2[LIN][2])
{
    int i, j;

    for(i=0; i<LIN; i++)
    {
        for(j=0; j<DIM; j++)
        {
            if(j<4)
            {
                v1[i][j] = w[i][j];
                if(DEBUG) printf("v1[%d][%d]: %.3f\n", i, j, v1[i][j]);
            }
            else
            {
                v2[i][j] = w[i][j];
                if(DEBUG) printf("v2[%d][%d]: %.3f\n", i, j, v2[i][j]);
            }

        }
    }
    return;

}

void normaliza_entrada(float b[LIN][DIM], float w[LIN][DIM])
{
    int i, j;
    float max, min;

    for(j=0; j<DIM; j++)
    {
        min = minmax(b, j, 'm');
        max = minmax(b, j, 'M');

        if(DEBUG) printf("min:%f | max:%f\n", min, max);

        for(i=0; i<LIN; i++)
            w[i][j] = (b[i][j] - min)/(max - min);
    }

    if(DEBUG)
    {
        for(i=0; i<LIN; i++)
        {
            for(j=0; j<DIM; j++) 
                printf("w[%d][%d]: %.3f\n", i, j, w[i][j]);
            printf("\n");
        }
    }

    return;
}

float minmax(float b[LIN][DIM], int j, char vez)
{
    int i;
    float minimax;

    minimax = b[0][j];
    for(i=0; i<LIN; i++)
    {
        if(vez == 'm' && b[i][j] < minimax)
            minimax = b[i][j];

        if(vez == 'M' && b[i][j] > minimax)
            minimax = b[i][j];
    }

    return minimax;
}

void entrada(float b[LIN][DIM])
{
    FILE *arq;
    int i, j;

    if((arq = fopen("input.txt", "r")) == NULL)
    {
        printf("Erro ao abrir o arquivo");
        exit(EXIT_FAILURE);
    }

    for(i=0; i<LIN; i++)
    {
        for(j=0; j<DIM; j++)
        {
            fscanf(arq, "%f", &b[i][j]);
            if(DEBUG)printf("%.3f ", b[i][j]);
        }
        if(DEBUG)printf("\n");
    }
    fclose(arq);

    return;
}

