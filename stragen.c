#include<stdio.h>
#include<stdlib.h>
#include<math.h>

/* Para compilar:
 *  gcc stragen.c -o stragen.x -Wall -Wextra -g -O0 -DDEBUG=1 */

#define LIN 600
#define DIM 6

#ifndef DEBUG
    #define DEBUG 0
#endif

/*prototipos*/
void entrada(float b[LIN][DIM]);
void normaliza_entrada(float b[LIN][DIM], float w[LIN][DIM]);
float minmax(float b[LIN][DIM], int j, char vez);
void grupos_homogeneos(float w[LIN][DIM], float v1[LIN][4], float v2[LIN][2]);

/*************/

int main(void)
{
    float b[LIN][DIM]={}, w[LIN][DIM]={};
    float v1[LIN][4], v2[LIN][2];

    entrada(b);

    normaliza_entrada(b, w);
    /*ate aqui os numeros estao normalizados*/
    
    /* Wi = [wi1 ... wiD]; 1<=i<=L
     * Wi = [X1 Y1 X2 Y2 Ang1 Ang2]
     * V1 = [[X1 Y1] [X2 Y2]]
     * V2 = [[Ang1] [Ang2]]
     * Wi = [V1 V2]
     * Dividir o vetor W em 2 grupos homogenos v1, v2*/
    grupos_homogeneos(w, v1, v2);



    return EXIT_SUCCESS;
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

