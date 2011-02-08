#include "Font.h"
#include <iostream>

using namespace std;

Font::Font(char* fontname, int size)
{
	//cout << "font" << endl;
	this->fontname = fontname;
	this->size = size;
	glftFont = new GLFT_Font(fontname, size);
	//cout << "font2" << endl;
	/*if ( face == 0 || !face->isValid() ) {
		cout << "Could not construct face from " << fontname << endl;
	}*/
}

Font::~Font()
{
	delete glftFont;
}


GLFT_Font* Font::getGLFTFont()
{
	return glftFont;
}
