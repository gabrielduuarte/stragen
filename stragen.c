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

/*************/

int main(void)
{
    float b[LIN][DIM]={}, w[LIN][DIM]={};

    entrada(b);

    normaliza_entrada(b, w);

    return EXIT_SUCCESS;
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
                printf("w[%d][%d]: %.2f\n", i, j, w[i][j]);
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

