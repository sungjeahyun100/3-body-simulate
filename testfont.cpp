#include "src/font.h"
#include "src/ui.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>

// 전역 변수로 현재 창 크기 저장
int g_windowWidth = 1200;
int g_windowHeight = 800;

void screenToGL(double screenX, double screenY, float& glX, float& glY, int width, int height) {
    // 기본 변환
    glX = (2.0f * screenX / width) - 1.0f;
    glY = 1.0f - (2.0f * screenY / height);
    
    // 종횡비 조정 (투영 매트릭스와 일치하도록)
    float aspect = (float)width / (float)height;
    if (aspect > 1.0f) {
        // 가로가 더 넓은 경우
        glX *= aspect;
    } else {
        // 세로가 더 넓은 경우
        glY /= aspect;
    }
}

// 창 크기 변경 콜백
void framebufferSizeCallback(GLFWwindow* window, int width, int height) {
    g_windowWidth = width;
    g_windowHeight = height;
    glViewport(0, 0, width, height);
    
    // UI에 창 크기 정보 전달
    setWindowSize(width, height);
    
    // 종횡비 유지를 위한 투영 매트릭스 설정
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    
    float aspect = (float)width / (float)height;
    if (aspect > 1.0f) {
        // 가로가 더 넓은 경우
        glOrtho(-aspect, aspect, -1.0f, 1.0f, -1.0f, 1.0f);
    } else {
        // 세로가 더 넓은 경우
        glOrtho(-1.0f, 1.0f, -1.0f/aspect, 1.0f/aspect, -1.0f, 1.0f);
    }
    
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

int main(){
        if (!glfwInit()) {
        std::cerr << "GLFW 초기화 실패" << std::endl;
        return -1;
    }
    
    GLFWwindow* window = glfwCreateWindow(1200, 800, "font testing", NULL, NULL);
    if (!window) {
        glfwTerminate();
        return -1;
    }
    
    glfwMakeContextCurrent(window);
    
    if (glewInit() != GLEW_OK) {
        std::cerr << "GLEW 초기화 실패" << std::endl;
        return -1;
    }
    
    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
    
    // 초기 창 크기 설정
    glfwGetFramebufferSize(window, &g_windowWidth, &g_windowHeight);
    framebufferSizeCallback(window, g_windowWidth, g_windowHeight);
    
    
    // UI 상태는 이미 기본값으로 초기화됨
    
    glClearColor(0.0f, 0.0f, 0.05f, 1.0f);
    
    while (!glfwWindowShouldClose(window)) {
        
        drawText("abcdefghijklmnopqrstuvwxyz;:,.-1234567890[]|", 0.0f, 0.0f, 0.025);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    
    glfwTerminate();
    return 0;
}

// Grid points for consistent alignment - 가로 사이즈를 줄임 (0.8배)
//float charWidth = size * 0.8f;
//float x0 = x, 
//x1 = x + charWidth*0.25f, 
//x2 = x + charWidth*0.5f, 
//x3 = x + charWidth*0.75f, 
//x4 = x + charWidth;
//float y0 = y, 
//y1 = y + size*0.25f, 
//y2 = y + size*0.5f, 
//y3 = y + size*0.75f, 
//y4 = y + size;