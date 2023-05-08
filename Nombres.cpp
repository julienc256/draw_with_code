#define TOPLEFT      0
#define TOPMIDDLE    1
#define TOPRIGHT     2
#define CENTERLEFT   3
#define CENTER       4
#define CENTERRIGHT  5
#define BOTTOMLEFT   6
#define BOTTOMMIDDLE 7
#define BOTTOMRIGHT  8

private int Dessin::segments(char *chaine, int x, int y, int taille, uint color)
{
	// Taille = hauteur du caractère en nombre de pixels
	// Taille mini = 9
	double unite = taille / 18.0;
	int i,j;
	
	int x1, x2, x3, y1, y2, y3;
	
	while (*chaine != '\0')
	{
		switch *chaine:
		{
			case 'a':
				x1 = (int)round(unite)   + x;
				x2 = (int)round(unite*8) + x;
				y1 = y;
				y2 = (int)round(unite)   + y;
				y3 = (int)round(unite*2) + y;
				for (y=y1, y<y2; y++)
				{
					LigneHor(x1, x2, y, color);
					x1--;
					x2++;					
				}
				for (y=y2, y<=y3; y++)
				{
					LigneHor(x1, x2, y, color);
					x1++;
					x2--;					
				}
			break;
				
			case 'b';
				x1 = (int)round(unite*8)  + x;
				x2 = (int)round(unite*9)  + x;
				x3 = (int)round(unite*10) + x;
				y1 = (int)round(unite*2)  + y;
				y2 = (int)round(unite*8)  + y;
				for (x=x1; x<x2; x++)
				{
					LigneVer(y1, y2, x, color);
					y1--;
					y2++;
				}
				for (x=x2; x<=x3; x++)
				{
					LigneVer(y1, y2, x, color);
					y1++;
					y2--;					
				}
			break;
			
			case 'c':
			
			break;
				
			case 'd';
			
			break;
			
			case 'e':
			
			break;
				
			case 'f';
			
			break;
			
			case 'g':
			
			break;
		}
		chaine++;
	}
	
}


private int Dessin::chiffre(char c, int x, int y, int taille, uint color)
{
	char chaine[9];
	if (taille < 9)
		return 1;
	if (c > '9' || c < '0')
		return 1;
	
	//     a
	//   # # #
	// #       #
	// #f      #b
	// #   g   #
	//   # # #
	// #       #
	// #e      #c
	// #       #  dp
	//   # # #    #
	//     d
	
	switch (c)
	{
		case '0': strcpy(chaine, "abcdef");  break;
		case '1': strcpy(chaine, "bc");      break;
		case '2': strcpy(chaine, "abdeg");   break;
		case '3': strcpy(chaine, "abcdg");   break;
		case '4': strcpy(chaine, "bcfg");    break;
		case '5': strcpy(chaine, "acdfg");   break;
		case '6': strcpy(chaine, "acdef");   break;
		case '7': strcpy(chaine, "abc");     break;
		case '8': strcpy(chaine, "abcdefg"); break;
		case '9': strcpy(chaine, "abcdfg");  break;
	}
	segments(chaine, x, y, taille, align, color);
}

int Dessin::nombre(char *nombre, int x, int y, int taille, int align, uint color)
{
	char c;
	int i = 0;
	if (taille < 9)
		return 1;
	while ((c = *nombre) != '\0')
	{
		chiffre(c, x, y, taille);
		#TODO : décaler x !!!!
		nombre++;
	}
}

int Dessin::nombre(int nombre, int x, int y, int taille, int align, uint color)
{
	if (taille < 9)
		return 1;
	char str[11] = "";
	sprintf(str, "%d", nombre);
	nombre(str, x, y, taille, align, color);
}
