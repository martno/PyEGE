#ifndef FONT_H
#define FONT_H

//#include <wingdi.h>
//#include <windows.h>

#include <GL/GLee.h>	// Must be included before GLFW
#include <GL/glfw.h>	// For GLFW, OpenGL and GLU
//#include <config/config.h.in>

#include <GLFT_Font.hpp>

class Font
{
public:
	Font(char* fontname, int size);
	~Font();

	GLFT_Font* getGLFTFont();

private:
	char* fontname;
	int size;
	GLFT_Font* glftFont;

};

#endif FONT_H
