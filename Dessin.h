#include <stdio.h>
#include "colors.h"

#pragma pack(2)

typedef unsigned short WORD;
typedef unsigned long DWORD;
typedef long LONG;
typedef unsigned char BYTE;
typedef unsigned int uint;

#ifndef _WINGDI_H
typedef struct tagBITMAPFILEHEADER {
  WORD	bfType;      // Signature du fichier. BM=Bitmap windows, BA= Bitmap OS2,CI=Icône couleur OS2, CP=pointeur de couleur OS2, IC= icône OS2, PT=pointeur OS2
  DWORD bfSize;      // Taille totale du fichier en octets
  WORD	bfReserved1; // 0x0
  WORD	bfReserved2; // 0x0
  DWORD bfOffBits;   // Adresse de la zone de définition de l’image, équivalente à la taille de l'entête précédent les données (2 en-têtes (BITMAPFILEHEADER et BITMAPINFOHEADER), puis une palette (tableau de RGBQUAD) si elle est utilisée)
} BITMAPFILEHEADER, *PBITMAPFILEHEADER;

//Bitmap information header
//provides information specific to the image data
typedef struct tagBITMAPINFOHEADER{
  DWORD  biSize;          // Taille en octets de cet en-tête BITMAPINFOHEADER (0x28)
  LONG   biWidth;         // Largeur de l’image en pixels
  LONG   biHeight;        // Hauteur de l’image en pixels
  WORD   biPlanes;        // Nombre de plans
  WORD   biBitCount;      // Nombre de bits par pixel
  DWORD  biCompression;   // Type de compression : 0=pas de compression, 1=compressé à 8 bits par pixel, 2=4bits par pixel.
  DWORD  biSizeImage;     // Taille en octets des données de l’image
  LONG   biXPelsPerMeter; // Résolution horizontale en pixels par mètre
  LONG   biYPelsPerMeter; // Résolution verticale en pixels par mètre
  DWORD  biClrUsed;       // Nombre de couleurs dans l’image : 0=maximum possible. Si une palette est utilisée, ce nombre indique le nombre de couleurs de la palette
  DWORD  biClrImportant;  // Nombre de couleurs importantes. 0= toutes importantes
} BITMAPINFOHEADER, *PBITMAPINFOHEADER;

//Colour palette
typedef struct tagRGBQUAD {
  BYTE	rgbBlue;
  BYTE	rgbGreen;
  BYTE	rgbRed;
  BYTE	rgbReserved;
} RGBQUAD;
#endif

#define PI M_PI
#define MIN(X, Y) (((X) < (Y)) ? (X) : (Y))
#define MAX(X, Y) (((X) > (Y)) ? (X) : (Y))
#define ABS(X) ((X) < 0 ? -(X) : (X))

#define TOPLEFT      0
#define TOPMIDDLE    1
#define TOPRIGHT     2
#define CENTERLEFT   3
#define CENTER       4
#define CENTERRIGHT  5
#define BOTTOMLEFT   6
#define BOTTOMMIDDLE 7
#define BOTTOMRIGHT  8

typedef struct _COORD {
	int x;
	int y;
} COORD, *PCOORD;

class Dessin
{
protected:
	int  width;
	int  height;
	uint size;
	//uint  bitsPerPixel=24;
	char fileName[256];
	FILE *file;
	uint currentBgColor;
	uint currentPenColor;
	uint currentPenWidth;
	uint colorDelim; // Utilisé pour le Fill
	int  currentX;
	int  currentY;


	uint currentFontSize;
	//uint currentFontSpace;
	BYTE *BMP;
	uint *tab;
	uint flagSvg; // indique si des modifications ont ete faites depuis le dernier enregistrement
	BITMAPFILEHEADER file_header;
	BITMAPINFOHEADER info_header;
	int ok;
	void Fill_(int x, int y, uint colorBg);
	void FillOptimizedTmp(int x, int y, uint color);
	int printSegmentHor(int x1, int x2, int y1, int y2, int y3, uint color);
	int printSegmentVer(int x1, int x2, int x3, int y1, int y2, uint color);
	int printChiffre7DIGIT(char c, int x, int y, int fontSize, uint color);
public:
    // Constructeur & Destructeur
	Dessin(uint width, uint height, uint bgColor = colorWhite, uint penColor = colorBlack, uint bitsPerPixel=24);
	Dessin(const char *filename);
	~Dessin();

	// Accesseurs
	int getCurX();
	int getCurY();
	void setCurX(int x);
	void setCurY(int y);
	void setCur(int x, int y);
	void setCurrentBgColor (uint color);
	void setCurrentPenColor(uint color);
	void setCurrentPenWidth(uint color);
	void setCurrentFontSize(uint fontSize);
	void setColorDelim(uint color);
	//void setCurrentFontSpace(uint fontSpace);

	int enregistrer();
	int enregistrerSous(const char *filename);
	int ouvrir(const char *filename);
	int insereBitmap(Dessin *image, int x, int y, int align, uint transparencyColor = 0xFFFFFFFF);
	int isOk();

	void erase(uint color);
	int MoveAngle     (double angle, int len);
	int Ligne         (int x1, int y1, int x2, int y2, uint penColor, uint penWidth=1);
	int Ligne         (int x1, int y1, int x2, int y2);
	int LigneAngle    (int x,  int y,  double angle, int longueur, uint penColor);
	int LigneAngle    (int x,  int y,  double angle, int longueur);
	int LigneHor      (int x1, int x2, int y, uint penColor, uint penWidth=1);
	int LigneHor      (int x1, int x2, int y);
	int LigneVer      (int x,  int y1, int y2, uint penColor, uint penWidth=1);
	int LigneVer      (int x,  int y1, int y2);
	int Rectangle     (int x1, int y1, int x2, int y2, uint penColor, uint penWidth=1);
	int Rectangle     (int x1, int y1, int x2, int y2);
	int RectangleFill (int x1, int y1, int x2, int y2, uint bgColor);
	int RectangleFill (int x1, int y1, int x2, int y2);
	int RectangleFill (int x1, int y1, int x2, int y2, uint bgColor, uint penColor, uint penWidth=1);
	int RectangleEdgeAndFill(int x1, int y1, int x2, int y2);
	int RectangleEdgeAndFill(int x1, int y1, int x2, int y2, uint bgColor, uint penColor, uint penWidth=1);
	int Polygon       (int centreX, int centreY, int rayon, int nbFaces, uint color, int initPosition = 0, double rotationAngle = 0.0);
	//int Polygon       (int centreX, int centreY, int rayon, int nbFaces, int initPosition = 0, double rotationAngle = 0.0);
	int Cercle        (int centreX, int centreY, int rayon, uint color);
	int Cercle        (int centreX, int centreY, int rayon);
	int CercleFill    (int centreX, int centreY, int rayon, uint color);
	int CercleFill    (int centreX, int centreY, int rayon);
	int CercleEdgeAndFill(int centreX, int centreY, int rayon);
	int CercleEdgeAndFill(int centreX, int centreY, int rayon, uint bgColor, uint penColor);
	int ArcCercle     (int x, int y, int rayon, double angle1, double angle2);
	int ArcCercle     (int x, int y, int rayon, double angle1, double angle2, uint penColor);
	int ArcCercleEdgeAndFill(int x, int y, int rayon, double angle1, double angle2);
	int ArcCercleEdgeAndFill(int x, int y, int rayon, double angle1, double angle2, uint bgColor, uint penColor, uint penWidth=1);
    int ArcCercleNonOptimise (int x, int y, int rayon, double angle1, double angle2, uint penColor);
	int Fill          (int x, int y, uint colorBg, uint colorDelimiter);
	int Ellipse       (int x, int y, int r1, int r2, uint color);
	int Ellipse       (int x, int y, int r1, int r2);
	int Point         (int x, int y, uint penColor);
	int Point         (int x, int y);
	int PrintNombre   (int nombre,   int x, int y, uint fontSize, int align, uint color);
	int PrintNombre   (double nombre, int x, int y, uint fontSize, int align, uint color);
	int PrintNombre   (char *nombre, int x, int y, uint fontSize, int align, uint color);

	int triangle      (int x1, int y1, int x2, int y2, int x3, int y3, uint color, uint penWidth=1);
	int triangleFill  (int x1, int y1, int x2, int y2, int x3, int y3, uint bgColor, uint penWidth=1);
	int triangleEdgeAndFill (int x1, int y1, int x2, int y2, int x3, int y3, uint bgColor, uint penColor, uint penWidth=1);

	//2019-02-14 : ajout fonctions dessin triangle de Sierpinsky (fractale)
	int triangle_SierpinskyPoints(int xa, int ya, int xb, int yb, int xc, int yc, int n);
	int triangle_Sierpinsky(int xa, int ya, int xb, int yb, int xc, int yc, int n, uint color, uint penWidth, uint m, uint *colors);

	//2020-12-23 : ajout fonctions traitement typons de circuits imprimés
	int ci_remove_isolated_points();
	int ci_draw_pellets();

	//2023-03-02 : ajout fonctions de retouche d'image
	int replaceColors(uint oldColor, uint newColor);
	int replaceColors(uint oldRed, uint oldGreen, uint oldBlue, uint newRed, uint newGreen, uint newBlue);
    int replaceColorsTolerance(uint targetRed, uint targetGreen, uint targetBlue, uint tolerance);
    int brightness(uint seuil);
    int brightness(uint value, uint seuilHaut);
    int brightnessVariable(uint value);
};

class Graph : public Dessin
{
private:
	double xmin, xmax, xscl, ymin, ymax, yscl;
public:
	Graph(uint width, uint height, uint bgColor = colorWhite, uint penColor = colorBlack, uint bitsPerPixel=24);
	Graph(char *filename);
	~Graph();
	void setTraceRange(double xmin, double xmax, double ymin, double ymax);
	int TraceAxes     (uint color, int reperes = 0);
	int TraceFunc     (double (*ptrFonction)(double), uint color);
};
