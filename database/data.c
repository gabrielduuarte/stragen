#include<stdio.h>
#include<stdlib.h>
#include<math.h>

#define RAIO1 1
#define RAIO2 1

int main(void)
{
    int ang1, ang2;
    double x1, y1, x2, y2=0;

    for(ang1=0; ang1<=345; ang1+=15)
    {
        for(ang2=-180; ang2<=180; ang2+=15)
        {
            x1 = RAIO1*cos((ang1*M_PI)/180);
            y1 = RAIO1*sin((ang1*M_PI)/180);
            x2 = x1+x1*cos((ang2*M_PI)/180) - y1*sin((ang2*M_PI)/180);
            y2 = x1*sin((ang2*M_PI)/180) + y1+y1*cos((ang2*M_PI)/180);
            printf("x1: %.3f | y1: %.3f | x2: %.3f | y2: %.3f | ang1:%d | ang2:%d\n", x1, y1, x2, y2, ang1, ang2);
        }
    }


    return EXIT_SUCCESS;
}

