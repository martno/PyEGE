#include "Image.h"
#include "WindowHandler.h"

#include <GL/GLee.h>	// Must be included before GLFW
#include <GL/glfw.h>	// For GLFW, OpenGL and GLU

#include <IL/il.h>		// DevIL
#include <IL/ilu.h>		// DevIL
#include <IL/ilut.h>	// DevIL

#include <stdlib.h>    // For malloc() etc.
#include <stdio.h>     // For printf(), fopen() etc.
#include <math.h>      // For sin(), cos() etc.


const int LARGEST_SIZE_OF_IMAGE_PER_DIMENSION = 2048;


Image::Image(int width, int height, unsigned char *color) {
	this->windowHandler = windowHandler;
	imageWidth = width;
	imageHeight = height;
	bilinear = false;

	int red = color[0];
	int green = color[1];
	int blue = color[2];
	int alpha = color[3];

	int newWidth = getNextPowerOf2(imageWidth);
	int newHeight = getNextPowerOf2(imageHeight);
	widthRatio = (double)imageWidth/newWidth;
	heightRatio = (double)imageHeight/newHeight;

	glGenTextures(1, &texName);
	glBindTexture(GL_TEXTURE_2D, texName);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, newWidth, newHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glBindTexture(GL_TEXTURE_2D, 0);
	windowHandler->bindFBO(texName, imageWidth, imageHeight);
		glClearColor(red, green, blue, alpha);
		glClear(GL_COLOR_BUFFER_BIT);
	windowHandler->releaseFBO();
}

Image::Image(const char *fileName) {
	this->windowHandler = windowHandler;
	GLuint *imageInfo = loadImage(fileName);

	texName = imageInfo[0];
	imageWidth = imageInfo[1];
	imageHeight = imageInfo[2];
	widthRatio = (double)imageInfo[1]/imageInfo[3];
	heightRatio = (double)imageInfo[2]/imageInfo[4];

	bilinear = false;
}

Image::Image(const char *fileName, int x, int y, int width, int height) {
	this->windowHandler = windowHandler;
	GLuint *imageInfo = loadImage(fileName, x, y, width, height);

	texName = imageInfo[0];
	imageWidth = imageInfo[1];
	imageHeight = imageInfo[2];
	widthRatio = (double)imageInfo[1]/imageInfo[3];
	heightRatio = (double)imageInfo[2]/imageInfo[4];

	bilinear = false;
}

Image::~Image() {
	glDeleteTextures(1, &texName);
}



int Image::getWidth() {
	return imageWidth;
}

int Image::getHeight() {
	return imageHeight;
}

void Image::draw(double x, double y, bool bilinear, double scaleX = 1.0, double scaleY = 1.0, double rotate = 0.0, 
				 double rotatePtX = 0.0, double rotatePtY = 0.0, unsigned char *color = NULL) { //, bool invertColors, double *colorMatrix) {
	if(!color) {
		GLubyte *WHITE = new GLubyte[4];
		WHITE[0] = 255;
		WHITE[1] = 255;
		WHITE[2] = 255;
		WHITE[3] = 255;
		color = WHITE;
	}
	glColor4ubv(color);

	glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, texName);
			/* Bilinear */
			if(!this->bilinear && bilinear) {
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				this->bilinear = true;
			}
			else if(this->bilinear && !bilinear) {
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
				this->bilinear = false;
			}

			/* Invert Colors 
			if(invertColors) {
				glEnable(GL_COLOR_TABLE);
			}*/

			// I really want to use this!
			/* Color Matrix 
			if(colorMatrix) {
				glMatrixMode(GL_COLOR);
				glPushMatrix();
				glLoadMatrixf((GLfloat *)colorMatrix);
				glMatrixMode(GL_MODELVIEW);
			}*/
			
			/* Modelview */
			glMatrixMode(GL_MODELVIEW);
			glPushMatrix();
				glTranslatef(x, y, 0);

				glTranslatef(rotatePtX, rotatePtY, 0);
				glRotatef(rotate, 0, 0, 1);
				glTranslatef(-rotatePtX, -rotatePtY, 0);

				glScalef(scaleX, scaleY, 1);
				

				/* Draw */
				glBegin(GL_QUADS);
					glTexCoord2f(0.0, 0.0);
						glVertex2f(0.0, 0.0);
					glTexCoord2f((GLfloat)widthRatio, 0.0);
						glVertex2f(imageWidth, 0.0);
					glTexCoord2f((GLfloat)widthRatio, heightRatio );
						glVertex2f(imageWidth, imageHeight);
					glTexCoord2f(0.0, heightRatio );
						glVertex2f(0.0, imageHeight);
				glEnd();
			glPopMatrix();
		glBindTexture(GL_TEXTURE_2D, 0);

		/* Finalize Color Matrix 
		if(colorMatrix) {
			glMatrixMode(GL_COLOR);
			glPopMatrix();
			glMatrixMode(GL_MODELVIEW);
		}*/
		/* Finalize Invert Colors 
		if(invertColors) {
			glDisable(GL_COLOR_TABLE);
		}*/
		
		//glMatrixMode(GL_MODELVIEW);
	glDisable(GL_TEXTURE_2D);
}

void Image::drawSubImg(Image *subImg, double x, double y, bool bilinear, double scaleX = 1.0, double scaleY = 1.0,
					   double rotate = 0.0, double rotatePtX = 0.0, double rotatePtY = 0.0, unsigned char *color = NULL) {
	windowHandler->bindFBO(texName, imageWidth, imageHeight);
		subImg->draw(x, y, bilinear, scaleX, scaleY, rotate, rotatePtX, rotatePtY, color);
	windowHandler->releaseFBO();
}

void Image::clear(unsigned char *color) {
	GLubyte red = color[0];
	GLubyte green = color[1];
	GLubyte blue = color[2];
	GLubyte alpha = color[3];

	windowHandler->bindFBO(texName, imageWidth, imageHeight);
		glClearColor(red, green, blue, alpha);
		glClear(GL_COLOR_BUFFER_BIT);
	windowHandler->releaseFBO();
}

void Image::drawPoints(double *coords, int dimension, unsigned char *color, double size, bool antiAliasing) {
	windowHandler->bindFBO(texName, imageWidth, imageHeight);
		windowHandler->cDrawPoints(coords, dimension, color, size, antiAliasing);
	windowHandler->releaseFBO();
}

void Image::drawLines(double *coords, int dimension, unsigned char *color, double width, bool loop,
				bool stipple, int stippleFactor, int stipplePattern, bool antiAliasing) {
	windowHandler->bindFBO(texName, imageWidth, imageHeight);
		windowHandler->cDrawLines(coords, dimension, color, width, loop, stipple, stippleFactor, stipplePattern, antiAliasing);
	windowHandler->releaseFBO();
}

void Image::drawTriangles(double *coords, int dimension, unsigned char *color, int form) {
	windowHandler->bindFBO(texName, imageWidth, imageHeight);
		windowHandler->cDrawTriangles(coords, dimension, color, form);
	windowHandler->releaseFBO();
}

void Image::drawPolygon(double *coords, int dimension, unsigned char *color) {
	windowHandler->bindFBO(texName, imageWidth, imageHeight);
		windowHandler->cDrawPolygon(coords, dimension, color);
	windowHandler->releaseFBO();
}




GLuint* Image::loadImage(const char *fileName)
{
	GLuint *imageInfo = new GLuint[5];

	ilGenImages(1, &imageName);
	ilBindImage(imageName);
	//ilLoadImage((wchar_t *)FileName);
	ilLoadImage(fileName);
	ilConvertImage(IL_RGBA, IL_UNSIGNED_BYTE);
	ILint imageWidth = ilGetInteger(IL_IMAGE_WIDTH);
	ILint imageHeight = ilGetInteger(IL_IMAGE_HEIGHT);

	int newWidth = getNextPowerOf2(imageWidth);
	int newHeight = getNextPowerOf2(imageHeight);
	iluEnlargeCanvas(newWidth, newHeight, 1);
	ILubyte *imageData = ilGetData();
	
	glGenTextures(1, &texName);
	glBindTexture(GL_TEXTURE_2D, texName);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, newWidth, newHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, imageData);
	glBindTexture(GL_TEXTURE_2D, 0);
	ilDeleteImages(1, &imageName);

	imageInfo[0] = texName;
	imageInfo[1] = (GLuint)imageWidth;
	imageInfo[2] = (GLuint)imageHeight;
	imageInfo[3] = (GLuint)newWidth;
	imageInfo[4] = (GLuint)newHeight;

	return imageInfo;	// maybe these should be returned as pointers to incoming parameters
}

GLuint* Image::loadImage(const char *fileName, int x, int y, int width, int height)
{
	GLuint *imageInfo = new GLuint[5];

	ilGenImages(1, &imageName);
	ilBindImage(imageName);
	//ilLoadImage((wchar_t *)FileName);
	ilLoadImage(fileName);
	ilConvertImage(IL_RGBA, IL_UNSIGNED_BYTE);
	//ILint imageWidth = ilGetInteger(IL_IMAGE_WIDTH);
	//ILint imageHeight = ilGetInteger(IL_IMAGE_HEIGHT);

	void *imageData = malloc( 4 * sizeof(ILubyte) * width * height );	// ILvoid *imageData = malloc( 4 * sizeof(ILubyte) * width * height );
	//ILvoid *imageData;
	ilCopyPixels(x, y, 0, width, height, 0, IL_RGBA, IL_UNSIGNED_BYTE, imageData);
	
	glGenTextures(1, &texName);
	glBindTexture(GL_TEXTURE_2D, texName);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		// This doesn't seem to work...
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, imageData);
	glBindTexture(GL_TEXTURE_2D, 0);
	ilDeleteImages(1, &imageName);
	delete imageData;

	imageInfo[0] = texName;
	imageInfo[1] = (GLuint)width;
	imageInfo[2] = (GLuint)height;
	imageInfo[3] = 0;	// Does this work?
	imageInfo[4] = 0;

	return imageInfo;
}

int Image::getNextPowerOf2(int nr) {
	for(int i=1; i<=LARGEST_SIZE_OF_IMAGE_PER_DIMENSION; i=i*2) {
		if(i >= nr) {
			return i;
		}
	}
	return -1;
}


void Image::setWindowHandlerPointer(WindowHandler *newWindowHandler) {
	windowHandler = newWindowHandler;
}


/* Static variables initialization: */
WindowHandler *Image::windowHandler = NULL;
