#include <GL/GLee.h>	// Must be included before GLFW
#include <GL/glfw.h>	// For GLFW, OpenGL and GLU

#include <IL/il.h>		// DevIL
#include <IL/ilu.h>		// DevIL
#include <IL/ilut.h>	// DevIL

class WindowHandler;

class Image {
public:
	Image(int width, int height, unsigned char *color);
	Image(const char *fileName);
	Image(const char *fileName, int x, int y, int width, int height);
//	Image(char *text, Font *font);
//	Image(FT_Bitmap *bitmap, int width, int height, bool flag);
	//Image(int *texels);
	~Image();

	void draw(double x, double y, bool bilinear, double scaleX, double scaleY, double rotate, 
				 double rotatePtX, double rotatePtY, unsigned char *color);
	int getWidth();
	int getHeight();
	//void saveImage(char *file, int type);

	void clear(unsigned char *color);
	void drawPoints(double *coords, int dimension, unsigned char *color, double size, bool antiAliasing);
	void drawLines(double *coords, int dimension, unsigned char *color, double width, bool loop,
				bool stipple, int stippleFactor, int stipplePattern, bool antiAliasing);
	void drawTriangles(double *coords, int dimension, unsigned char *color, int form);
	void drawPolygon(double *coords, int dimension, unsigned char *color);
	void drawSubImg(Image *subImg, double x, double y, bool bilinear, double scaleX, double scaleY,
		  double rotate, double rotatePtX, double rotatePtY, unsigned char *color);
	void setRotatePt(double x, double y);


	static void setWindowHandlerPointer(WindowHandler *newWindowHandler);


private:
	/* Methods: */
	int getNextPowerOf2(int nr);
	GLuint* Image::loadImage(const char *FileName);
	GLuint* Image::loadImage(const char *FileName, int x, int y, int width, int height);


	/* Variables */
	GLuint texName;

	GLuint imageWidth;
	GLuint imageHeight;
	double widthRatio;
	double heightRatio;
	bool bilinear;

	ILuint imageName;

	//WindowHandler *windowHandler;
	/* Static variables */
	static WindowHandler *windowHandler;
};

