#include <time.h>   /* time */
#include <stdio.h>
#include <stdlib.h> /* srand, rand */
#include <string.h>
#include <math.h>
#include "Dessin.h"

//-----------------------------------------------------------------------------
// my_rand : retourne une valeur entière dans l'intervalle [0 ; 16777215]
//-----------------------------------------------------------------------------
int my_rand(void)
{
        static int first = 0;
        int randomValue;
        if (first == 0)
        {
                srand (time (NULL));
                first = 1;
        }
        randomValue = (int)(rand() / (double)RAND_MAX * (0x0FF));
        //randomValue = rand() % N;
   return randomValue;
}

//-----------------------------------------------------------------------------
// PROGRAMME PRINCIPAL
//-----------------------------------------------------------------------------
int main()
{
	int i;
	int WIDTH = 450;
	int HEIGHT = 450;
	Dessin *image = new Dessin(WIDTH, HEIGHT);
	if (!image->isOk())
	    return EXIT_FAILURE;
	for (i=4; i<=WIDTH/2; i+=8)
	{
		image->Ligne(WIDTH/2, i,          WIDTH/2+i, HEIGHT/2);
		image->Ligne(WIDTH/2, i,          WIDTH/2-i, HEIGHT/2);
		image->Ligne(WIDTH/2, i+HEIGHT/2, WIDTH-i,   HEIGHT/2);
		image->Ligne(WIDTH/2, i+HEIGHT/2, i,         HEIGHT/2);
	}
	image->enregistrerSous("A:/example2.bmp");
	delete image;
	return EXIT_SUCCESS;
}
