#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<time.h>
#include<allegro.h>

/* Para compilar:
 *  gcc stragen.c -o stragen.x -Wall -Wextra -g -O0 -DDEBUG=1 */

#define LIN 600
#define DIM 6
#define Q 10
#define P 0.01
#define CONST ((650/2)/0.5)
#define MAXX 650
#define MAXY 650
#define CORBRANCO (makecol(255,255,255))
#define CORAZUL (makecol(0, 0, 255))
#define CORVERDE (makecol(0, 255, 0))

#ifndef DEBUG
#define DEBUG 0
#endif

typedef struct c_nodos
{
    int n;
    int v;
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
int vencedor(float w[LIN][DIM], t_nodos *nodos, int p);
int vice(float w[LIN][DIM], t_nodos *nodos, int p, int s1);
void verifica_conexao(t_conexoes **conexoes, int s1, int s2);
void criterio_atividade(float a[2], float v1[LIN][4]);
void verifica_atividade(float ai[2], float v1[LIN][4], int p, int s1);
void remover_conexao(t_conexoes **conexoes, int s1, int s2);
void nova_conexao(t_conexoes **conexoes, float v1[LIN][4], int s3, int s1, int s2);
void allegro_fim(float w[LIN][DIM]);
void copia_conex(t_conexoes *conexoes, t_conexoes **new_conect);
void copia_nodo(t_nodos *nodos, t_nodos **new_nodos);
void remover_nodo(t_nodos **nodos, int x);

/*************/

int main(void)
{
    float b[LIN][DIM]={}, w[LIN][DIM]={}, wi[LIN][DIM]={};
    float v1[LIN][4], v2[LIN][2];
    float a[2]={}, ai[2]={};
    float learn=0.2, learnf=0.1, rho;
    int t=0, tmax=LIN, tok;
    int i, vp[LIN], p, venc[LIN]={}, j;
    int s1, s2, s3, sigmaf = 2*tmax/LIN;
    t_nodos *nodos = NULL, *new_nodos = NULL;
    t_conexoes *conexoes = NULL, *new_conect = NULL;
    srand(time(NULL));

    entrada(b);

    normaliza_entrada(b, w);
    for(i=0; i<LIN; i++)
        for(j=0; j<DIM; j++)
            wi[i][j] = w[i][j];

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

    /* Criterio de atividade Vs = v1
     * limiar de atividade: a[2]
     * percentual para atividade P = 1%*/
    criterio_atividade(a, v1);

    /*vetor utilizado para saber se o padrao foi usado:
     * se usado vp[i]=0;
     * senao vp[i]=1*/
    for(i=0; i<LIN; i++)
        vp[i] = 1;

    /*treinamento da rede*/
    while(t < tmax)
    {
        /*p == padrao; a == vetor utilizado para saber se o padrao ja foi utilizado, se ja recebe 0*/
        p = balbuciamento_motor(p, w, vp, t); /* p == padrao, vp == vetor utilizado para saber se o padrao foi usado*/

        /*escolhe o nodo vencedor quando comparado com o padrao*/
        s1 = vencedor(w, nodos, p);
        /*atualiza o numero de vitorias do vencedor*/
        venc[s1]+=1;
        /*escolhe o vice comparado com o padrao*/
        s2 = vice(w, nodos, p, s1);
        if(DEBUG) printf("p:%d, s1:%d, vez:%d, s2:%d\n", p, s1, venc[s1], s2);
        /*verifica se deve inserir uma nova conexao entre s1 e s2, se sim, a propria funcao ja insere*/
        verifica_conexao(&conexoes, s1, s2);
        /*verifica a atividade*/
        verifica_atividade(ai, v1, p, s1);
        tok=0;
        /*como sao dois subgrupos de v1 para verificar a atividade: 0<=i<2*/
        for(i=0; i<2; i++)
        {
            printf("ai[%d]:%f, a[%d]:%f\n", i, ai[i], i, a[i]);
            /*Passo 6: Se qualquer atividade for menor que o limiar estabelecido, entao add um novo nodo s3
             * na localizacao exata do vetor de entrada*/
            if(ai[i] < a[i])
            {
                s3 = p;
                insere_nodo(&nodos, s3);
                remover_conexao(&conexoes, s1, s2);
                /*insere 2 novas conexoes entre os nodos com menores distancias*/
                nova_conexao(&conexoes, v1, s3, s1, s2);
                /*token usado para saber se um novo nodo foi inserido no passo 6, se sim, tok=1*/
                tok=1;
                break;
            }
        }
        /*Passo 7: se um novo nodo nao foi inserido no passo 6, entao t==0*/
        if(tok==0)
        {
            /*deve-se atualizar o vetor de pesos do nodo vencedor s1*/
            if(venc[s1] <= sigmaf)
                rho = learn*pow(learnf, (venc[s1]/sigmaf));
            else
                rho = learn*learnf;
            printf("rho:%f\n", rho);

            for(i=0; i<6; i++)
            {
                printf("Antes w[%d][%d]: %f\n", s1, i, w[s1][i]);
                printf("w[%d][%d]:%f - w[%d][%d]:%f\n", p, i, w[p][i], s1, i, w[s1][i]);
                w[s1][i] = rho*(w[p][i] - w[s1][i]);
                printf("Depois w[%d][%d]: %f\n", s1, i, w[s1][i]);
            }

            for(i=0; i<LIN; i++)
                for(j=0; j<6; j++)
                {
                    if(j<4)
                        v1[i][j] = w[i][j];
                    else 
                        v2[i][j] = w[i][j];
                }
        }

        t++;
    }
   
    /* reseta o vetor vp */
    for(i=0; i<LIN; i++)
        vp[i] = 1;

    /* Fase de poda*/
    /* Passo 1: Crie novo conjunto de nodos e conexoes*/
    copia_nodo(nodos, &new_nodos);
    copia_conex(conexoes, &new_conect);

    /*Passo 2: repita para i=1,...,l iteracoes*/
    for(t=0; t<300; t++)
    {
        /* gere sinal de entrada */
        p = balbuciamento_motor(p, wi, vp, t);
        s1 = vencedor(wi, nodos, p);
        s2 = vice(wi, nodos, p, s1);
        /*Exclua nodo vencedor s1 do novo conjunto de nodos e exclua a conexao s1,s2*/
        remover_nodo(&new_nodos, s1);
        remover_conexao(&new_conect, s1, s2);
    }


    /*Passo 3: ainda estou trabalhando aqui ~nao reparem na bagunça~*/
    t_nodos *aux1 = nodos;
    t_nodos *aux2 = new_nodos;
    while(aux1 != NULL)
    {
        aux2 = new_nodos;
        while(aux2 != NULL)
        {
            if(aux1->n == aux2->n)
                remover_nodo(&nodos, aux1->n);
            aux2 = aux2->prox;
        }
        aux1 = aux1->prox;
    }

    /* imprime(nodos, conexoes); */
    /* imprime(new_nodos, new_conect); */
    libera(&nodos, &conexoes);
    libera(&new_nodos, &new_conect);

    return EXIT_SUCCESS;
}

void remover_nodo(t_nodos **nodos, int x)
{
    t_nodos *aux = *nodos;
    t_nodos *ant = NULL;

    printf("quero remover:%d\n", x);
    while(aux != NULL)
    {
        if(aux->n == x)
            break;
        ant = aux;
        aux = aux->prox;
    }
    if(aux == NULL)
        return;
    printf("achei?%d\n", aux->n);
    
    if(ant != NULL)
        ant->prox = aux->prox;
    else
        *nodos = aux->prox;

    free(aux);

    return;
}

void copia_nodo(t_nodos *nodos, t_nodos **new_nodos)
{
    t_nodos *aux=nodos;

    while(aux != NULL)
    {
        insere_nodo(new_nodos, aux->n);
        aux = aux->prox;
    }

    return;
}

void copia_conex(t_conexoes *conexoes, t_conexoes **new_conect)
{
    t_conexoes *aux = conexoes;

    while(aux != NULL)
    {
        insere_conexao(new_conect, aux->n1, aux->n2);
        aux = aux->prox;
    }

    return;
}


void allegro_fim(float w[LIN][DIM])
{
    BITMAP *bmp;
    PALETTE pal;
    int i, j;
    
    if(install_allegro(SYSTEM_NONE, &errno, atexit)!=0)
        exit(EXIT_FAILURE);

    set_color_depth(16);
    get_palette(pal);
    bmp = create_bitmap(MAXX,MAXY);
    if(bmp == NULL)
    {
        printf("Could not create buffer!\n");
        exit(EXIT_FAILURE);
    }

    vline(bmp, MAXX/2, 0, MAXY, CORVERDE);
    hline(bmp, 0, MAXY/2, MAXX, CORVERDE);
   
    j=2;
    for(i=0; i<LIN; i++)
    {
            circlefill(bmp, w[i][j]*CONST, w[i][j+1]*CONST, 5, CORBRANCO);
            circle(bmp, w[i][j]*CONST, w[i][j+1]*CONST, 5, CORAZUL);
    }
    save_bitmap("saida_b2.bmp", bmp, pal);
    destroy_bitmap(bmp);
    allegro_exit();

 
    return;
}

void nova_conexao(t_conexoes **conexoes, float v1[LIN][4], int s3, int s1, int s2)
{
    int i, j, m, v;
    float d, min, vic=1000;

    for(j=0; j<3; j++)
    {
        d=0;
        if(j==0)
        {
            for(i=0; i<4; i++)
                d += pow(v1[s3][i] - v1[s1][i], 2);
            d = sqrt(d);
            min = d;
            m=j;
        }

        if(j==1)
        {
            for(i=0; i<4; i++)
                d += pow(v1[s3][i] - v1[s2][i], 2);
            d = sqrt(d);
        }

        if(j==2)
        {
            for(i=0; i<4; i++)
                d += pow(v1[s1][i] - v1[s2][i], 2);
            d = sqrt(d);
        }
        if(d < min)
        {
            vic = min;
            v = m;
            min = d;
            m = j;
        }
        if(d > min && d < vic)
        {
            vic = d;
            v = j;
        }

        /* printf("d:%f\n", d); */
    }
    switch(m)
    {
        case 0:
            insere_conexao(conexoes, s3, s1);
            break;
        case 1:
            insere_conexao(conexoes, s3, s2);
            break;
        case 2:
            insere_conexao(conexoes, s1, s2);
            break;
    }

    switch(v)
    {
        case 0:
            insere_conexao(conexoes, s3, s1);
            break;
        case 1:
            insere_conexao(conexoes, s3, s2);
            break;
        case 2:
            insere_conexao(conexoes, s1, s2);
            break;
    }

    /* printf("min[%d]:%f, vic[%d]:%f\n", m, min, v, vic); */

    return;
}

void remover_conexao(t_conexoes **conexoes, int s1, int s2)
{
    t_conexoes *aux = *conexoes;
    t_conexoes *ant = NULL;

    printf("quero remover: %d,%d\n", s1, s2);
    while(aux != NULL)
    {
        if((aux->n1 == s1 && aux->n2 == s2) || (aux->n1 == s2 && aux->n2 == s1))
            break;
        ant = aux;
        aux = aux->prox;
    }
    if(aux == NULL)
        return;
    printf("achei? %d,%d\n", aux->n1, aux->n2);

    if(ant != NULL)
        ant->prox = aux->prox;
    else
        *conexoes = aux->prox;

    free(aux);

    return;
}

void verifica_atividade(float ai[2], float v1[LIN][4], int p, int s1)
{
    int i, j;
    float d;

    for(i=0; i<2; i++)
    {
        d=0;
        ai[i]=0;
        if(i==0)
        {
            for(j=0; j<2; j++)
                d += pow(v1[p][j] - v1[s1][j], 2);
            d = sqrt(d);
        }
        else
        {
            for(j=2; j<4; j++)
                d += pow(v1[p][j] - v1[s1][j], 2);
            d = sqrt(d);
        }
        ai[i] = exp(-d);
        if(DEBUG)printf("ai[%d]:%f\n", i, ai[i]);
    }

    return;
}

void criterio_atividade(float a[2], float v1[LIN][4])
{
    int i, j, z;
    float d, max;

    for(i=0; i<2; i++)
    {
        for(j=0; j<LIN; j++)
        {
            d=0;
            if(i==0)
            {
                for(z=0; z<2; z++)
                    d += pow(v1[j][z], 2);
                d = sqrt(d);
                if(j==0)
                    max = d;
                if(d > max)
                    max = d;
            }
            else
            {
                for(z=2; z<4; z++)
                    d += pow(v1[j][z], 2);
                d = sqrt(d);
                if(j==0)
                    max = d;
                if(d > max)
                    max = d;
            }
        }
        printf("max:%f\n", max);
        a[i] = exp(-P*sqrt(max));
        printf("a[%d]:%f\n", i, a[i]);
    }

    return;
}

void verifica_conexao(t_conexoes **conexoes, int s1, int s2)
{
    t_conexoes *aux = *conexoes;

    while(aux != NULL)
    {
        if((aux->n1 == s1 && aux->n2 == s2) || (aux->n1 == s2 && aux->n2 == s1))
            return;
        aux = aux->prox;
    }
    insere_conexao(conexoes, s1, s2);

    return;
}

int vice(float w[LIN][DIM], t_nodos *nodos, int p, int s1)
{
    int j, in;
    float d, win;
    t_nodos *aux = nodos;

    win = 100000;
    while(aux != NULL)
    {
        if(aux->n == s1)
        {
            aux = aux->prox;
            continue;
        }
        d=0;
        for(j=0; j<4; j++)
            d += pow(w[p][j] - w[aux->n][j], 2);
        d = sqrt(d);
        if(d <= win)
        {
            win = d;
            in = aux->n;
        }
        aux = aux->prox;
    }

    return in;
}

int vencedor(float w[LIN][DIM], t_nodos *nodos, int p)
{
    int j, in;
    float d, win;
    t_nodos *aux = nodos;

    win = 100000;
    while(aux != NULL)
    {
        d = 0;
        for(j=0; j<4; j++)
            d += pow(w[p][j] - w[aux->n][j], 2);
        d = sqrt(d);
        if(d < win)
        {
            win = d;
            in = aux->n;
        }
        aux = aux->prox;
    }

    return in;
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
    int n=0, c=0;

    while(aux1!=NULL)
    {
        printf("%d -> ", aux1->n);
        aux1 = aux1->prox;
        n++;
    }
    printf("NULL\n");
    printf("\n\nNODOS:%d\n", n);

    while(aux2!=NULL)
    {
        printf("%d,%d -> ", aux2->n1, aux2->n2);
        aux2 = aux2->prox;
        c++;
    }
    printf("NULL\n");
    printf("\n\nCONEXOES:%d\n", c);
}

void inicializa(t_nodos **nodos, t_conexoes **conexoes)
{
    int i, j;

    i = rand()%LIN;
    while((j = rand()%LIN) == i);

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
        /* if((aux->n1 == x && aux->n2 == y) || (aux->n1 == y && aux->n2 == x)) */
        /*     return; */
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
    aux->v = 0;
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

