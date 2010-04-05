#ifndef WINDOW_HANDLER_H
#define WINDOW_HANDLER_H

#include <GL/GLee.h>	// Must be included before GLFW
#include <GL/glfw.h>	// For GLFW, OpenGL and GLU

#include <IL/il.h>		// DevIL
#include <IL/ilu.h>		// DevIL
#include <IL/ilut.h>	// DevIL


class WindowHandler {
public:
	WindowHandler();
	~WindowHandler();
	
	void cNewWindow(int width, int height, bool fullscreen);
	void cCloseWindow();
	void cSetWindowSize(int width, int height);
	double cGetTime();
	void cSetFPS(int newFPS);
	bool cGetKey(int key);
	void cGetMousePos(int *x, int *y);
	bool cGetMouseButton(int button);
	int cGetMouseWheel();
	void cShowMouseCursor(bool show);
	void cUpdate();
	void cDrawPoints(double *coords, int dimension, unsigned char *color, double size, bool antiAliasing);
	void cDrawLines(double *coords, int dimension, unsigned char *color, double width, bool loop,
					bool stipple, int stippleFactor, int stipplePattern, bool antiAliasing);
	void cDrawTriangles(double *coords, int dimension, unsigned char *color, int form);
	void cDrawPolygon(double *coords, int dimension, unsigned char *color);
	void cDrawEllipse(double x, double y, double a, double b, double alpha , unsigned char *color);
	void cSetClearColor(unsigned char *color);
	
	void bindFBO(GLuint texName, GLuint width, GLuint height);
	void releaseFBO();

private:
	enum TriangleForms { SEPARATE, STRIP, FAN };

	bool windowOpened;
	int windowWidth;
	int windowHeight;

	double prevTime;
	double fps;

	GLuint texName;
	ILubyte *imageData;

	GLuint fbo;
};

#endif WINDOW_HANDLER_H
