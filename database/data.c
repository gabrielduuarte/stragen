#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<allegro.h>

/* para compilar:
 *      gcc data.c -o data.x -Wall -Wextra -g -O0 `allegro-config --cflags --libs` -lm*/

#define RAIO1 1
#define RAIO2 1
#define CORBRANCO (makecol(255,255,255))
#define CORAZUL (makecol(0, 0, 255))
#define CORVERDE (makecol(0, 255, 0))
#define CONST (650/4.5)
#define MAXX 650
#define MAXY 650

int main(void)
{
    int ang1, ang2;
    double x1, y1, x2, y2=0;
    BITMAP *bmp;
    PALETTE pal;

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

    for(ang1=0; ang1<=345; ang1+=15)
    {
        for(ang2=-180; ang2<=180; ang2+=15)
        {
            x1 = RAIO1*cos((ang1*M_PI)/180);
            y1 = RAIO1*sin((ang1*M_PI)/180);
            x2 = x1+x1*cos((ang2*M_PI)/180) - y1*sin((ang2*M_PI)/180);
            y2 = x1*sin((ang2*M_PI)/180) + y1+y1*cos((ang2*M_PI)/180);
            circlefill(bmp, (MAXX/2)+x2*CONST, (MAXY/2)+y2*CONST, 5, CORBRANCO);
            circle(bmp, (MAXX/2)+x2*CONST, (MAXY/2)+y2*CONST, 5, CORAZUL);
            printf("%.3f %.3f %.3f %.3f %d %d\n", x1, y1, x2, y2, ang1, ang2);
        }
    }

    save_bitmap("pontos_iniciais.bmp", bmp, pal);
    destroy_bitmap(bmp);
    allegro_exit();

    return EXIT_SUCCESS;
}
END_OF_MAIN()

