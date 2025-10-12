#include "font.h"
#include <GL/glew.h>

// Clean grid-based font with proper alignment
void drawChar(char c, float x, float y, float size) {
    glLineWidth(1.0f);
    
    // Grid points for consistent alignment - 가로 사이즈를 줄임 (0.8배)
    float charWidth = size * 0.8f;
    float x0 = x, x1 = x + charWidth*0.25f, x2 = x + charWidth*0.5f, x3 = x + charWidth*0.75f, x4 = x + charWidth;
    float y0 = y, y1 = y + size*0.25f, y2 = y + size*0.5f, y3 = y + size*0.75f, y4 = y + size;
    
    switch(c) {
        case '0':
            glBegin(GL_LINE_LOOP);
            glVertex2f(x1, y0); glVertex2f(x3, y0); 
            glVertex2f(x4, y1); glVertex2f(x4, y3);
            glVertex2f(x3, y4); glVertex2f(x1, y4);
            glVertex2f(x0, y3); glVertex2f(x0, y1);
            glEnd();
            break;
        case '1':
            glBegin(GL_LINES);
            glVertex2f(x2, y0); glVertex2f(x2, y4);
            glVertex2f(x1, y3); glVertex2f(x2, y4);
            glVertex2f(x0, y0); glVertex2f(x4, y0);
            glEnd();
            break;
        case '2':
            glBegin(GL_LINE_STRIP);
            glVertex2f(x0, y3); glVertex2f(x1, y4); 
            glVertex2f(x3, y4); glVertex2f(x4, y3);
            glVertex2f(x4, y2); glVertex2f(x0, y2);
            glVertex2f(x0, y1); glVertex2f(x3, y0);
            glVertex2f(x4, y0);
            glEnd();
            break;
        case '3':
            glBegin(GL_LINE_STRIP);
            glVertex2f(x0, y4); glVertex2f(x3, y4); 
            glVertex2f(x4, y3); glVertex2f(x3, y2);
            glVertex2f(x2, y2);
            glEnd();
            glBegin(GL_LINE_STRIP);
            glVertex2f(x3, y2); glVertex2f(x4, y1);
            glVertex2f(x3, y0); glVertex2f(x0, y0);
            glEnd();
            break;
        case '4':
            glBegin(GL_LINES);
            glVertex2f(x0, y4); glVertex2f(x0, y2);
            glVertex2f(x0, y2); glVertex2f(x4, y2);
            glVertex2f(x3, y0); glVertex2f(x3, y4);
            glEnd();
            break;
        case '5':
            glBegin(GL_LINE_STRIP);
            glVertex2f(x4, y4); glVertex2f(x0, y4);
            glVertex2f(x0, y2); glVertex2f(x3, y2);
            glVertex2f(x4, y1); glVertex2f(x4, y0);
            glVertex2f(x1, y0); glVertex2f(x0, y1);
            glEnd();
            break;
        case '6':
            glBegin(GL_LINE_STRIP);
            glVertex2f(x4, y3); glVertex2f(x3, y4);
            glVertex2f(x1, y4); glVertex2f(x0, y3);
            glVertex2f(x0, y1); glVertex2f(x1, y0);
            glVertex2f(x3, y0); glVertex2f(x4, y1);
            glVertex2f(x4, y2); glVertex2f(x0, y2);
            glEnd();
            break;
        case '7':
            glBegin(GL_LINE_STRIP);
            glVertex2f(x0, y4); glVertex2f(x4, y4);
            glVertex2f(x2, y0);
            glEnd();
            break;
        case '8':
            glBegin(GL_LINE_LOOP);
            glVertex2f(x1, y2); glVertex2f(x1, y4);
            glVertex2f(x3, y4); glVertex2f(x3, y2);
            glEnd();
            glBegin(GL_LINE_LOOP);
            glVertex2f(x1, y2); glVertex2f(x1, y0);
            glVertex2f(x3, y0); glVertex2f(x3, y2);
            glEnd();
            break;
        case '9':
            glBegin(GL_LINE_STRIP);
            glVertex2f(x0, y1); glVertex2f(x1, y0);
            glVertex2f(x3, y0); glVertex2f(x4, y1);
            glVertex2f(x4, y3); glVertex2f(x3, y4);
            glVertex2f(x1, y4); glVertex2f(x0, y3);
            glVertex2f(x0, y2); glVertex2f(x4, y2);
            glEnd();
            break;
        case '-':
            glBegin(GL_LINES);
            glVertex2f(x1, y2); glVertex2f(x3, y2);
            glEnd();
            break;
        case '.':
            glBegin(GL_QUADS);
            glVertex2f(x2-size*0.1f, y0); glVertex2f(x2+size*0.1f, y0);
            glVertex2f(x2+size*0.1f, y1); glVertex2f(x2-size*0.1f, y1);
            glEnd();
            break;
        case ':':
            glBegin(GL_QUADS);
            glVertex2f(x2-size*0.1f, y0); glVertex2f(x2+size*0.1f, y0);
            glVertex2f(x2+size*0.1f, y1); glVertex2f(x2-size*0.1f, y1);
            glEnd();
            glBegin(GL_QUADS);
            glVertex2f(x2-size*0.1f, y3); glVertex2f(x2+size*0.1f, y3);
            glVertex2f(x2+size*0.1f, y4); glVertex2f(x2-size*0.1f, y4);
            glEnd();
            break;
        case ';':
            glBegin(GL_QUADS);
            glVertex2f(x2-charWidth*0.1f, y0); glVertex2f(x2+charWidth*0.1f, y0);
            glVertex2f(x2+charWidth*0.1f, y2); glVertex2f(x2-charWidth*0.1f, y2);
            glEnd();
            glBegin(GL_QUADS);
            glVertex2f(x2-size*0.1f, y3); glVertex2f(x2+size*0.1f, y3);
            glVertex2f(x2+size*0.1f, y4); glVertex2f(x2-size*0.1f, y4);
            glEnd();
            break;
        case ',':
            glBegin(GL_QUADS);
            glVertex2f(x2-size*0.1f, y0); glVertex2f(x2+size*0.1f, y0);
            glVertex2f(x2+size*0.1f, y2); glVertex2f(x2-size*0.1f, y2);
            glEnd();
            break;
        case '[':
            glBegin(GL_LINE_STRIP);
            glVertex2f(x4, y0); glVertex2f(x0, y0);
            glVertex2f(x0, y4); glVertex2f(x4, y4);
            glEnd();
            break;
        case ']':
            glBegin(GL_LINE_STRIP);
            glVertex2f(x0, y0); glVertex2f(x4, y0);
            glVertex2f(x4, y4); glVertex2f(x0, y4);
            glEnd();
            break;
        case ' ':
            // Space character - draw nothing
            break;
        // Complete alphabet A-Z, a-z (grid-aligned)
        case 'A': case 'a':
            glBegin(GL_LINE_STRIP);
            glVertex2f(x0, y0); glVertex2f(x0, y3);
            glVertex2f(x2, y4); glVertex2f(x4, y3);
            glVertex2f(x4, y0);
            glEnd();
            glBegin(GL_LINES);
            glVertex2f(x1, y2); glVertex2f(x3, y2);
            glEnd();
            break;
        case 'B': case 'b':
            glBegin(GL_LINE_STRIP);
            glVertex2f(x0, y0); glVertex2f(x0, y4);
            glVertex2f(x3, y4); glVertex2f(x3, y2);
            glVertex2f(x0, y2); glVertex2f(x3, y2);
            glVertex2f(x3, y0); glVertex2f(x0, y0);
            glEnd();
            break;
        case 'C': case 'c':
            glBegin(GL_LINE_STRIP);
            glVertex2f(x4, y1); glVertex2f(x3, y0);
            glVertex2f(x1, y0); glVertex2f(x0, y1);
            glVertex2f(x0, y3); glVertex2f(x1, y4);
            glVertex2f(x3, y4); glVertex2f(x4, y3);
            glEnd();
            break;
        case 'D': case 'd':
            glBegin(GL_LINE_STRIP);
            glVertex2f(x0, y0); glVertex2f(x0, y4);
            glVertex2f(x3, y4); glVertex2f(x4, y3);
            glVertex2f(x4, y1); glVertex2f(x3, y0);
            glVertex2f(x0, y0);
            glEnd();
            break;
        case 'E': case 'e':
            glBegin(GL_LINE_STRIP);
            glVertex2f(x4, y0); glVertex2f(x0, y0);
            glVertex2f(x0, y4); glVertex2f(x4, y4);
            glEnd();
            glBegin(GL_LINES);
            glVertex2f(x0, y2); glVertex2f(x3, y2);
            glEnd();
            break;
        case 'F': case 'f':
            glBegin(GL_LINE_STRIP);
            glVertex2f(x0, y0); glVertex2f(x0, y4);
            glVertex2f(x4, y4);
            glEnd();
            glBegin(GL_LINES);
            glVertex2f(x0, y2); glVertex2f(x3, y2);
            glEnd();
            break;
        case 'G': case 'g':
            glBegin(GL_LINE_STRIP);
            glVertex2f(x4, y1); glVertex2f(x3, y0);
            glVertex2f(x1, y0); glVertex2f(x0, y1);
            glVertex2f(x0, y3); glVertex2f(x1, y4);
            glVertex2f(x3, y4); glVertex2f(x4, y3);
            glVertex2f(x4, y2); glVertex2f(x2, y2);
            glEnd();
            break;
        case 'H': case 'h':
            glBegin(GL_LINES);
            glVertex2f(x0, y0); glVertex2f(x0, y4);
            glVertex2f(x4, y0); glVertex2f(x4, y4);
            glVertex2f(x0, y2); glVertex2f(x4, y2);
            glEnd();
            break;
        case 'I': case 'i':
            glBegin(GL_LINES);
            glVertex2f(x2, y0); glVertex2f(x2, y4);
            glVertex2f(x1, y0); glVertex2f(x3, y0);
            glVertex2f(x1, y4); glVertex2f(x3, y4);
            glEnd();
            break;
        case 'J': case 'j':
            glBegin(GL_LINE_STRIP);
            glVertex2f(x0, y1); glVertex2f(x1, y0);
            glVertex2f(x3, y0); glVertex2f(x4, y1);
            glVertex2f(x4, y4);
            glEnd();
            break;
        case 'K': case 'k':
            glBegin(GL_LINES);
            glVertex2f(x0, y0); glVertex2f(x0, y4);
            glVertex2f(x0, y2); glVertex2f(x4, y4);
            glVertex2f(x0, y2); glVertex2f(x4, y0);
            glEnd();
            break;
        case 'L': case 'l':
            glBegin(GL_LINE_STRIP);
            glVertex2f(x0, y4); glVertex2f(x0, y0);
            glVertex2f(x4, y0);
            glEnd();
            break;
        case 'M': case 'm':
            glBegin(GL_LINE_STRIP);
            glVertex2f(x0, y0); glVertex2f(x0, y4);
            glVertex2f(x2, y2); glVertex2f(x4, y4);
            glVertex2f(x4, y0);
            glEnd();
            break;
        case 'N': case 'n':
            glBegin(GL_LINE_STRIP);
            glVertex2f(x0, y0); glVertex2f(x0, y4);
            glVertex2f(x4, y0); glVertex2f(x4, y4);
            glEnd();
            break;
        case 'O': case 'o':
            glBegin(GL_LINE_LOOP);
            glVertex2f(x1, y0); glVertex2f(x3, y0);
            glVertex2f(x4, y1); glVertex2f(x4, y3);
            glVertex2f(x3, y4); glVertex2f(x1, y4);
            glVertex2f(x0, y3); glVertex2f(x0, y1);
            glEnd();
            break;
        case 'P': case 'p':
            glBegin(GL_LINE_STRIP);
            glVertex2f(x0, y0); glVertex2f(x0, y4);
            glVertex2f(x3, y4); glVertex2f(x3, y2);
            glVertex2f(x0, y2);
            glEnd();
            break;
        case 'Q': case 'q':
            glBegin(GL_LINE_LOOP);
            glVertex2f(x1, y0); glVertex2f(x3, y0);
            glVertex2f(x4, y1); glVertex2f(x4, y3);
            glVertex2f(x3, y4); glVertex2f(x1, y4);
            glVertex2f(x0, y3); glVertex2f(x0, y1);
            glEnd();
            glBegin(GL_LINES);
            glVertex2f(x2, y1); glVertex2f(x4, y0);
            glEnd();
            break;
        case 'R': case 'r':
            glBegin(GL_LINE_STRIP);
            glVertex2f(x0, y0); glVertex2f(x0, y4);
            glVertex2f(x3, y4); glVertex2f(x3, y2);
            glVertex2f(x0, y2);
            glEnd();
            glBegin(GL_LINES);
            glVertex2f(x1, y2); glVertex2f(x4, y0);
            glEnd();
            break;
        case 'S': case 's':
            glBegin(GL_LINE_STRIP);
            glVertex2f(x4, y1); glVertex2f(x3, y0);
            glVertex2f(x1, y0); glVertex2f(x0, y1);
            glVertex2f(x0, y2); glVertex2f(x4, y2);
            glVertex2f(x4, y3); glVertex2f(x3, y4);
            glVertex2f(x1, y4); glVertex2f(x0, y3);
            glEnd();
            break;
        case 'T': case 't':
            glBegin(GL_LINES);
            glVertex2f(x0, y4); glVertex2f(x4, y4);
            glVertex2f(x2, y0); glVertex2f(x2, y4);
            glEnd();
            break;
        case 'U': case 'u':
            glBegin(GL_LINE_STRIP);
            glVertex2f(x0, y4); glVertex2f(x0, y1);
            glVertex2f(x1, y0); glVertex2f(x3, y0);
            glVertex2f(x4, y1); glVertex2f(x4, y4);
            glEnd();
            break;
        case 'V': case 'v':
            glBegin(GL_LINE_STRIP);
            glVertex2f(x0, y4); glVertex2f(x2, y0);
            glVertex2f(x4, y4);
            glEnd();
            break;
        case 'W': case 'w':
            glBegin(GL_LINE_STRIP);
            glVertex2f(x0, y4); glVertex2f(x1, y0);
            glVertex2f(x2, y2); glVertex2f(x3, y0);
            glVertex2f(x4, y4);
            glEnd();
            break;
        case 'X': case 'x':
            glBegin(GL_LINES);
            glVertex2f(x0, y0); glVertex2f(x4, y4);
            glVertex2f(x4, y0); glVertex2f(x0, y4);
            glEnd();
            break;
        case 'Y': case 'y':
            glBegin(GL_LINES);
            glVertex2f(x0, y4); glVertex2f(x2, y2);
            glVertex2f(x4, y4); glVertex2f(x2, y2);
            glVertex2f(x2, y2); glVertex2f(x2, y0);
            glEnd();
            break;
        case 'Z': case 'z':
            glBegin(GL_LINE_STRIP);
            glVertex2f(x0, y4); glVertex2f(x4, y4);
            glVertex2f(x0, y0); glVertex2f(x4, y0);
            glEnd();
            break;
        default:
            // Unknown characters displayed as small rectangles
            glBegin(GL_LINE_LOOP);
            glVertex2f(x0, y0); glVertex2f(x3, y0);
            glVertex2f(x3, y3); glVertex2f(x0, y3);
            glEnd();
            break;
    }
}

// Text display function
void drawText(const std::string& text, float x, float y, float size) {
    float currentX = x;
    float charWidth = size * 0.8f;  // 문자 실제 폭
    float spacing = size * 1.1f;    // 문자 간격을 늘림 (0.9f → 1.1f)
    
    for (char c : text) {
        drawChar(c, currentX, y, size);
        currentX += spacing;
    }
}