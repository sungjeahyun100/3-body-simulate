#ifndef FONT_H
#define FONT_H

#include <GL/glew.h>
#include <string>

// Font rendering functions
void drawChar(char c, float x, float y, float size);
void drawText(const std::string& text, float x, float y, float size);

#endif // FONT_H