# draw_with_code
Dessiner dans une image bitmap (format BMP) 24 bits, par la programmation !

# Introduction

Programme de dessin dans une image bitmap 24 bits par pixels (au format BMP) uniquement, pour des raisons de simplicité.

Libre ensuite à l'utilisateur de convertir l'image bmp vers d'autres formats (png, jpeg, etc...) avec un logiciels tiers tel que paint, Irfanview, etc ...

Objectif : dessiner par le code, ce qui permet de réaliser certaines choses difficilement réalisables avec un logiciel classique de dessin ! (voir exemples ci-dessous)

Le programme n'utilise aucune librairie hormis les librairies de base de la bibliothèque standard (stdio.h, stdlib.h, etc...). Pas de librairie graphique donc.

Le programme est donc portable sur n'importe quel système d'exploitation, il faut juste disposer d'un compilateur C++ de base.

Fan d'optimisation, l'accent a été mis sur la performance : **dessiner le plus rapidement possible** !

Voici quelques exemples ci-dessous (que j'étofferai au fur et à mesure)

# Exemples

## Lignes et formes de base

### Exemple 1

#### Code

	int main()
	{
		int WIDTH = 450;
		// On crée l'image en mémoire
		Dessin *image = new Dessin(WIDTH, WIDTH);
		if (!image->isOk())
			return EXIT_FAILURE;

		for (int i=10; i<=WIDTH; i+=10)
		{
			image->Ligne(1, i, i, WIDTH, colorBlack);
			image->Ligne(i, 1, WIDTH, i, colorBlack);
		}
		image->enregistrerSous("A:/example1.bmp");
		// On libère l'espace occupé par l'image en mémoire
		delete image;
		return EXIT_SUCCESS;
	}

#### Résultat

![exemple 1](/readme/example1.jpg "exemple1")

### Exemple 2

#### Code

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

#### Résultat

![exemple 2](/readme/example2.jpg "exemple2")

### Exemple 3

#### Code

	int main()
	{
		int WIDTH        = 800;
		int HEIGHT       = 450;
		int CARRE_WIDTH  = WIDTH/16  - 6;
		int CARRE_HEIGHT = HEIGHT/16 - 6;
		uint grayVal, colorR;
		Dessin *image = new Dessin(WIDTH, HEIGHT);
		if (!image->isOk())
			return EXIT_FAILURE;

		for (int y = 0; y < 16; y++)
		{
			for (int x = 0; x < 16; x++)
			{
				grayVal = 255-y*16-x;
				colorR = (uint)(grayVal<<24) + (uint)(grayVal<<16) + (uint)(grayVal<<8);
				image->RectangleEdgeAndFill(
				           x*WIDTH/16+4, y*HEIGHT/16+3,
				           x*WIDTH/16+4+CARRE_WIDTH, y*HEIGHT/16+3+CARRE_HEIGHT,
				           colorR, colorBlack);
			}
		}
		image->enregistrerSous("A:/example3.bmp");
		delete image;
		return EXIT_SUCCESS;
	}

#### Résultat

![exemple 3](/readme/example3.jpg "exemple3")

### Exemple 4

#### Code

	int main()
	{
		int WIDTH  = 600;
		int HEIGHT = 450;
		int rayon  = 200;
		int centreX = WIDTH>>1;
		int centreY = HEIGHT>>1;

		Dessin *image = new Dessin(WIDTH, HEIGHT);
		if (!image->isOk())
			return EXIT_FAILURE;

		image->erase(colorYellow);
		image->LigneHor  (centreX-rayon, centreX+rayon, centreY);
		image->LigneAngle(centreX, centreY, 5*PI/3, rayon);
		image->LigneAngle(centreX, centreY, 4*PI/3, rayon);
		image->LigneAngle(centreX, centreY, 2*PI/3, rayon);
		image->LigneAngle(centreX, centreY,   PI/3, rayon);
		image->ArcCercle (centreX, centreY,  rayon, 5*PI/3, 4*PI/3);
		image->ArcCercle (centreX, centreY,  rayon, 3*PI/3, 2*PI/3);
		image->ArcCercle (centreX, centreY,  rayon, PI/3, 0);

		image->Fill(240, 240, colorBlack, colorBlack);
		image->Fill(560, 240, colorBlack, colorBlack);
		image->Fill(centreX, 500, colorBlack, colorBlack);

		image->CercleFill(centreX, centreY, rayon/3-1, colorYellow);
		image->CercleFill(centreX, centreY, rayon/5, colorBlack);

		image->enregistrerSous("A:/example5.bmp");

		delete image;
		return EXIT_SUCCESS;
	}

#### Résultat

![exemple 4](/readme/example4.jpg "exemple4")

## Fractales

### Triangle de Sierpinsky

### Exemple 1

#### Code

	int main()
	{
		int WIDTH  = 800;
		int HEIGHT = 450;
		int centreX = WIDTH>>1;
		Dessin *image = new Dessin(WIDTH, HEIGHT);
		if (!image->isOk())
			return EXIT_FAILURE;

		uint Colors[8] = {colorGold, colorGreenYellow, colorSteelBlue, colorDarkViolet,
		                  colorLemonChiffon, colorLightSalmon, colorGold, colorGreenYellow};
		image->triangle_Sierpinsky(1, HEIGHT, WIDTH, HEIGHT, centreX, 1, 5, colorBlack, 1, 0, Colors);
		image->enregistrerSous("A:/example7.bmp");
		delete image;
		return EXIT_SUCCESS;
	}

#### Résultat

![exemple 5](/readme/example5.jpg "exemple5")

### Exemple 2

#### Code

	int main()
	{
		int WIDTH  = 800;
		int HEIGHT = 450;
		int centreX = WIDTH>>1;
		Dessin *image = new Dessin(WIDTH, HEIGHT);
		if (!image->isOk())
			return EXIT_FAILURE;

		// 10000 points
		image->triangle_SierpinskyPoints(1, HEIGHT, WIDTH, HEIGHT, centreX, 1, 10000);

		image->enregistrerSous("A:/example6.bmp");
		delete image;
		return EXIT_SUCCESS;
	}

#### Résultat

![exemple 6](/readme/example6.jpg "exemple6")

## Fonctions mathématiques

( à venir ...)

## Retouche de typons de circuits imprimés

( à venir ...)

# Compilation

( à venir ...)
