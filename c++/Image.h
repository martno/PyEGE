#ifndef IMAGE_H
#define IMAGE_H

#include <GL/GLee.h>	// Must be included before GLFW
#include <GL/glfw.h>	// For GLFW, OpenGL and GLU

#include <IL/il.h>		// DevIL
#include <IL/ilu.h>		// DevIL
#include <IL/ilut.h>	// DevIL


class WindowHandler;
class Font;

class Image {
public:
	Image(int width, int height, unsigned char *color);
	Image(const char *filename);
	//Image(const char *fileName, int x, int y, int width, int height);
	Image(char* text, Font font);
	Image(char* text, int size);	// this constructor should not be used in future versions

//	Image(char *text, Font *font);
//	Image(FT_Bitmap *bitmap, int width, int height, bool flag);
	//Image(int *texels);
	~Image();

	void draw(double x, double y, double x_0=0, double y_0=0, double width=-1, double height=-1, bool bilinear=true, 
				 double scaleX = 1.0, double scaleY = 1.0, double rotate = 0.0,
				 double rotatePtX = 0.0, double rotatePtY = 0.0, unsigned char *color = NULL); //, bool invertColors, double *colorMatrix) {
	//void applyKeyColor(int red, int green, int blue, int alpha=0);

	int getWidth();
	int getHeight();
	//void saveImage(char *file, int type);

	void clear(unsigned char *color);
	void drawPoints(double *coords, int dimension, unsigned char *color, double size, bool antiAliasing);
	void drawLines(double *coords, int dimension, unsigned char *color, double width, bool loop,
				bool stipple, int stippleFactor, int stipplePattern, bool antiAliasing);
	void drawTriangles(double *coords, int dimension, unsigned char *color, int form);
	void drawPolygon(double *coords, int dimension, unsigned char *color);
	void drawSubImg(Image *subImg, double x, double y, double x_0, double y_0, double width, double height, bool bilinear, double scaleX = 1.0, double scaleY = 1.0,
					   double rotate = 0.0, double rotatePtX = 0.0, double rotatePtY = 0.0, unsigned char *color = NULL);
	//void drawSubImg(Image *subImg, double x, double y, bool bilinear, double scaleX, double scaleY,
//		  double rotate, double rotatePtX, double rotatePtY, unsigned char *color);


	static void setWindowHandlerPointer(WindowHandler *newWindowHandler);
	static void setDefaultFont(Font* newDefaultFont);

private:
	/* Methods: */
	int getNextPowerOf2(int no);
	void loadImage(const char *fileName, GLuint* outImageWidth, GLuint* outImageHeight,
						 GLuint* outRealWidth, GLuint* outRealHeight);
	//GLuint* Image::loadImage(const char *FileName, int x, int y, int width, int height);


	/* Variables */
	GLuint texName;

	GLuint imageWidth;
	GLuint imageHeight;
	GLuint realWidth;
	GLuint realHeight;
	double widthRatio;
	double heightRatio;
	bool bilinear;

	ILuint imageName;

	//WindowHandler *windowHandler;
	/* Static variables */
	static WindowHandler* windowHandler;
	static Font* defaultFont;
};


#endif IMAGE_H

