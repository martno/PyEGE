#include "WindowHandler.h"
#include "Image.h"
#include "Font.h"

#include <stdlib.h>    // For malloc() etc.
#include <stdio.h>     // For printf(), fopen() etc.
#include <math.h>      // For sin(), cos() etc.

#include <GL/GLee.h>	// Must be included before GLFW
#include <GL/glfw.h>	// For GLFW, OpenGL and GLU

#include <iostream>
#include <string>
#include <fstream>

#include <IL/il.h>		// DevIL
#include <IL/ilu.h>		// DevIL
#include <IL/ilut.h>	// DevIL

using namespace std;

#define PI 3.14159265



// All methods that starts with 'c' are methods called by python

WindowHandler::WindowHandler() {
	windowOpened = false;

	prevTime = 0;
	fps = 60;

	imageData = new ILubyte;

	// Initialize IL
	ilInit();
	ilOriginFunc(IL_ORIGIN_LOWER_LEFT);
	ilEnable(IL_ORIGIN_SET);
	// Initialize ILU
	iluInit();
	iluImageParameter(ILU_PLACEMENT, ILU_LOWER_LEFT);

	// Initialize ILUT with OpenGL support.
	//ilutRenderer(ILUT_OPENGL);

    //int    ok;             // Flag telling if the window was opened
    //int    running;        // Flag telling if the program is running

    // Initialize GLFW
    glfwInit();
	glfwSwapInterval(1);	// 1 => vsync on, 0 => vsync off

	//glfwOpenWindowHint(GLFW_WINDOW_NO_RESIZE, GL_TRUE);	// Doesn't seem to work =/
	

	Image::setWindowHandlerPointer(this);

	Font* defaultFont = new Font("C:\\Windows\\Fonts\\times.ttf", 10);
	Image::setDefaultFont( defaultFont );
}





WindowHandler::~WindowHandler() {
	/* GLFW */
    glfwTerminate();

}



void WindowHandler::cNewWindow(int width, int height, bool fullscreen) {
	if(!windowOpened) {
		windowOpened = true;
		windowWidth = width;
		windowHeight = height;

		if(fullscreen) {
			glfwOpenWindow(
				width, height,		// Width and height of window
				8, 8, 8,			// Number of red, green, and blue bits for color buffer
				8,					// Number of bits for alpha buffer
				0,					// Number of bits for depth buffer (Z-buffer)
				0,					// Number of bits for stencil buffer
				GLFW_FULLSCREEN		// We want a desktop window (could be GLFW_FULLSCREEN)
			);
		}
		else {
			glfwOpenWindow(
				width, height,		// Width and height of window
				8, 8, 8,			// Number of red, green, and blue bits for color buffer
				8,					// Number of bits for alpha buffer
				0,					// Number of bits for depth buffer (Z-buffer)
				0,					// Number of bits for stencil buffer
				GLFW_WINDOW			// We want a desktop window (could be GLFW_FULLSCREEN)
			);
		}
		// Set window title
		glfwSetWindowTitle( "CE" );

		// Enable sticky keys
		glfwEnable( GLFW_STICKY_KEYS );
		glfwEnable( GLFW_KEY_REPEAT );

		int width;
		int height;

		// Get current time
		double currentTime = glfwGetTime();
		prevTime = currentTime;
		
		// Get window size
		glfwGetWindowSize( &width, &height );

		// Make sure that height is non-zero to avoid division by zero
		height = height < 1 ? 1 : height;

		// Set viewport
		glViewport( 0, 0, width, height );

		glClearColor( 0.0f, 0.0f, 0.0f, 0.0f );
		glClear(GL_COLOR_BUFFER_BIT);

		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		gluOrtho2D(0.0, (GLfloat)width, 0.0, (GLfloat)height);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();

		/* Blend */
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		/* FBO */
		glGenFramebuffersEXT(1, &fbo);

		// Unable to use this. Is this the same problem as with convolution?
		// Better find an alternative to this..
		/* Color Table, Invert colors 
		GLubyte colorTable[256][3];
		for(int i = 0; i < 255; i++) {
			colorTable[i][0] = 255 - i;
			colorTable[i][1] = 255 - i;
			colorTable[i][2] = 255 - i;
		}
		cout << "glGetString(GL_EXTENSIONS)\n" << glGetString(GL_EXTENSIONS) << endl;
		//glColorTable(GL_COLOR_TABLE, GL_RGB, 256, GL_RGB, GL_UNSIGNED_BYTE, colorTable);
		*/

	}
	else {
		// throw something
	}
}

void WindowHandler::cCloseWindow() {
	if(windowOpened) {
		glfwCloseWindow();
		windowOpened = false;
	}
}

void WindowHandler::cSetWindowSize(int width, int height) {
	if(windowOpened) {
		glfwSetWindowSize(width, height);
		windowWidth = width;
		windowHeight = height;
	}
}

double WindowHandler::cGetTime() {
	return glfwGetTime();
}

void WindowHandler::cSetFPS(int newFPS) {
	fps = newFPS;
}

bool WindowHandler::cGetKey(int key) {
	return glfwGetKey(key);
}

void WindowHandler::cGetMousePos(int *x, int *y) {
	glfwGetMousePos(x, y);
	*y = windowHeight - *y;
}

bool WindowHandler::cGetMouseButton(int button) {
	return glfwGetMouseButton(button);
}

int WindowHandler::cGetMouseWheel() {
	return glfwGetMouseWheel();
}



void WindowHandler::cShowMouseCursor(bool show) {
	if(show) {
		glfwEnable( GLFW_MOUSE_CURSOR );
	}
	else {
		glfwDisable( GLFW_MOUSE_CURSOR );
	}
}

void WindowHandler::cUpdate() {
	//sf::RenderWindow App(sf::VideoMode(800, 600, 32), "SFML Graphics");
	//sf::Shape Line = sf::Shape::Line(10, 10, 100, 100, 3, sf::Color(255, 255, 255));

	glfwSwapBuffers();

	int    width, height;  // Window dimensions
	// needed if the user has changed the size
    
	double currentTime = glfwGetTime();
	while( (glfwGetTime() - prevTime) < (1/fps) ) {
		// do nothing
	}
	prevTime = glfwGetTime();

    // Get window size
    glfwGetWindowSize( &width, &height );

    // Make sure that height is non-zero to avoid division by zero
    height = height < 1 ? 1 : height;

	// Krävs verkligen _alla_ de här varje gång skärmen uppdateras?
    // Set viewport
    glViewport( 0, 0, width, height );

	glClear(GL_COLOR_BUFFER_BIT);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0.0, (GLfloat)width, 0.0, (GLfloat)height);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	/* Disable */
	//glDisable(GL_CONVOLUTION_2D);
	// I think i added this so that convolution was disabled if it was enabled in another method

}

void WindowHandler::cDrawPoints(double *coords, int dimension, unsigned char *color, double size, bool antiAliasing) {
	/* Color */
	glColor4ubv((GLubyte*)color);

	/* Size */
	glPointSize( (GLfloat)size );

	// A bit heavy to change this every time, check how it was earlier instead
	/* AntiAliasing */
	if(antiAliasing) {
		glEnable(GL_POINT_SMOOTH);
		glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);
	}
	else {
		glDisable(GL_POINT_SMOOTH);
		//glHint(GL_POINT_SMOOTH, GL_FASTEST);	// Why did I comment out this line?
	}

	/* Draw */
	glBegin(GL_POINTS);
		for(int i = 0; i < dimension; i++) {
			glVertex2f( (GLfloat)coords[2*i], (GLfloat)coords[2*i+1] );
		}
	glEnd();
}

void WindowHandler::cDrawLines(double *coords, int dimension, unsigned char *color, double width, bool loop,
				bool stipple, int stippleFactor, int stipplePattern, bool antiAliasing) {
	/* Color */
	glColor4ubv((GLubyte*)color);
	
	/* Width */
	glLineWidth( (GLfloat)width );

	/* Stipple */
	if(stipple) {
		glEnable(GL_LINE_STIPPLE);
		glLineStipple(stippleFactor, stipplePattern);
	}
	else {
		glDisable(GL_LINE_STIPPLE);
	}

	/* AntiAliasing */
	if(antiAliasing) {
		glEnable(GL_LINE_SMOOTH);
		glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
	}
	else {
		glDisable(GL_LINE_SMOOTH);
		//glHint(GL_POINT_SMOOTH, GL_FASTEST);
	}
	
	/* Loop, Draw */
	if(loop){
		glBegin(GL_LINE_LOOP);
	}
	else {
		glBegin(GL_LINE_STRIP);
	}
		for(int i = 0; i < dimension; i++) {
			glVertex2f( (GLfloat)coords[2*i], (GLfloat)coords[2*i+1] );
		}
	glEnd();
}

void WindowHandler::cDrawTriangles(double *coords, int dimension, unsigned char *color, int form) {
	/* Color */
	glColor4ubv((GLubyte*)color);

	/* Form, Draw */
	switch(form) {
		case SEPARATE:
			glBegin(GL_TRIANGLES);
			break;
		case STRIP:
			glBegin(GL_TRIANGLE_STRIP);
			break;
		case FAN:
			glBegin(GL_TRIANGLE_FAN);
			break;
	}
		for(int i = 0; i < dimension; i++) {
			glVertex2f( (GLfloat)coords[2*i], (GLfloat)coords[2*i+1] );
		}
	glEnd();
}

// Change this method so that it can handle arbitrary polygons, a.k.a. tesselators
void WindowHandler::cDrawPolygon(double *coords, int dimension, unsigned char *color) {
	/* Color */
	glColor4ubv((GLubyte*)color);

	/* Draw */
	glBegin(GL_POLYGON);
		for(int i = 0; i < dimension; i++) {
			glVertex2f( (GLfloat)coords[2*i], (GLfloat)coords[2*i+1] );
		}
	glEnd();
}

void WindowHandler::cDrawEllipse(double x, double y, double a, double b, double rotate , unsigned char *color) {
	/* Color */
	glColor4ubv((GLubyte*)color);

	/* Draw */
	int noOfPolygonPts;
	if(a >= b) {
		noOfPolygonPts = (int)a*2*PI;
	}
	else {
		noOfPolygonPts = (int)b*2*PI;
	}
	double deltaTheta = (double)1/noOfPolygonPts;

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
		glTranslatef(x, y, 0);
		glRotatef(rotate, 0, 0, 1);
		glBegin(GL_POLYGON);
			for(int i = 0; i < noOfPolygonPts; i++) {
				glVertex2f( (GLfloat)a*cos(i*deltaTheta*2*PI), 
							(GLfloat)b*sin(i*deltaTheta*2*PI) );
			}
		glEnd();
	glPopMatrix();
}

void WindowHandler::cSetClearColor(unsigned char *color) {
	GLubyte red = color[0];
	GLubyte green = color[1];
	GLubyte blue = color[2];
	GLubyte alpha = color[3];

	glClearColor(red, green, blue, alpha);
}


/* FBO */
void WindowHandler::bindFBO(GLuint texName, GLuint width, GLuint height) {
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fbo);
	glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, texName, 0);

	glPushAttrib(GL_COLOR_BUFFER_BIT);
	//glPushAttrib(GL_VIEWPORT_BIT | GL_COLOR_BUFFER_BIT);
	//glViewport(0, 0, width, height);

	if( !(glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT) == GL_FRAMEBUFFER_COMPLETE_EXT) ) {
		cout << "FBO error!" << endl;
	}
}

void WindowHandler::releaseFBO() {
	glPopAttrib();
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
}



/*
void WindowHandler::cSetWindowTitle(char *title) {
	char *str = new char[strlen(title) + 5];
	strcpy(str, title);
	strcat(str, " [CE]");
	glfwSetWindowTitle( str );
}

void WindowHandler::cApplyConvolution(float *convolution, int width, int height) {
	glEnable(GL_CONVOLUTION_2D);
	glConvolutionFilter2D(GL_CONVOLUTION_2D, GL_RGB , width, height, 
		GL_RGB, GL_FLOAT, convolution);
}

*/