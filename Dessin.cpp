#include <time.h>   /* time */
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include "Dessin.h"

int Dessin::getCurX()
{
	return currentX;
}

int Dessin::getCurY()
{
	return currentY;
}

void Dessin::setCurX(int x)
{
	currentX = x;
}

void Dessin::setCurY(int y)
{
	currentY = y;
}

void Dessin::setCur(int x, int y)
{
    currentX = x;
	currentY = y;
}

void Dessin::setCurrentBgColor(uint color)
{
    currentBgColor = color;
}

void Dessin::setCurrentPenColor(uint color)
{
    currentPenColor = color;
}

void Dessin::setCurrentPenWidth(uint width)
{
    currentPenWidth = width;
}

void Dessin::setCurrentFontSize(uint fontSize)
{
    currentFontSize  = fontSize;
    //currentFontSpace = fontSize << 2;
}

void Dessin::setColorDelim(uint color)
{
    colorDelim = color;
}

/*void Dessin::setCurrentFontSpace(uint fontSpace)
{
    currentFontSpace = fontSpace;
}*/

int Dessin::isOk()
{
  return ok;
}

void Graph::setTraceRange(double xmin, double xmax, double ymin, double ymax)
{
    if (xmin >= xmax || ymin >= ymax)
        return;
    this->xmin = xmin;
    this->xmax = xmax;
    this->ymin = ymin;
    this->ymax = ymax;
}

Dessin::Dessin(uint width, uint height, uint bgColor, uint penColor, uint bitsPerPixel)
{
    ok = 0;
    this->width  = width;
    this->height = height;
    this->size   = width*height;
    this->currentBgColor  = bgColor;
    this->currentPenColor = penColor;
    currentFontSize = 36;
    //currentFontSpace = 9;

    // spécifique format BMP : chaque ligne doit comporter un nombre d'octets multiple de 4
    int ligne_width = width*3;
    if (ligne_width%4 != 0)
       ligne_width += 4 - ligne_width%4;

    // renseignement de file_header
    file_header.bfType = 'MB';
    file_header.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
    file_header.bfSize = file_header.bfOffBits + ligne_width*height;

    // renseignement de info_header
    info_header.biSize          = sizeof(BITMAPINFOHEADER);
    info_header.biWidth         = width;
    info_header.biHeight        = height;
    info_header.biPlanes        = (WORD)1;
    info_header.biBitCount      = (WORD)24;
    info_header.biCompression   = 0;
    info_header.biSizeImage     = file_header.bfSize - file_header.bfOffBits;
    info_header.biXPelsPerMeter = 2835;  // environ 72 dpi
    info_header.biYPelsPerMeter = 2835;  // environ 72 dpi
    info_header.biClrUsed       = 0;
    info_header.biClrImportant  = 0;

    tab = new uint[size];

    if (tab !=NULL)
    {
        for (uint i=0; i<size; i++)
        {
            tab[i] = bgColor;
        }
        BMP = new BYTE[info_header.biSizeImage];
        if (BMP != NULL)
            ok = 1;
    }
}

/*
  ENTREE : fileName
           Nom du fichier. Les antislash '\' doivent etre doublés si présents.
           Exemple : C:\\tests\\image01.bmp au lieu de C:\tests\image01.bmp
*/
Dessin::Dessin(const char *fileName)
{
    FILE *f;
    ok = 0;
    if ((f = fopen(fileName, "rb")))
    {
        // on lit le fichier
        fread(&file_header, sizeof(BITMAPFILEHEADER), 1, f);
        fread(&info_header, sizeof(BITMAPINFOHEADER), 1, f);

        // on effectue les verifications suivantes :
        // type BM, 24BPP, pas de compression, pas de palette, 1 seul plan
        if (file_header.bfType != 'MB')
        {
            printf("ERREUR : fichier '%s' non reconnu comme un BMP !\n", fileName);
        }
        else
        {
            if (info_header.biBitCount != 24 || info_header.biPlanes != 1 || info_header.biClrUsed
                || file_header.bfOffBits > sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER))
            {
                printf("ERREUR : l'image BMP '%s' n'est pas au format 24Bits (16.7M couleurs) !",
                       fileName);
            }
            else
            {
                if (info_header.biCompression)
                {
                    printf("ERREUR : l'image BMP '%s' est compressee (compression de type RLE) !",
                           fileName);
                }
                else
                {
                    info_header.biSizeImage = file_header.bfSize - file_header.bfOffBits;
                    BMP = new BYTE[info_header.biSizeImage];
                    if (BMP != NULL)
                    {
                        fread(BMP, info_header.biSizeImage, 1, f);
                        width  = info_header.biWidth;
                        height = info_header.biHeight;
                        size   = width*height;
                        currentFontSize = 36;
                        //currentFontSpace = 9;

                        tab = new uint[size];

                        int x, y;
                        uint index = 0;

                        // spécifique format BMP : chaque ligne doit comporter un nombre d'octets multiple de 4
                        int ligne_width = width*3;
                        if (ligne_width%4 != 0)
                           ligne_width += 4 - ligne_width%4;

                        for (y = height-1; y >=0; y--)
                        {
                            for (x = 0; x < width; x++)
                            {
                                tab[index]    = BMP[ligne_width * y + 3*x  ] << 8;
                                tab[index]   |= BMP[ligne_width * y + 3*x+1] << 16;
                                tab[index++] |= BMP[ligne_width * y + 3*x+2] << 24;
                            }
                        }
                        strcpy(this->fileName, fileName);
                        ok = 1;
                    }
                }
            }
        }
        fclose(f);
    }
    else
    {
        printf("Impossible d'ouvrir le fichier '%s' en lecture !\n", fileName);
    }
}

Dessin::~Dessin()
{
    delete tab;
    delete BMP;
}

void Dessin::erase(uint color)
{
    for (uint i=0; i<size; i++)
        tab[i] = color;
}

int Dessin::Point(int x, int y, uint penColor)
{
    if (x > width || x < 1 || y > height || y < 1)
    {
        printf("Erreur Point : hors zone\n");
        return EXIT_FAILURE;
    }
    x--;
    y--;
    tab[width * y + x] = penColor;
    return EXIT_SUCCESS;
}

int Dessin::MoveAngle(double angle, int len)
{
    int x_ = currentX + (int)round(len * cos(angle));
    int y_ = currentY - (int)round(len * sin(angle));
    if (x_ < 1 || x_ > width || y_ < 1 || y_ > height)
    {
        printf("Erreur MoveAngle : hors zone\n");
        return EXIT_FAILURE;
    }
    currentX = x_;
    currentY = y_;
    return EXIT_SUCCESS;
}

int Dessin::Point(int x, int y)
{
    return Point(x, y, currentPenColor);
}


int Dessin::Ligne(int x1, int y1, int x2, int y2, uint penColor, uint penWidth)
{
    int x;
    int y;
    if (x1 > width || x1 < 1 || x2 > width || x2 < 1 || y1 > height || y1 < 1 || y2 > height || y2 < 1)
    {
        printf("Erreur Ligne(%d, %d, %d, %d, ...) : parametres incorrectes\n", x1, y1, x2, y2);
        return EXIT_FAILURE;
    }
    else if (y1 == y2)
    {
        return LigneHor(x1, x2, y1, penColor, penWidth);
    }
    else if (x1 == x2)
    {
        return LigneVer(x1, y1, y2, penColor, penWidth);
    }
    else
    {

        x1--;
        x2--;
        y1--;
        y2--;
        if (ABS(x2-x1) >= ABS(y2-y1))
        {
            int xmin = MIN(x1, x2);
            int xmax = MAX(x1, x2);
            double delta = (double)(y2 - y1) / (x2 - x1); // delta compris entre 0 et 1
            double y_;
			if (x1 == xmin)
				y_ = (double)y1;
            else
                y_ = (double)y2;
            for (x = xmin; x <= xmax; x++)
            {
                tab[width * (uint)(round(y_)) + x] = penColor;
                y_ += delta;
            }
        }
        else
        {
            int ymin = MIN(y1, y2);
            int ymax = MAX(y1, y2);
            double delta = (double)(x2 - x1) / (y2 - y1); // delta compris entre 0 et 1

            double x_;
            if (y1 == ymin)
                x_ = (double)x1;
            else
                x_ = (double)x2;
            for (y = ymin; y <= ymax; y++)
            {
                tab[width * y + (uint)round(x_)] = penColor;
                x_ += delta;
            }
        }
        currentX = x2+1;
        currentY = y2+1;
        return EXIT_SUCCESS;
    }
}

int Dessin::Ligne(int x1, int y1, int x2, int y2)
{
    return Ligne(x1, y1, x2, y2, currentPenColor, currentPenWidth);
}

int Dessin::LigneAngle(int x1, int y1, double angle, int longueur, uint penColor)
{
    int x2 = x1 + (int)round(longueur * cos(angle));
    int y2 = y1 - (int)round(longueur * sin(angle));
    return Ligne(x1, y1, x2, y2, penColor);
}

int Dessin::LigneAngle(int x, int y, double angle, int longueur)
{
    return LigneAngle(x, y, angle, longueur, currentPenColor);
}


int Dessin::LigneHor(int x1, int x2, int y, uint penColor, uint penWidth)
{
    if (x1 > width || x1 < 1 || x2 > width || x2 < 1 || y > height || y < 1 || x1 == x2)
        return EXIT_FAILURE;
    x1--;
    x2--;
    y--;
    int xmin  = MIN(x1, x2);
    int xmax  = MAX(x1, x2);
    int start = width * y + xmin;
    int end   = width * y + xmax;
    for (int x = start; x <= end; x++)
        tab[x] = penColor;
    currentY = y+1;
    currentX = x2+1;
    return EXIT_SUCCESS;
}

int Dessin::LigneHor(int x1, int x2, int y)
{
    return LigneHor(x1, x2, y, currentPenColor, currentPenWidth);
}

int Dessin::LigneVer(int x, int y1, int y2, uint penColor, uint penWidth)
{
    if (x > width || x < 1 || y1 > height || y1 < 1 || y2 > height || y2 < 1 || y1 == y2)
        return EXIT_FAILURE;
    y1--;
    y2--;
    x--;
    int ymin = MIN(y1, y2);
    int ymax = MAX(y1, y2);
    int indice = width * ymin + x;
    for (int y = ymin; y <= ymax; y++)
    {
        tab[indice] = penColor;
        indice += width;
    }
    currentX = x+1;
    currentY = y2+1;
    return EXIT_SUCCESS;
}

int Dessin::LigneVer(int x, int y1, int y2)
{
    return LigneVer(x, y1, y2, currentPenColor, currentPenWidth);
}

int Dessin::Rectangle(int x1, int y1, int x2, int y2, uint penColor, uint penWidth)
{
    if ((x1 > width || x1 < 1 || x2 > width || x2 < 1 || y1 > height || y1 < 1 || y2 > height ||
        y2 < 1 || x1 == x2) && (y1 == y2))
        return EXIT_FAILURE;
    LigneHor(x1, x2, y1, penColor, penWidth);
    LigneHor(x1, x2, y2, penColor, penWidth);
    LigneVer(x1, y1, y2, penColor, penWidth);
    LigneVer(x2, y1, y2, penColor, penWidth);
    return EXIT_SUCCESS;
}

int Dessin::Rectangle(int x1, int y1, int x2, int y2)
{
    return Rectangle(x1, y1, x2, y2, currentPenColor, currentPenWidth);
}

int Dessin::RectangleFill(int x1, int y1, int x2, int y2, uint color)
{
    if ((x1 > width || x1 < 1 || x2 > width || x2 < 1 || y1 > height || y1 < 1 || y2 > height ||
        y2 < 1 || x1 == x2) && (y1 == y2))
    {
        printf("ERREUR RectangleFill(%d,%d,%d,%d,...) : coordonnées incorrectes\n", x1, y1, x2, y2);
        return EXIT_FAILURE;
    }

    x1--;
    x2--;
    y1--;
    y2--;
    int x, y;
    int ymin = MIN(y1, y2);
    int ymax = MAX(y1, y2);
    int xmin = MIN(x1, x2);
    int xmax = MAX(x1, x2);
    uint pos = ymin * width;
    for (y = ymin; y <= ymax; y++)
    {
        for (x=xmin; x <= xmax; x++)
        {
            tab[pos+x] = color;
        }
        pos += width;
    }
    currentX = x-1;
    currentY = y-1;
    return EXIT_SUCCESS;
}

int Dessin::RectangleFill(int x1, int y1, int x2, int y2)
{
    return RectangleFill(x1, y1, x2, y2, currentBgColor);
}


int Dessin::RectangleEdgeAndFill(int x1, int y1, int x2, int y2, uint bgColor, uint penColor,
                                 uint penWidth)
{
    if (RectangleFill(x1, y1, x2, y2, bgColor) == EXIT_SUCCESS)
        return Rectangle(x1, y1, x2, y2, penColor, penWidth);
    return EXIT_FAILURE;
}

int Dessin::RectangleEdgeAndFill(int x1, int y1, int x2, int y2)
{
    return RectangleEdgeAndFill(x1, y1, x2, y2, currentBgColor, currentPenColor, currentPenWidth);
}

int Dessin::Polygon(int centreX, int centreY, int rayon, int nbFaces, uint color, int initPosition,
                    double rotationAngle)
{
    if (centreX < 1 || centreY < 1 || rayon < 4 || nbFaces < 3 || (centreX+rayon) > width ||
        (centreY+rayon) > height)
    {
        printf("Erreur Polygon(%d, %d, %d, %d,...) : Coordonnées incorrectes\n", centreX, centreY, rayon, nbFaces);
        return EXIT_FAILURE;
    }

    double stepAngle = M_PI * 2 / nbFaces;
    double curAngle = rotationAngle - M_PI / 2.0;
    if (initPosition == 1)
        curAngle += M_PI / nbFaces;

    int x1, y1, x2, y2;

    for (int nb = 0; nb <= nbFaces; nb++)
    {
        if (nb == 0)
        {
            x1 = (int)round((cos(curAngle) * rayon + centreX));
            y1 = (int)round((sin(curAngle) * rayon + centreY));
            curAngle += stepAngle;
            continue;
        }
        x2 = (int)round((cos(curAngle) * rayon + centreX));
        y2 = (int)round((sin(curAngle) * rayon + centreY));

        Ligne(x1, y1, x2, y2, color);

        x1 = x2;
        y1 = y2;
        curAngle += stepAngle;
    }
    return EXIT_SUCCESS;
}

int Dessin::Cercle(int centreX, int centreY, int rayon)
{
    return Cercle(centreX, centreY,rayon, currentPenColor);
}

int Dessin::Cercle(int centreX, int centreY, int rayon, uint color)
{
    if ( (centreX-rayon) < 1 || (centreY-rayon) < 1 || rayon < 1 || (centreX+rayon) > width || (centreY+rayon) > height)
    {
        printf("Erreur Cercle(%d, %d, %d, ...) : coordonnées incorrectes\n", centreX, centreY, rayon);
        return EXIT_FAILURE;
    }

    centreX--;
    centreY--;
    //int x[8], y[8];
    int x1, y1, x2, y2, x3, y3, x4, y4, x5, y5, x6, y6, x7, y7, x8, y8;
    // Dessin de l'arc de cercle sur l'intervalle d'angle [-PI/4 ; PI/2], ce qui équivaut à
    // x € [racine(2)/2;0] puis extension par symétries successives, à des fins d'optimisation
    for (x1 = - (int)round((rayon * sqrt(2)/2)); x1 <= 0; x1++)
    {
        double cos_a = (double) x1 / rayon;

        // Rappel : cos² a + sin² a = 1
        double sin_a = sqrt(1 - cos_a * cos_a);

        y1 = (int)round((sin_a * rayon));

        // symétrie par rapport à un axe oblique
        x2 =  y1; y2 = x1;

        // symétrie par rapport à un axe vertical
        x3 = -x1; y3 = y1; x4 = -x2; y4 = y2;

        // symétrie par rapport à un axe horizontal
        x5 = x1; y5 = -y1; x6 = x2; y6 = -y2;
        x7 = x3; y7 = -y3; x8 = x4; y8 = -y4;

        tab[width * (centreY - y1) + centreX + x1] =
        tab[width * (centreY - y2) + centreX + x2] =
        tab[width * (centreY - y3) + centreX + x3] =
        tab[width * (centreY - y4) + centreX + x4] =
        tab[width * (centreY - y5) + centreX + x5] =
        tab[width * (centreY - y6) + centreX + x6] =
        tab[width * (centreY - y7) + centreX + x7] =
        tab[width * (centreY - y8) + centreX + x8] = color;
    }
    return EXIT_SUCCESS;
}

int Dessin::CercleFill(int centreX, int centreY, int rayon)
{
    return CercleFill(centreX, centreY, rayon, currentPenColor);
}

int Dessin::CercleFill(int centreX, int centreY, int rayon, uint color)
{
    if ( (centreX-rayon) < 1 || (centreY-rayon) < 1 || rayon < 1 || (centreX+rayon) > width || (centreY+rayon) > height)
    {
        printf("Erreur CercleFill(%d, %d, %d, ...) : coordonnées incorrectes\n", centreX, centreY, rayon);
        return EXIT_FAILURE;
    }

    centreX--;
    centreY--;
    //int x[8], y[8];
    int x1, y1, x2, y2, x3, x4, y4, x5, y5, x6, x7, x8, y8;
    register int x, start, end;
    // Dessin de l'arc de cercle sur l'intervalle d'angle [-PI/4 ; PI/2], ce qui équivaut à
    // x € [racine(2)/2;0] puis extension par symétries successives, à des fins d'optimisation
    for (x1 = - (int)round((rayon * sqrt(2)/2)); x1 <= 0; x1++)
    {
        double cos_a = (double) x1 / rayon;

        // Rappel : cos² a + sin² a = 1
        double sin_a = sqrt(1 - cos_a * cos_a);

        y1 = (int)round((sin_a * rayon));

        // symétrie par rapport à un axe oblique
        x2 =  y1; y2 = x1;

        // symétrie par rapport à un axe vertical
        x3 = -x1; x4 = -x2; y4 = y2;

        // symétrie par rapport à un axe horizontal
        x5 = x1; y5 = -y1; x6 = x2;
        x7 = x3; x8 = x4; y8 = -y4;

        start = width * (centreY - y1) + centreX + x1;
        end   = start - x1 + x3;
        for (x = start ; x <= end; x++)
            tab[x] = color;

        start = width * (centreY - y4) + centreX + x4;
        end   = start - x4 + x2;
        for (x = start ; x <= end; x++)
            tab[x] = color;

        start = width * (centreY - y5) + centreX + x5;
        end   = start - x5 + x7;
        for (x = start ; x <= end; x++)
            tab[x] = color;

        start = width * (centreY - y8) + centreX + x8;
        end   = start - x8 + x6;
        for (x = start ; x <= end; x++)
            tab[x] = color;
    }
    return EXIT_SUCCESS;
}

int Dessin::CercleEdgeAndFill(int centreX, int centreY, int rayon)
{
    return CercleEdgeAndFill(centreX, centreY, rayon, currentBgColor, currentPenColor);
}

int Dessin::CercleEdgeAndFill(int centreX, int centreY, int rayon, uint bgColor, uint penColor)
{
    if (CercleFill(centreX, centreY, rayon, bgColor) == EXIT_SUCCESS)
        return Cercle(centreX, centreY, rayon, penColor);
    return EXIT_FAILURE;
}

int Dessin::ArcCercle(int x, int y, int rayon, double angleStart, double angleEnd)
{
    return ArcCercle(x, y, rayon, angleStart, angleEnd, currentPenColor);
}

int Dessin::ArcCercle(int centreX, int centreY, int rayon, double angleStart, double angleEnd, uint penColor)
{
    // TODO : supprimer test (centreX+rayon) > width || (centreY+rayon) > height) à la fin
    if ( (centreX-rayon) < 1 || (centreY-rayon) < 1 || rayon < 1 || (centreX+rayon) > width || (centreY+rayon) > height)
    {
        printf("Erreur ArcCercle(%d, %d, %d, %lf, %lf,...) : coordonnées incorrectes\n", centreX, centreY, rayon, angleStart, angleEnd);
        return EXIT_FAILURE;
    }

    centreX--;
    centreY--;

    double x1, x2, y1, y2;
    x1 = cos(angleStart);
    x2 = cos(angleEnd);
    y1 = sin(angleStart);
    y2 = sin(angleEnd);

    if (x1==x2 && y1==y2)
        return EXIT_FAILURE;

    double cos_PI4 = sqrt(2)/2; //cos(PI/4);
    int zoneDep, zoneFin;
    int zone1 =0, zone2=0, zone3=0, zone4=0;

    if (x1 > cos_PI4)       zoneDep = 2;
    else if (x1 < -cos_PI4) zoneDep = 4;
    else if (y1 > 0)        zoneDep = 1;
    else                    zoneDep = 3;

    if (x2 > cos_PI4)       zoneFin = 2;
    else if (x2 < -cos_PI4) zoneFin = 4;
    else if (y2 > 0)        zoneFin = 1;
    else                    zoneFin = 3;

#define FIRSTZONE        1
#define ENDZONE          2
#define FIRSTANDENDZONE  3
#define UNIQUEZONE       4
#define TOTALZONE        5

    if (zoneDep==1)
    {
        zone1 = FIRSTZONE;
        if (zoneFin==1)
        {
            if (x2 < x1) { zone2 = zone3 = zone4 = TOTALZONE; zone1 = FIRSTANDENDZONE; }
            else zone1 = UNIQUEZONE;
        }
        else if (zoneFin == 2) zone2 = ENDZONE;
        else if (zoneFin == 3) { zone2 = TOTALZONE; zone3 = ENDZONE; }
        else { zone2 = TOTALZONE; zone3 = TOTALZONE; zone4 = ENDZONE;}
    }
    else if (zoneDep==2)
    {
        zone2 = FIRSTZONE;
        if (zoneFin==2)
        {
            if (y2 > y1)
            {
                zone3 = zone4 = zone1 = TOTALZONE;
                zone2 = FIRSTANDENDZONE;
            }
            else
                zone2 = UNIQUEZONE;
        }
        else if (zoneFin == 3)
            zone3 = ENDZONE;
        else if (zoneFin == 4)
        {
            zone3 = TOTALZONE;
            zone4 = ENDZONE;
        }
        else //(zoneFin == 1)
        {
            zone3 = TOTALZONE;
            zone4 = TOTALZONE;
            zone1 = ENDZONE;
        }
    }
    else if (zoneDep==3)
    {
        zone3 = FIRSTZONE;
        if (zoneFin==3)
        {
            if (x2 > x1)
            {
                zone4 = zone1 = zone2 = TOTALZONE;
                zone3 = FIRSTANDENDZONE;
            }
            else
                zone3 = UNIQUEZONE;
        }
        else if (zoneFin == 4)
            zone4 = ENDZONE;
        else if (zoneFin == 1)
        {
            zone4 = TOTALZONE;
            zone1 = ENDZONE;
        }
        else //(zoneFin == 2)
        {
            zone4 = TOTALZONE;
            zone1 = TOTALZONE;
            zone2 = ENDZONE;
        }
    }
    else if (zoneDep==4)
    {
        zone4 = FIRSTZONE;
        if (zoneFin==4)
        {
            if (y2 < y1)
            {
                zone1 = zone2 = zone3 = TOTALZONE;
                zone4 = FIRSTANDENDZONE;
            }
            else
                zone4 = UNIQUEZONE;
        }
        else if (zoneFin == 1)
            zone1 = ENDZONE;
        else if (zoneFin == 2)
        {
            zone1 = TOTALZONE;
            zone2 = ENDZONE;
        }
        else //(zoneFin == 3)
        {
            zone1 = TOTALZONE;
            zone2 = TOTALZONE;
            zone3 = ENDZONE;
        }
    }

    // Dessin de l'arc de cercle
    double xmin, xmax, ymin, ymax;
    int iXmin, iXmax, iYmin, iYmax, iX, iY;
    if (zone1)
    {
        if (zone1 == FIRSTZONE)
        {
            xmin = x1;
            xmax = cos_PI4;
        }
        else if (zone1 == UNIQUEZONE)
        {
            xmin = x1;
            xmax = x2;
        }
        else if (zone1 == ENDZONE)
        {
            xmin = -cos_PI4;
            xmax = x2;
        }
        else if (zone1 == FIRSTANDENDZONE)
        {
            xmin = -cos_PI4;
            xmax = x2;
        }
        else // zone1 == TOTAL
        {
            xmin = -cos_PI4;
            xmax = cos_PI4;
        }

        int nb_iterations = (zone1==FIRSTANDENDZONE)?2:1;
        while (nb_iterations)
        {
            iXmin = (int)round(rayon*xmin);
            iXmax = (int)round(rayon*xmax);

            for (iX =iXmin; iX<=iXmax; iX++)
            {
                double cos_a = (double) iX / rayon;
                // Rappel : cos² a + sin² a = 1
                double sin_a = sqrt(1 - cos_a * cos_a);
                iY = (int)round((sin_a * rayon));
                tab[width * (centreY - iY) + centreX + iX] = penColor;
            }
            if (zone1==FIRSTANDENDZONE)
            {
                xmin = x1;
                xmax = cos_PI4;
            }
            nb_iterations--;
        }
    }
    if (zone3)
    {
        if (zone2 == FIRSTZONE)
        {
            xmin = -cos_PI4;
            xmax = x2;
        }
        else if (zone3 == UNIQUEZONE)
        {
            xmin = x2;
            xmax = x1;
        }
        else if (zone3 == ENDZONE)
        {
            xmin = x2;
            xmax = cos_PI4;
        }
        else if (zone3 == FIRSTANDENDZONE)
        {
            xmin = -cos_PI4;
            xmax = x1;
        }
        else // zone1 == TOTAL
        {
            xmin = -cos_PI4;
            xmax = cos_PI4;
        }
        int nb_iterations = (zone1==FIRSTANDENDZONE)?2:1;
        while (nb_iterations)
        {
            iXmin = (int)round(rayon*xmin);
            iXmax = (int)round(rayon*xmax);

            for (iX =iXmin; iX<=iXmax; iX++)
            {
                double cos_a = (double) iX / rayon;
                // Rappel : cos² a + sin² a = 1
                double sin_a = sqrt(1 - cos_a * cos_a);
                iY = (int)round((sin_a * rayon));
                tab[width * (centreY + iY) + centreX + iX] = penColor;
            }
            if (zone1==FIRSTANDENDZONE)
            {
                xmin = x2;
                xmax = cos_PI4;
            }
            nb_iterations--;
        }
    }
    if (zone2)
    {
        if (zone2 == FIRSTZONE)
        {
            ymin = -cos_PI4;
            ymax = y1;
        }
        else if (zone2 == UNIQUEZONE)
        {
            ymin = y2;
            ymax = y1;
        }
        else if (zone2 == ENDZONE)
        {
            ymin = y2;
            ymax = cos_PI4;
        }
        else if (zone2 == FIRSTANDENDZONE)
        {
            ymin = y2;
            ymax = cos_PI4;
        }
        else // zone1 == TOTAL
        {
            ymin = -cos_PI4;
            ymax = cos_PI4;
        }
        int nb_iterations = (zone1==FIRSTANDENDZONE)?2:1;
        while (nb_iterations)
        {
            iYmin = (int)round(rayon*ymin);
            iYmax = (int)round(rayon*ymax);

            for (iY=iYmin; iY<=iYmax; iY++)
            {
                double cos_a = (double) iY / rayon;
                // Rappel : cos² a + sin² a = 1
                double sin_a = sqrt(1 - cos_a * cos_a);
                iX = (int)round((sin_a * rayon));
                tab[width * (centreY - iY) + centreX + iX] = penColor;
            }
            if (zone1==FIRSTANDENDZONE)
            {
                ymin = -cos_PI4;
                ymax = y1;
            }
            nb_iterations--;
        }
    }
    if (zone4)
    {
        if (zone4 == FIRSTZONE)
        {
            ymin = y1;
            ymax = cos_PI4;
        }
        else if (zone4 == UNIQUEZONE)
        {
            ymin = y1;
            ymax = y2;
        }
        else if (zone4 == ENDZONE)
        {
            ymin = -cos_PI4;
            ymax = y2;
        }
        else if (zone4 == FIRSTANDENDZONE)
        {
            ymin = -cos_PI4;
            ymax = y2;
        }
        else // zone1 == TOTAL
        {
            ymin = -cos_PI4;
            ymax = cos_PI4;
        }
        int nb_iterations = (zone1==FIRSTANDENDZONE)?2:1;
        while (nb_iterations)
        {
            iYmin = (int)round(rayon*ymin);
            iYmax = (int)round(rayon*ymax);

            for (iY=iYmin; iY<=iYmax; iY++)
            {
                double cos_a = (double) iY / rayon;
                // Rappel : cos² a + sin² a = 1
                double sin_a = sqrt(1 - cos_a * cos_a);
                iX = (int)round((sin_a * rayon));
                tab[width * (centreY - iY) + centreX - iX] = penColor;
            }
            if (zone1==FIRSTANDENDZONE)
            {
                ymin = y1;
                ymax = cos_PI4;
            }
            nb_iterations--;
        }
    }
    return EXIT_SUCCESS;
}

int Dessin::ArcCercleEdgeAndFill(int x, int y, int rayon, double angle1, double angle2, uint bgColor, uint penColor, uint penWidth)
{
    if (LigneAngle(x, y, angle1, rayon, penColor) == EXIT_FAILURE)
        return EXIT_FAILURE;
    if (LigneAngle(x, y, angle2, rayon, penColor) == EXIT_FAILURE)
        return EXIT_FAILURE;
    if (ArcCercle(x, y, rayon, angle1, angle2, penColor) == EXIT_FAILURE)
        return EXIT_FAILURE;
    return EXIT_SUCCESS;
}

int Dessin::ArcCercleEdgeAndFill(int x, int y, int rayon, double angle1, double angle2)
{
    return ArcCercleEdgeAndFill(x, y, rayon, angle1, angle2, currentBgColor, currentPenColor);
}

int Dessin::ArcCercleNonOptimise(int centreX, int centreY, int rayon, double angle1, double angle2, uint penColor)
{
    if (centreY < 1 || centreY < 1 || rayon < 1 || (centreY+rayon) > width || (centreY+rayon) > height)
        return EXIT_FAILURE;

    centreX--;
    centreY--;

    int X, Y;
    double angle = angle1;
    double stepAngle = 1.0 / rayon;
    while (angle <= angle2)
    {
        X = (int)round(rayon * cos(angle));
        Y = (int)round(rayon * sin(angle));
        tab[width * (centreY - Y) + centreX - X] = penColor;
        angle += stepAngle;
    }
    return EXIT_SUCCESS;
}

int Dessin::triangle(int x1, int y1, int x2, int y2, int x3, int y3, uint penColor, uint penWidth)
{
    int res1 = Ligne(x1, y1, x2, y2, penColor, penWidth);
    int res2 = Ligne(x1, y1, x3, y3, penColor, penWidth);
    int res3 = Ligne(x2, y2, x3, y3, penColor, penWidth);
    return res1 || res2 || res3;
}

// colorDelim : couleur de délimitation (pour indiquer où le remplissage s'arrête)
int Dessin::Fill(int x, int y, uint color, uint colorDelimiter)
{
    if (x < 1 || y < 1 || x > width || y > height)
    {
        printf("Erreur Fill(%d, %d, ...) : parametres incorrects\n", x, y);
        return EXIT_FAILURE;
    }
    colorDelim = colorDelimiter;
    Fill_(x, y, color);
    return EXIT_SUCCESS;
}

void Dessin::Fill_(int x, int y, uint color)
{
    int pos = width * (y - 1) + x - 1;
    if (tab[pos] == color || tab[pos] == colorDelim)
        return;
    tab[pos] = color;
    Fill_(x-1, y, color);
    Fill_(x+1, y, color);
    Fill_(x, y-1, color);
    Fill_(x, y+1, color);
}

void Dessin::FillOptimizedTmp(int x, int y, uint color)
{
	while (y < height)
	{
		int pos = width * (y - 1) + x - 1;
		int posSav = pos;
		while (tab[pos] != colorDelim)
			tab[pos--] = color;
		int posMin = pos;
		pos = posSav;
		while (tab[pos] != colorDelim)
			tab[pos++] = color;
		if (pos - posMin < 1)
			break;
		y++;
	}
}

int Dessin::insereBitmap(Dessin *image, int x, int y, int align, uint transparencyColor)
{
    int deltaX, deltaY;
    switch(align)
    {
        case TOPLEFT:      deltaX = 0;              deltaY = 0; break;
        case TOPMIDDLE:    deltaX = image->width/2; deltaY = 0; break;
        case TOPRIGHT:     deltaX = image->width;   deltaY = 0; break;
        case CENTERLEFT:   deltaX = 0;              deltaY = image->height/2; break;
        case CENTER:       deltaX = image->width/2; deltaY = image->height/2; break;
        case CENTERRIGHT:  deltaX = image->width;   deltaY = image->height/2; break;
        case BOTTOMLEFT:   deltaX = 0;              deltaY = image->height; break;
        case BOTTOMMIDDLE: deltaX = image->width/2; deltaY = image->height; break;
        case BOTTOMRIGHT:  deltaX = image->width;   deltaY = image->height; break;
        default: return EXIT_FAILURE; break;
    }
    int startX = x - deltaX;
    int startY = y - deltaY;
    int endX   = x - deltaX + image->width;
    int endY   = y - deltaY + image->height;
    if (startX < 1 || endX > width || startY < 1 || endY > height)
    {
        printf("Erreur insereBitmap : coordonnées incorrectes\n");
        return EXIT_FAILURE;
    }

    for (y = 0; y < image->height; y++)
    {
        for (x = 0; x < image->width; x++)
        {
            tab[ (startY-1 + y) * width + x ] = image->tab[ y * image->width + x ];
        }
    }
    return EXIT_SUCCESS;
}


int Dessin::enregistrer()
{
    FILE *f;
    if (!fileName || !flagSvg)
    {
        printf("ERREUR enregistrer : fileName non defini ou flagSvg à 0");
        return EXIT_FAILURE;
    }
    if (! (f = fopen(fileName, "wb")))
    {
        printf("ERREUR enregistrer : impossible d'ouvrir le fichier %s en ecriture !\n", fileName);
        return EXIT_FAILURE;
    }

    // Enregistrement au format BMP :
    // Chaque pixel en commençant par celui en bas à gauche défini par un groupe de 3 octets
    // representant respectivement ses teintes BLEUE VERT ROUGE.
    int x, y, iPix = 0;

    for (y = height-1; y >=0; y--)
    {
        for (x = 0; x < width; x++)
        {
            uint iTab = tab[width * y + x];
            // TODO : peut-Ãªtre devoir caster Ã  BYTE
            BMP[iPix++] = (iTab & 0x0000FF00) >> 8;
            BMP[iPix++] = (iTab & 0x00FF0000) >> 16;
            BMP[iPix++] = (iTab & 0xFF000000) >> 24;
        }
        // Chaque ligne doit comporter un nombre d'octets multiple de 4
        while (iPix%4)
        {
            BMP[iPix++] = 0;
        }
    }

    // on enregistre dans le fichier
    fwrite(&file_header, sizeof(BITMAPFILEHEADER), 1, f);
    fwrite(&info_header, sizeof(BITMAPINFOHEADER), 1, f);
    fwrite(BMP, info_header.biSizeImage, 1, f);
    fclose(f);

    flagSvg = 0; // Ã€ la fin
    printf("SUCCES enregister : sauvegarde image sous %s", fileName);
    return EXIT_SUCCESS;
}

/*
  ENTREE : fileName
           Nom du fichier. Les antislash '\' doivent etre doublés si présents.
           Exemple : C:\\tests\\image01.bmp au lieu de C:\tests\image01.bmp
*/
int Dessin::enregistrerSous(const char *fileName)
{
    if (!strcmp(this->fileName, fileName))
    {
        printf("ERREUR enregistrerSous : le fichier %s existe déjà !", fileName);
        return EXIT_FAILURE; // le fichier existe deja, on quitte
    }
    strcpy(this->fileName, fileName);
    flagSvg = 1;
    return enregistrer();
}

int Graph::TraceAxes(uint color, int reperes)
{
    if (xmin < 0 && xmax > 0)
    {
        int origineX = (int)(-xmin / (xmax - xmin) * width);
        LigneVer(origineX, 1, height, color);
    }
    if (ymin < 0 && ymax > 0)
    {
        int origineY = height - (int)(-ymin / (ymax - ymin) * height);
        LigneHor(1, width, origineY, color);
    }
    return EXIT_SUCCESS;
}

int Graph::TraceFunc(double (*ptrFonction)(double), uint color)
{
    double x, y;
    int ptX, ptY, ptYAnt, Y;
    for (ptX=0; ptX<width; ptX++)
    {
        x = xmin + (double)(ptX) / width * (xmax - xmin);
        y = (*ptrFonction)(x);
        ptY = height - int(round((y - ymin) / (ymax - ymin) * height));
        if (ptY >= height || ptY < 0)
        {
            ptYAnt = ptY;
            continue;
        }
        if (ABS(ptY - ptYAnt) < 2 || ptX == 0)
            tab[width * ptY + ptX] = color;
        else // on trace une ligne
        {
            if (ptYAnt < height && ptYAnt >= 0)
            {
                double delta = (double)1 / ABS(ptYAnt - ptY); // delta compris entre 0 et 1
                double x_ = (double)(ptX-1) + delta;
                if (ptYAnt > ptY)
                {
                    for (Y = ptYAnt-1; Y >= ptY; Y--)
                    {
                        tab[width * Y + (uint)round(x_)] = color;
                        x_ += delta;
                    }
                }
                else // ptYant < ptY)
                {
                    for (Y = ptYAnt+1; Y <= ptY; Y++)
                    {
                        tab[width * Y + (uint)round(x_)] = color;
                        x_ += delta;
                    }
                }
            }
        }
        ptYAnt = ptY;
    }
   return EXIT_SUCCESS;
}

int Dessin::triangle_SierpinskyPoints(int xa, int ya, int xb, int yb, int xc, int yc, int n)
{
    COORD pts[3];
    pts[0].x = xa;
    pts[0].y = ya;
    pts[1].x = xb;
    pts[1].y = yb;
    pts[2].x = xc;
    pts[2].y = yc;
    srand (time (NULL));

    // On initialise le 1er point
    int xi = (xa+xb)>>1;
    int yi = (ya+yc)>>1;

    for(int i=0; i<n; i++)
    {
        int randomValue = rand() % 3;
        xi = (pts[randomValue].x + xi) / 2;
        yi = (pts[randomValue].y + yi) / 2;
        Point(xi, yi);
    }
    return EXIT_SUCCESS;
}


int Dessin::triangle_Sierpinsky(int xa, int ya, int xb, int yb, int xc, int yc, int n, uint color, uint penWidth=1, uint m=0, uint *colors=NULL)
{
    //int xe = (xa+xb)%2==0? (xa+(xb>>1)) : (xa+(xb>>1))+1;
    int xe = (xa+xb)>>1;
    int ye = (ya+yb)>>1;
    int xf = (xb+xc)>>1;
    int yf = (yb+yc)>>1;
    int xg = (xa+xc)>>1;
    //int xg = (xa+xc)%2==0?(xa+(xc>>1)):(xa+(xc>>1))+1;
    int yg = (ya+yc)>>1;

    if (m==0)
    {
        if (triangle(xa, ya, xb, yb, xc, yc, color) == EXIT_FAILURE)
            return EXIT_FAILURE;
    }
    else if (m==1)
    {
        Ligne(xc, yc, xb, yb, color);
    }
    else if (m==2)
    {
        Ligne(xa, ya, xc, yc, color);
    }
    else if (m==3)
    {
        Ligne(xa, ya, xb, yb, color);
        if (colors != NULL)
		{
			Fill(xc, ya+1, colors[n], color);
		}
    }
    if (n>0)
    {
        triangle_Sierpinsky(xa, ya, xe, ye, xg, yg, n-1, color, 1, 1, colors);
        triangle_Sierpinsky(xe, ye, xb, yb, xf, yf, n-1, color, 1, 2, colors);
        triangle_Sierpinsky(xg, yg, xf, yf, xc, yc, n-1, color, 1, 3, colors);
    }
    return EXIT_SUCCESS;
}


int Dessin::printSegmentHor(int x1, int x2, int y1, int y2, int y3, uint color)
{
    int y;
    for (y=y1; y<y2; y++)
    {
        LigneHor(x1, x2, y, color);
        x1--; x2++;
    }
    if (y2-y1 > y3-y2)
    {
        x1++; x2--;
    }
    else if (y2-y1 < y3-y2)
    {
        x1--; x2++;
    }
    for (y=y2; y<y3; y++)
    {
        x1++; x2--;
        LigneHor(x1, x2, y, color);
    }
    return EXIT_SUCCESS;
}

int Dessin::printSegmentVer(int x1, int x2, int x3, int y1, int y2, uint color)
{
    int x;
    for (x=x1; x<x2; x++)
    {
        LigneVer(x, y1, y2, color);
        y1--; y2++;
    }
    if (x2-x1 > x3-x2)
    {
        y1++; y2--;
    }
    else if (x2-x1 < x3-x2)
    {
        y1--; y2++;
    }
    for (x=x2; x<x3; x++)
    {
        y1++; y2--;
        LigneVer(x, y1, y2, color);
    }
    return EXIT_SUCCESS;
}

int Dessin::printChiffre7DIGIT(char c, int x, int y, int fontSize, uint color)
{
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

    // no bit 8 7 6 5 4 3 2 1
    //        . g f e d c b a

    static int vals[10] = {63, 6, 91, 79, 102, 109, 125, 7, 127, 111};
    if (fontSize < 9)
        return EXIT_FAILURE;
    if  (c != '.' && (c > '9' || c < '0'))
        return EXIT_FAILURE;

    // Taille = hauteur du caractÃ¨re en nombre de pixels
    // Taille mini = 9
    double unite = fontSize / 18.0;
    int unite1  = (int)round(unite);
    int unite2  = (int)round(unite*2);
    int unite8  = (int)round(unite*8);
    int unite9  = (int)round(unite*9);
    int unite10 = (int)round(unite*10);
    int unite16 = (int)round(unite*16);
    int unite17 = (int)round(unite*17);

    if (c != '.')
    {
        int val = vals[c - '0'];

        if (val & 1) // segment a
        {
            printSegmentHor(unite2 + x-1, unite8 + x, y, unite1 + y, unite2 + y, color);
        }
        if (val & 2) // segment b
        {
            printSegmentVer(unite8 + x, unite9 + x, unite10+x, unite2+y-1, unite8 + y, color);
        }
        if (val & 4) // segment c
        {
            printSegmentVer(unite8+x, unite9+x, unite10+x, unite10+y-1, unite16+y, color);
        }
        if (val & 8) //segment d
        {
            printSegmentHor(unite2 + x-1, unite8 + x, unite16+ y, unite17+ y, fontSize+ y, color);
        }
        if (val & 16) // segment e
        {
            printSegmentVer(x, unite1 + x, unite2 + x, unite10+ y-1, unite16+ y, color);
        }
        if (val & 32) // segment f
        {
            printSegmentVer(x, unite1 + x, unite2 + x, unite2 + y-1, unite8 + y, color);
        }
        if (val & 64) // segment g
        {
            printSegmentHor(unite2 + x-1, unite8 + x, unite8 + y, unite9 + y, unite10+ y, color);
        }
    }
    else
    {
        // on dessine le point
        RectangleFill(x, y + fontSize-unite2, x+unite2, y+fontSize, color);
    }
    return EXIT_SUCCESS;
}

int Dessin::PrintNombre(char *nombre, int x, int y, uint fontSize, int align, uint color)
{
    char c;
    int deltaX, deltaY;
    if (fontSize < 9)
    {
        printf("ERREUR [PrintNombre] : fontSize = %d (doit etre superieur a 8)\n", fontSize);
        return EXIT_FAILURE;
    }
    int charWidth = int(round(fontSize * 5.0 / 9.0)) + (fontSize >> 1);
    int strWidth  = charWidth * strlen(nombre);
    switch(align)
    {
        case TOPLEFT:      deltaX = 0;          deltaY = 0; break;
        case TOPMIDDLE:    deltaX = strWidth/2; deltaY = 0; break;
        case TOPRIGHT:     deltaX = strWidth;   deltaY = 0; break;
        case CENTERLEFT:   deltaX = 0;          deltaY = fontSize/2; break;
        case CENTER:       deltaX = strWidth/2; deltaY = fontSize/2; break;
        case CENTERRIGHT:  deltaX = strWidth;   deltaY = fontSize/2; break;
        case BOTTOMLEFT:   deltaX = 0;          deltaY = fontSize; break;
        case BOTTOMMIDDLE: deltaX = strWidth/2; deltaY = fontSize; break;
        case BOTTOMRIGHT:  deltaX = strWidth;   deltaY = fontSize; break;
        default: return EXIT_FAILURE; break;
    }
    int startX = x - deltaX;
    int startY = y - deltaY;
    int endX   = x - deltaX + strWidth;
    int endY   = y - deltaY + fontSize;
    if (startX < 1 || endX > width || startY < 1 || endY > height)
    {
        printf("ERREUR [PrintNombre] : la chaine %s de taille %d ne rentre pas dans l'image\n", nombre, fontSize);
        return EXIT_FAILURE;
    }
    x = startX;
    y = startY;
    while ((c = *nombre) != '\0')
    {
        printChiffre7DIGIT(c, x, y, fontSize, color);
        if (c != '.')
            x += charWidth;
        else
            x += charWidth/2;
        nombre++;
    }
    return EXIT_SUCCESS;
}

int Dessin::PrintNombre(int nombre, int x, int y, uint fontSize, int align, uint color)
{
    if (fontSize < 9)
    {
        printf("ERREUR [PrintNombre] : fontSize = %d (doit etre superieur a 8)\n", fontSize);
        return EXIT_FAILURE;
    }
    char str[11] = "";
    sprintf(str, "%d", nombre);
    PrintNombre(str, x, y, fontSize, align, color);
    return EXIT_SUCCESS;
}

int Dessin::PrintNombre(double nombre, int x, int y, uint fontSize, int align, uint color)
{
    if (fontSize < 9)
    {
        printf("ERREUR [PrintNombre] : fontSize = %d (doit etre superieur a 8)\n", fontSize);
        return EXIT_FAILURE;
    }
    char str[20] = "";
    sprintf(str, "%f", nombre);
    PrintNombre(str, x, y, fontSize, align, color);
    return EXIT_SUCCESS;
}

int Dessin::ci_remove_isolated_points()
{
    int ptX, ptY;
    int nbPts1_removed = 0;
    int nbPts2_removed = 0;
    int nbPts3_removed = 0;

    // On vérifie que l'image ne comporte que des pixels Blancs ou Noirs
    for (ptY = 0; ptY < height; ptY++)
    {
        for (ptX = 0; ptX < width; ptX++)
        {
            // Si le pixel n'est ni Blanc, ni Noir, on quitte
            if (tab[width * ptY + ptX] != colorWhite && tab[width * ptY + ptX] != colorBlack)
            {
                printf("ERREUR : L'image \"%s\" comporte au moins 1 pixel qui n'est ni Blanc, ni Noir\n", fileName);
                return EXIT_FAILURE;
            }
        }
    }

    // A Ameliorer (enlever le -3 mais ajouter tests limites max)
    for (ptY = 1; ptY < height-4; ptY++)
    {
        for (ptX = 1; ptX < width-4; ptX++)
        {
            int step_1 = width * (ptY-1);
            int step_2 = width * (ptY  );
            int step_3 = width * (ptY+1);
            int step_4 = width * (ptY+2);
            int step_5 = width * (ptY+3);

            // On supprime les points isolés
            if (tab[step_2 + ptX] == colorBlack)
            {
                if (tab[step_1 + ptX-1] == colorWhite &&
                    tab[step_1 + ptX  ] == colorWhite &&
                    tab[step_1 + ptX+1] == colorWhite &&
                    tab[step_2 + ptX-1] == colorWhite &&
                    tab[step_3 + ptX-1] == colorWhite)
                {
                    if (tab[step_2 + ptX+1] == colorWhite &&
                        tab[step_3 + ptX  ] == colorWhite &&
                        tab[step_3 + ptX+1] == colorWhite)
                    {
                        nbPts1_removed++;
                        tab[step_2 + ptX] = colorWhite;
                    }
                    else if (tab[step_1 + ptX+2] == colorWhite &&
                        tab[step_4 + ptX-1] == colorWhite)
                    {
                        if (tab[step_2 + ptX+2] == colorWhite &&
                            tab[step_3 + ptX+2] == colorWhite &&
                            tab[step_4 + ptX  ] == colorWhite &&
                            tab[step_4 + ptX+1] == colorWhite &&
                            tab[step_4 + ptX+2] == colorWhite)
                        {
                            nbPts2_removed++;
                            tab[step_2 + ptX  ] = colorWhite;
                            tab[step_2 + ptX+1] = colorWhite;
                            tab[step_3 + ptX  ] = colorWhite;
                            tab[step_3 + ptX+1] = colorWhite;
                        }

                        else if (tab[step_1 + ptX+3] == colorWhite &&
                            tab[step_5 + ptX-1] == colorWhite &&
                            tab[step_2 + ptX+3] == colorWhite &&
                            tab[step_3 + ptX+3] == colorWhite &&
                            tab[step_4 + ptX+3] == colorWhite &&
                            tab[step_5 + ptX  ] == colorWhite &&
                            tab[step_5 + ptX+1] == colorWhite &&
                            tab[step_5 + ptX+2] == colorWhite &&
                            tab[step_5 + ptX+3] == colorWhite)
                        {
                            nbPts3_removed++;
                            tab[step_2 + ptX  ] = colorWhite;
                            tab[step_2 + ptX+1] = colorWhite;
                            tab[step_2 + ptX+2] = colorWhite;
                            tab[step_3 + ptX  ] = colorWhite;
                            tab[step_3 + ptX+1] = colorWhite;
                            tab[step_3 + ptX+2] = colorWhite;
                            tab[step_4 + ptX  ] = colorWhite;
                            tab[step_4 + ptX+1] = colorWhite;
                            tab[step_4 + ptX+2] = colorWhite;
                        }
                    }
                }
            }
        }
    }
    printf("Fonction ci_remove_isolated_points :\n");
    printf("    %d pixels noirs supprimes\n", nbPts1_removed);
    printf("    %d blocs 2x2 supprimes\n", nbPts2_removed);
    printf("    %d blocs 3x3 supprimes\n", nbPts3_removed);
    return EXIT_SUCCESS;
}

/*
 Dessin::ci_draw_pellets
 Remplace un pixel de couleur rouge (0xFF0000) par une pastille 0.6mm Dint et 2.2mm  Dext       (image 600DPI)
 Remplace un pixel de couleur rouge fonce (0x800000) par une pastille 0.6mm Dint et 2.1mm  Dext       (image 600DPI)
 Remplace un pixel de couleur vert  (0x00FF00) par une pastille 0.8mm Dint et 2.54mm Dext      (image 600DPI)
 Remplace un pixel de couleur bleu  (0x0000FF) par une pastille 1.0mm Dint et x.xxmm Dext      (image 600DPI)   TODO
 Remplace un pixel de couleur gris  (0x808080) par une pastille 1.6mm Dint et 3.0 mm Dext      (image 600DPI)


*/
int Dessin::ci_draw_pellets()
{
    int ptX, ptY;
    int nb = 0;
    int nbPellet06_drawn = 0;
    int nbPellet06bis_drawn = 0;
    int nbPellet08_drawn = 0;
    int nbPellet16_drawn = 0;

    for (ptY = 0; ptY < height; ptY++)
    {
        for (ptX = 0; ptX < width; ptX++)
        {
            if (tab[width * ptY + ptX] == colorRed) // pastille 0.6 mm
            {
                if (CercleFill(ptX+1, ptY+1, 26, colorOrangeRed) == EXIT_SUCCESS)
                    if (CercleFill(ptX+1, ptY+1, 7, colorWhite) == EXIT_SUCCESS)
                        nbPellet06_drawn++;
            }
            else if (tab[width * ptY + ptX] == colorMaroon) // pastille 0.6mm Dext un peu plus petit
            {
                if (CercleFill(ptX+1, ptY+1, 25, colorBrown) == EXIT_SUCCESS)
                    if (CercleFill(ptX+1, ptY+1, 7, colorWhite) == EXIT_SUCCESS)
                        nbPellet06bis_drawn++;
            }
            else if (tab[width * ptY + ptX] == colorGREEN) // pastille 0.8mm
            {
                if (CercleFill(ptX+1, ptY+1, 30, colorSeaGreen) == EXIT_SUCCESS)
                    if (CercleFill(ptX+1, ptY+1, 9, colorWhite) == EXIT_SUCCESS)
                        nbPellet08_drawn++;
            }
            else if (tab[width * ptY + ptX] == colorGray) // pastille 1.6mm
            {
                if (CercleFill(ptX+1, ptY+1, 35, colorDimGray) == EXIT_SUCCESS)
                    if (CercleFill(ptX+1, ptY+1, 20, colorWhite) == EXIT_SUCCESS)
                        nbPellet16_drawn++;
            }
        }
    }
    printf("Fonction ci_draw_pellet :\n");
    printf("    %d pastilles 0.6mm dessinées\n", nbPellet06_drawn);
    printf("    %d petites pastilles 0.6mm dessinées\n", nbPellet06bis_drawn);
    printf("    %d pastilles 0.8mm dessinées\n", nbPellet08_drawn);
    printf("    %d pastilles 1.6mm dessinées\n", nbPellet16_drawn);
    return EXIT_SUCCESS;
}

int Dessin::replaceColors(uint oldColor, uint newColor)
{
    uint index;
    for(index=0; index<size; index++)
    {
        if (tab[index] == oldColor)
            tab[index] = newColor;
    }
    return EXIT_SUCCESS;
}

int Dessin::replaceColors(uint oldRed, uint oldGreen, uint oldBlue, uint newRed, uint newGreen, uint newBlue)
{
    if (oldRed > 255 || oldGreen > 255 || oldBlue > 255 || newRed > 255 || newGreen > 255 || newBlue > 255)
    {
        printf("replaceColors : couleur hors zone (valeur max=255)\n");
        return EXIT_FAILURE;
    }
    uint oldColor = (oldRed << 24) + (oldGreen << 16) + (oldBlue << 8);
    uint newColor = (newRed << 24) + (newGreen << 16) + (newBlue << 8);
    return replaceColors(oldColor, newColor);
}

/**
Tous les pixels dont la couleurs est proche de la couleur cible, prennent la couleur cible
tolerance indique le seuil de proximité couleur.
**/
int Dessin::replaceColorsTolerance(uint targetRed, uint targetGreen, uint targetBlue, uint tolerance)
{
    uint index, curRed, curGreen, curBlue;
    uint targetColor = (targetRed << 24) + (targetGreen << 16) + (targetBlue << 8);
    if (targetRed > 255 || targetGreen > 255 || targetBlue > 255)
    {
        printf("replaceColorsTolerance : couleur doit etre <= 255\n");
        return EXIT_FAILURE;
    }
    if (tolerance==0 || tolerance>=255)
    {
        printf("replaceColorsTolerance : tolerance doit etre > 0 et <255\n");
        return EXIT_FAILURE;
    }
    for(index=0; index<size; index++)
    {
        uint curColor = tab[index];
        if (curColor == targetColor)
            continue;
        curRed   =  curColor >> 24;
        curGreen = (curColor & 0x00FF0000) >> 16;
        curBlue  = (curColor & 0x0000FF00) >> 8;
        if (abs(targetRed-curRed)     <= tolerance &&
            abs(targetGreen-curGreen) <= tolerance &&
            abs(targetBlue-curBlue)   <= tolerance)
        {
            tab[index] = targetColor;
        }
    }
    return EXIT_SUCCESS;
}

/**
Augmente la luminosité de l'image
**/
int Dessin::brightness(uint value)
{
    uint index, curColor, newColor, newRed, newGreen, newBlue;
    if (value == 0 || value > 255)
    {
        printf("brightness : value doit etre > 0 et <= 255\n");
        return EXIT_FAILURE;
    }
    for(index=0; index<size; index++)
    {
        curColor = tab[index];
        newRed   =  (curColor >> 24) + value;
        newGreen = ((curColor & 0x00FF0000) >> 16) + value;
        newBlue  = ((curColor & 0x0000FF00) >> 8)  + value;
        if (newRed > 255)   newRed   = 255;
        if (newGreen > 255) newGreen = 255;
        if (newBlue > 255)  newBlue  = 255;
        newColor = (newRed << 24) + (newGreen << 16) + (newBlue << 8);
        tab[index] = newColor;
    }
    return EXIT_SUCCESS;
}

/**
Augmente la luminosité des zones sombres de l'image uniquement.
seuilHaut indique le niveau max de luminosité auquel appliquer le traitement
Fonction expérimentale ...
**/
int Dessin::brightness(uint value, uint seuilHaut)
{
    uint index, curColor, curRed, curGreen, curBlue;
    if (value == 0 || value > 255)
    {
        printf("brightness : value doit etre > 0 et <= 255\n");
        return EXIT_FAILURE;
    }
    if (seuilHaut == 0 || seuilHaut >= 255)
    {
        printf("brightness : seuilHaut doit etre > 0 et < 255\n");
        return EXIT_FAILURE;
    }
    for(index=0; index<size; index++)
    {
        curColor = tab[index];
        curRed   =  (curColor >> 24);
        curGreen = ((curColor & 0x00FF0000) >> 16);
        curBlue  = ((curColor & 0x0000FF00) >> 8);
        if ((curRed + curGreen + curBlue) < 3*seuilHaut && curRed > curGreen && curRed > curBlue)
        {
            curRed += value;
            if (curRed > 255) curRed   = 255;
            curGreen += value;
            if (curGreen > 255) curGreen = 255;
            curBlue += value;
            if (curBlue > 255)  curBlue  = 255;
        }
        tab[index] = (curRed << 24) + (curGreen << 16) + (curBlue << 8);
    }
    return EXIT_SUCCESS;
}


/**
Augmente la luminosité de manière variable : plus la zone est sombre, plus
l'augmentation de la luminosité sera grande.
**/
int Dessin::brightnessVariable(uint value)
{
    uint index, curColor, curRed, curGreen, curBlue;
    if (value == 0 || value > 255)
    {
        printf("brightnessVariable : value doit etre > 0 et <= 255\n");
        return EXIT_FAILURE;
    }
    for(index=0; index<size; index++)
    {
        curColor = tab[index];
        curRed   =  (curColor >> 24);
        curGreen = ((curColor & 0x00FF0000) >> 16);
        curBlue  = ((curColor & 0x0000FF00) >> 8);
        uint value2 = (255 - (curRed + curGreen + curBlue) / 3) / (255 / value);

        curRed += value2;
        if (curRed > 255) curRed   = 255;
        curGreen += value2;
        if (curGreen > 255) curGreen = 255;
        curBlue += value2;
        if (curBlue > 255)  curBlue  = 255;
        tab[index] = (curRed << 24) + (curGreen << 16) + (curBlue << 8);
    }
    return EXIT_SUCCESS;
}


