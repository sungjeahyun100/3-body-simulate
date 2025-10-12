#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>
#include <cmath>
#include "src/font.h"
#include "src/physics.h"
#include "src/ui.h"

// 전역 변수들
std::vector<Body> bodies;

std::vector<Body> initBodies = {
    {0.0f, -0.07f, 0.0f, 0.0f, 650000.0f, 1.0f, 0.0f, 0.0f},   // 빨강-태양
    {-0.85f - 0.06f, -0.07f, 0.0f, 0.235f + 0.061f, 100.0f, 0.0f, 1.0f, 0.0f},    // 초록-달
    {-0.85f, -0.07f, 0.0f, 0.235f, 4000.0f, 0.0f, 0.0f, 1.0f}     // 파랑-지구
};

UIState uiState;
OrbitTrails orbitTrails;

// 전역 변수로 현재 창 크기 저장
int g_windowWidth = 1200;
int g_windowHeight = 800;

void screenToGL(double screenX, double screenY, float& glX, float& glY, int width, int height) {
    // 줌을 고려한 좌표 변환을 위해 screenToWorld 함수를 사용
    screenToWorld(screenX, screenY, glX, glY, uiState, width, height);
}

// 창 크기 변경 콜백
void framebufferSizeCallback(GLFWwindow* window, int width, int height) {
    g_windowWidth = width;
    g_windowHeight = height;
    glViewport(0, 0, width, height);
    
    // UI에 창 크기 정보 전달
    setWindowSize(width, height);
    
    // 줌을 고려한 투영 매트릭스 설정
    applyZoomToProjection(uiState, width, height);
}

int findNearestBody(float x, float y) {
    int nearest = -1;
    float minDist = 0.1f;
    
    for (size_t i = 0; i < bodies.size(); i++) {
        float dx = bodies[i].x - x;
        float dy = bodies[i].y - y;
        float dist = sqrt(dx * dx + dy * dy);
        
        if (dist < minDist) {
            minDist = dist;
            nearest = i;
        }
    }
    return nearest;
}

void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
    if (button == GLFW_MOUSE_BUTTON_LEFT) {
        if (action == GLFW_PRESS) {
            double mouseX, mouseY;
            glfwGetCursorPos(window, &mouseX, &mouseY);
            
            float glX, glY;
            screenToGL(mouseX, mouseY, glX, glY, g_windowWidth, g_windowHeight);
            
            // 선택된 물체의 속도 벡터를 클릭했는지 확인 (편집 모드가 아닐 때만)
            if (uiState.editMode == EditMode::NONE && uiState.selectedBody != -1 && 
                isPointOnVelocityVector(bodies, uiState, glX, glY)) {
                std::cout << "Starting velocity vector drag" << std::endl;
                startVelocityVectorDrag(uiState, glX, glY);
                return;
            }
            
            // 속도 편집 모드에서 방향 드래그 시작
            if (uiState.editMode == EditMode::VELOCITY && uiState.selectedBody != -1) {
                std::cout << "Starting direction drag for velocity" << std::endl;
                startDirectionDrag(uiState, glX, glY);
                return;
            }
            
            // 물체 추가 모드에서 위치 설정
            if (uiState.editMode == EditMode::ADD_BODY) {
                std::cout << "Setting new body position: (" << glX << ", " << glY << ")" << std::endl;
                setNewBodyPosition(uiState, glX, glY);
                return;
            }
            
            // 다른 편집 모드에서는 클릭 무시
            if (uiState.editMode != EditMode::NONE && uiState.selectedBody != -1) {
                return;
            }
            
            uiState.selectedBody = findNearestBody(glX, glY);
            if (uiState.selectedBody != -1) {
                uiState.dragging = true;
                
                std::cout << "Selected Body " << uiState.selectedBody + 1 << std::endl;
                std::cout << "Position: (" << bodies[uiState.selectedBody].x << ", " << bodies[uiState.selectedBody].y << ")" << std::endl;
                std::cout << "Velocity: (" << bodies[uiState.selectedBody].vx << ", " << bodies[uiState.selectedBody].vy << ")" << std::endl;
                std::cout << "Mass: " << bodies[uiState.selectedBody].mass << std::endl;
            }
        }
        else if (action == GLFW_RELEASE) {
            if (uiState.draggingVelocityVector) {
                std::cout << "Finishing velocity vector drag" << std::endl;
                finishVelocityVectorDrag(uiState);
            }
            if (uiState.draggingDirection) {
                std::cout << "Applying direction drag" << std::endl;
                applyDirectionDrag(uiState);
            }
            if (uiState.panning) {
                stopPanning(uiState);
            }
            uiState.dragging = false;
            uiState.draggingDirection = false;
        }
    }
    else if (button == GLFW_MOUSE_BUTTON_RIGHT) {
        if (action == GLFW_PRESS) {
            // 우클릭으로 화면 패닝 시작 (편집 모드가 아닐 때만)
            if (uiState.editMode == EditMode::NONE) {
                double mouseX, mouseY;
                glfwGetCursorPos(window, &mouseX, &mouseY);
                startPanning(uiState, mouseX, mouseY);
            }
        }
        else if (action == GLFW_RELEASE) {
            if (uiState.panning) {
                stopPanning(uiState);
            }
        }
    }
}

void cursorPosCallback(GLFWwindow* window, double xpos, double ypos) {
    // 패닝 업데이트 (화면 좌표로 처리)
    if (uiState.panning) {
        updatePanning(uiState, xpos, ypos, g_windowWidth, g_windowHeight);
        applyZoomToProjection(uiState, g_windowWidth, g_windowHeight);
        return;
    }
    
    float glX, glY;
    screenToGL(xpos, ypos, glX, glY, g_windowWidth, g_windowHeight);
    
    if (uiState.draggingVelocityVector) {
        // 속도 벡터 직접 드래그 업데이트
        updateVelocityVectorDrag(bodies, uiState, glX, glY);
    } else if (uiState.draggingDirection) {
        // 방향 드래그 업데이트
        updateDirectionDrag(uiState, glX, glY);
    } else if (uiState.dragging && uiState.selectedBody != -1) {
        // 일반 물체 드래그
        bodies[uiState.selectedBody].x = glX;
        bodies[uiState.selectedBody].y = glY;
    }
}

// 마우스 휠 콜백 (줌 기능)
void scrollCallback(GLFWwindow* window, double xoffset, double yoffset) {
    // 편집 모드에서는 줌 비활성화
    if (uiState.editMode != EditMode::NONE) {
        return;
    }
    
    // 줌 변경량 계산
    float zoomDelta = yoffset * uiState.zoomSensitivity;
    
    // 줌 업데이트 (화면 중앙 기준, 마우스 위치 무시)
    updateZoom(uiState, zoomDelta);
    
    // 투영 매트릭스 업데이트
    applyZoomToProjection(uiState, g_windowWidth, g_windowHeight);
}

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (action == GLFW_PRESS) {
        switch (key) {
            case GLFW_KEY_ESCAPE:
                if (uiState.editMode == EditMode::DELETE_CONFIRM) {
                    // 삭제 확인 취소
                    std::cout << "Canceling delete confirmation" << std::endl;
                    cancelDeleteConfirm(uiState);
                } else if (uiState.editMode != EditMode::NONE) {
                    // 편집 취소
                    std::cout << "Canceling edit mode" << std::endl;
                    cancelEdit(uiState);
                } else {
                    glfwSetWindowShouldClose(window, GLFW_TRUE);
                }
                break;
            case GLFW_KEY_SPACE:
                if (uiState.editMode == EditMode::NONE) {
                    uiState.paused = !uiState.paused;
                }
                break;
            case GLFW_KEY_R:
                // 초기화
                bodies = initBodies;
                uiState.selectedBody = -1;
                uiState.dragging = false;
                uiState.paused = false;
                uiState.editMode = EditMode::NONE;
                // 줌 리셋
                resetZoom(uiState);
                applyZoomToProjection(uiState, g_windowWidth, g_windowHeight);
                // 궤도도 초기화
                initializeTrails(orbitTrails, bodies.size());
                break;
            case GLFW_KEY_I:
                uiState.showInfo = !uiState.showInfo;
                break;
            case GLFW_KEY_H:
                // 크로스헤어 토글
                uiState.showCrosshair = !uiState.showCrosshair;
                std::cout << "Crosshair " << (uiState.showCrosshair ? "enabled" : "disabled") << std::endl;
                break;
            case GLFW_KEY_V:
                // 속도 편집 모드 토글
                std::cout << "V key pressed. Selected body: " << uiState.selectedBody << std::endl;
                if (uiState.selectedBody != -1) {
                    if (uiState.editMode == EditMode::VELOCITY) {
                        std::cout << "Canceling velocity edit" << std::endl;
                        cancelEdit(uiState);
                    } else {
                        std::cout << "Starting velocity edit" << std::endl;
                        uiState.editMode = EditMode::VELOCITY;
                        uiState.currentEditField = 0;
                        handleVelocityEdit(bodies, uiState);
                    }
                } else {
                    std::cout << "No body selected. Click on a body first." << std::endl;
                }
                break;
                
            case GLFW_KEY_M:
                // 질량 편집 모드 토글
                std::cout << "M key pressed. Selected body: " << uiState.selectedBody << std::endl;
                if (uiState.selectedBody != -1) {
                    if (uiState.editMode == EditMode::MASS) {
                        std::cout << "Canceling mass edit" << std::endl;
                        cancelEdit(uiState);
                    } else {
                        std::cout << "Starting mass edit" << std::endl;
                        uiState.editMode = EditMode::MASS;
                        uiState.currentEditField = 0;
                        handleMassEdit(bodies, uiState);
                    }
                } else {
                    std::cout << "No body selected. Click on a body first." << std::endl;
                }
                break;
                
            case GLFW_KEY_P:
                // 물체 추가 모드 토글
                std::cout << "P key pressed. Toggle add body mode." << std::endl;
                if (uiState.editMode == EditMode::ADD_BODY) {
                    std::cout << "Canceling add body mode" << std::endl;
                    cancelAddBody(uiState);
                    uiState.paused = false; // Resume simulation
                } else {
                    std::cout << "Starting add body mode" << std::endl;
                    startAddBodyMode(uiState);
                    uiState.paused = true; // Pause simulation
                }
                break;
                
            case GLFW_KEY_C:
                // 좌표계 변환
                std::cout << "C key pressed. Switching coordinate mode." << std::endl;
                if (uiState.coordMode == CoordinateMode::CARTESIAN) {
                    uiState.coordMode = CoordinateMode::POLAR;
                    std::cout << "Switched to Polar coordinates" << std::endl;
                } else {
                    uiState.coordMode = CoordinateMode::CARTESIAN;
                    std::cout << "Switched to Cartesian coordinates" << std::endl;
                }
                break;
                
            case GLFW_KEY_B:
                // 벽 반발 계수 조정
                {
                    float currentRestitution = getBoundaryRestitution();
                    float newRestitution = currentRestitution + 0.1f;
                    if (newRestitution > 1.0f) {
                        newRestitution = 0.0f;  // 순환: 1.0 다음은 0.0
                    }
                    setBoundaryRestitution(newRestitution);
                    std::cout << "Wall bounce coefficient: " << (int)(newRestitution * 100) << "%" << std::endl;
                }
                break;
                
            case GLFW_KEY_T:
                // 궤도 추적 토글
                uiState.showTrails = !uiState.showTrails;
                if (uiState.showTrails) {
                    std::cout << "Orbit trails enabled" << std::endl;
                } else {
                    std::cout << "Orbit trails disabled" << std::endl;
                    clearTrails(orbitTrails);
                }
                break;
                
            case GLFW_KEY_DELETE:
            case GLFW_KEY_D:
                // 물체 삭제
                if (uiState.editMode == EditMode::DELETE_CONFIRM) {
                    std::cout << "Canceling delete confirmation" << std::endl;
                    cancelDeleteConfirm(uiState);
                } else if (uiState.selectedBody != -1) {
                    std::cout << "D/DELETE key pressed. Selected body: " << uiState.selectedBody << std::endl;
                    startDeleteConfirm(uiState);
                } else {
                    std::cout << "No body selected. Click on a body first to delete." << std::endl;
                }
                break;
            case GLFW_KEY_ENTER:
                // 편집 적용 또는 텍스트 입력 종료
                if (uiState.textInputMode) {
                    std::cout << "Finishing text input: " << uiState.inputBuffer << std::endl;
                    // 값은 이미 즉시 적용되었으므로 텍스트 입력 모드만 종료
                    uiState.textInputMode = false;
                    uiState.inputBuffer = "";
                } else if (uiState.editMode == EditMode::DELETE_CONFIRM) {
                    std::cout << "Confirming body deletion" << std::endl;
                    deleteSelectedBody(bodies, uiState);
                } else if (uiState.editMode == EditMode::ADD_BODY) {
                    std::cout << "Adding new body" << std::endl;
                    applyAddBodyEdit(bodies, uiState);
                    cancelAddBody(uiState);
                    uiState.paused = false; // Resume simulation
                } else if (uiState.selectedBody != -1) {
                    if (uiState.editMode == EditMode::VELOCITY) {
                        std::cout << "Applying velocity changes" << std::endl;
                        applyVelocityEdit(bodies, uiState);
                        uiState.editMode = EditMode::NONE;
                    } else if (uiState.editMode == EditMode::MASS) {
                        std::cout << "Applying mass changes" << std::endl;
                        applyMassEdit(bodies, uiState);
                        uiState.editMode = EditMode::NONE;
                    }
                }
                break;
                
            case GLFW_KEY_BACKSPACE:
                if (uiState.textInputMode) {
                    std::cout << "Backspace pressed" << std::endl;
                    handleBackspace(uiState);
                }
                break;
                
            // 숫자키와 소수점, 마이너스 처리
            case GLFW_KEY_0:
                // 0키 특별 처리: 편집 모드가 아닐 때 줌 리셋
                if (uiState.editMode == EditMode::NONE && !uiState.textInputMode) {
                    resetZoom(uiState);
                    applyZoomToProjection(uiState, g_windowWidth, g_windowHeight);
                    break;
                }
                // 편집 모드일 때는 일반 숫자 입력으로 처리
                [[fallthrough]];
            case GLFW_KEY_1: case GLFW_KEY_2: case GLFW_KEY_3: case GLFW_KEY_4:
            case GLFW_KEY_5: case GLFW_KEY_6: case GLFW_KEY_7: case GLFW_KEY_8: case GLFW_KEY_9:
                if (uiState.textInputMode) {
                    char digit = '0' + (key - GLFW_KEY_0);
                    handleTextInput(uiState, digit);
                    std::cout << "Input: " << uiState.inputBuffer << std::endl;
                } else if (uiState.editMode == EditMode::ADD_BODY) {
                    // 물체 추가 모드에서 숫자키 입력
                    char digit = '0' + (key - GLFW_KEY_0);
                    startTextInput(uiState, uiState.currentEditField);
                    handleTextInput(uiState, digit);
                    std::cout << "Add body - Started text input with: " << digit << std::endl;
                } else if (uiState.editMode != EditMode::NONE && uiState.selectedBody != -1) {
                    // 숫자키를 누르면 해당 필드의 텍스트 입력 모드 시작
                    char digit = '0' + (key - GLFW_KEY_0);
                    if (uiState.editMode == EditMode::MASS) {
                        startTextInput(uiState, 2); // 질량 필드
                    } else if (uiState.editMode == EditMode::VELOCITY) {
                        startTextInput(uiState, uiState.currentEditField);
                    }
                    handleTextInput(uiState, digit);
                    std::cout << "Started text input with: " << digit << std::endl;
                }
                break;
                
            case GLFW_KEY_PERIOD: // 소수점
                if (uiState.textInputMode) {
                    handleTextInput(uiState, '.');
                    std::cout << "Input: " << uiState.inputBuffer << std::endl;
                }
                break;
                
            case GLFW_KEY_TAB:
                // 필드 전환
                if (uiState.editMode == EditMode::ADD_BODY && !uiState.textInputMode) {
                    uiState.currentEditField = (uiState.currentEditField + 1) % 3; // 3 fields: vx/vy/mass or vr/vtheta/mass
                    std::string fieldName;
                    if (uiState.currentEditField == 2) {
                        fieldName = "Mass";
                    } else if (uiState.coordMode == CoordinateMode::CARTESIAN) {
                        fieldName = (uiState.currentEditField == 0) ? "Velocity X" : "Velocity Y";
                    } else {
                        fieldName = (uiState.currentEditField == 0) ? "Velocity R" : "Velocity θ";
                    }
                    std::cout << "TAB - switched to " << fieldName << " field" << std::endl;
                } else if (uiState.editMode == EditMode::VELOCITY && !uiState.textInputMode) {
                    uiState.currentEditField = (uiState.currentEditField + 1) % 2;
                    std::string fieldName;
                    if (uiState.coordMode == CoordinateMode::CARTESIAN) {
                        fieldName = (uiState.currentEditField == 0) ? "X" : "Y";
                    } else {
                        fieldName = (uiState.currentEditField == 0) ? "R" : "th";
                    }
                    std::cout << "TAB - switched to " << fieldName << " field" << std::endl;
                }
                break;
                
            case GLFW_KEY_EQUAL:  // '+' key for zoom in
            case GLFW_KEY_KP_ADD:
                if (uiState.editMode == EditMode::NONE) {
                    updateZoom(uiState, 0.2f);
                    applyZoomToProjection(uiState, g_windowWidth, g_windowHeight);
                }
                break;
                
            case GLFW_KEY_MINUS:  // '-' key for zoom out
            case GLFW_KEY_KP_SUBTRACT:
                if (uiState.editMode == EditMode::NONE && !uiState.textInputMode) {
                    updateZoom(uiState, -0.2f);
                    applyZoomToProjection(uiState, g_windowWidth, g_windowHeight);
                } else if (uiState.textInputMode) {
                    handleTextInput(uiState, '-');
                    std::cout << "Input: " << uiState.inputBuffer << std::endl;
                } else if (uiState.editMode != EditMode::NONE && uiState.selectedBody != -1) {
                    // 마이너스키를 누르면 텍스트 입력 시작 (기존 기능 유지)
                    if (uiState.editMode == EditMode::MASS) {
                        startTextInput(uiState, 2);
                    } else if (uiState.editMode == EditMode::VELOCITY) {
                        startTextInput(uiState, uiState.currentEditField);
                    }
                    handleTextInput(uiState, '-');
                    std::cout << "Started text input with minus" << std::endl;
                }
                break;
        }
    }
}

int main() {
    if (!glfwInit()) {
        std::cerr << "GLFW 초기화 실패" << std::endl;
        return -1;
    }
    
    GLFWwindow* window = glfwCreateWindow(1200, 800, "Interactive 3-Body Simulation", NULL, NULL);
    if (!window) {
        glfwTerminate();
        return -1;
    }
    
    glfwMakeContextCurrent(window);
    
    if (glewInit() != GLEW_OK) {
        std::cerr << "GLEW 초기화 실패" << std::endl;
        return -1;
    }
    
    glfwSetMouseButtonCallback(window, mouseButtonCallback);
    glfwSetCursorPosCallback(window, cursorPosCallback);
    glfwSetKeyCallback(window, keyCallback);
    glfwSetScrollCallback(window, scrollCallback);
    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
    
    // 초기 창 크기 설정
    glfwGetFramebufferSize(window, &g_windowWidth, &g_windowHeight);
    framebufferSizeCallback(window, g_windowWidth, g_windowHeight);
    
    // 초기 줌 설정 적용
    applyZoomToProjection(uiState, g_windowWidth, g_windowHeight);
    
    // 초기 물체 설정
    bodies = initBodies;
    
    // UI 상태는 이미 기본값으로 초기화됨
    
    // 궤도 추적 초기화
    initializeTrails(orbitTrails, bodies.size());
    
    glClearColor(0.0f, 0.0f, 0.05f, 1.0f);
    
    std::cout << "=== 3-Body Simulation Started ===" << std::endl;
    std::cout << "Controls:" << std::endl;
    std::cout << "  Left Mouse: Click and drag to move bodies" << std::endl;
    std::cout << "  Right Mouse: Drag to pan view" << std::endl;
    std::cout << "  Mouse wheel: Zoom in/out (screen center)" << std::endl;
    std::cout << "  Space: Pause/Resume simulation" << std::endl;
    std::cout << "  V: Edit velocity of selected body" << std::endl;
    std::cout << "  M: Edit mass of selected body" << std::endl;
    std::cout << "  D/Delete: Delete selected body (min 2 required)" << std::endl;
    std::cout << "  P: Add new body" << std::endl;
    std::cout << "  T: Toggle orbit trails" << std::endl;
    std::cout << "  B: Adjust wall bounce (0-100%)" << std::endl;
    std::cout << "  C: Toggle coordinate system (Cartesian/Polar)" << std::endl;
    std::cout << "  H: Toggle crosshair" << std::endl;
    std::cout << "  Numbers: Type to edit values" << std::endl;
    std::cout << "  TAB: Switch field when editing velocity" << std::endl;
    std::cout << "  Mouse drag: Set velocity direction (in velocity mode)" << std::endl;
    std::cout << "  +/-: Zoom in/out (keyboard)" << std::endl;
    std::cout << "  0: Reset zoom to default" << std::endl;
    std::cout << "  Enter: Apply changes" << std::endl;
    std::cout << "  R: Reset to initial positions and zoom" << std::endl;
    std::cout << "  I: Toggle info display" << std::endl;
    std::cout << "  ESC: Exit (or cancel editing)" << std::endl;
    
    while (!glfwWindowShouldClose(window)) {
        // 물리 시뮬레이션 업데이트 (RK4 방법 사용)
        updatePhysicsRK4(bodies, uiState.paused);
        
        // 벽 충돌 처리 (시뮬레이션이 실행 중일 때만)
        if (!uiState.paused) {
            float aspectRatio = (float)g_windowWidth / (float)g_windowHeight;
            handleBoundaryCollisions(bodies, aspectRatio);
        }
        
        // 궤도 추적 업데이트 (시뮬레이션이 실행 중일 때만)
        if (!uiState.paused) {
            updateTrails(orbitTrails, bodies);
        }
        
        glClear(GL_COLOR_BUFFER_BIT);
        
        // 궤도 그리기 (물체보다 먼저 그려서 뒤에 나타나게)
        drawTrails(orbitTrails, bodies, uiState.showTrails);
        
        // 물체 그리기
        for (size_t i = 0; i < bodies.size(); i++) {
            if (i == uiState.selectedBody) {
                glPointSize(15.0f);
                // 선택된 물체 주변에 링 그리기
                glColor3f(1.0f, 1.0f, 0.0f);
                glBegin(GL_LINE_LOOP);
                for (int j = 0; j < 20; j++) {
                    float angle = 2.0f * M_PI * j / 20;
                    glVertex2f(bodies[i].x + 0.05f * cos(angle), 
                              bodies[i].y + 0.05f * sin(angle));
                }
                glEnd();
            } else {
                glPointSize(10.0f);
            }
            
            glBegin(GL_POINTS);
            glColor3f(bodies[i].r, bodies[i].g, bodies[i].b);
            glVertex2f(bodies[i].x, bodies[i].y);
            glEnd();
        }
        
        // 크로스헤어 그리기 (물체들과 UI 사이에)
        drawCrosshair(uiState);
        
        // 선택된 물체의 현재 속도 벡터 표시 (world coordinates)
        drawCurrentVelocityVector(bodies, uiState);
        
        // 물체 추가 모드 미리보기 (world coordinates)
        drawAddBodyPreview(uiState);
        
        // World coordinate editors (affected by zoom/pan)
        if (uiState.editMode == EditMode::VELOCITY) {
            drawVelocityEditor(bodies, uiState);
        } else if (uiState.editMode == EditMode::MASS) {
            drawMassEditor(bodies, uiState);
        } else if (uiState.editMode == EditMode::ADD_BODY) {
            drawAddBodyEditor(uiState);
        } else if (uiState.editMode == EditMode::DELETE_CONFIRM) {
            drawDeleteConfirm(bodies, uiState);
        }
        
        // Screen-fixed UI 그리기 (not affected by zoom/pan)
        drawInfoFixed(bodies, uiState, g_windowWidth, g_windowHeight);
        drawCoordinateModeFixed(uiState.coordMode, g_windowWidth, g_windowHeight);
        
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    
    glfwTerminate();
    return 0;
}