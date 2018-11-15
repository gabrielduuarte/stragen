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


void entrada(float b[LIN][DIM]);

int main(void)
{
    float b[LIN][DIM]={}, w[LIN][DIM]={};

    entrada(b);

    return EXIT_SUCCESS;
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

