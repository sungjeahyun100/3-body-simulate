#include "ui.h"
#include "font.h"
#include "physics.h"
#include <GL/glew.h>
#include <sstream>
#include <iomanip>
#include <cmath>
#include <iostream>

// Font size constants - adjust these to change text size globally
const float BASE_TEXT_SIZE = 0.025f;
const float TITLE_TEXT_SIZE = BASE_TEXT_SIZE * 1.2f;
const float INFO_TEXT_SIZE = BASE_TEXT_SIZE * 0.6f;
const float STATUS_TEXT_SIZE = BASE_TEXT_SIZE * 1.5f;

// Window size globals
static int g_uiWindowWidth = 1200;
static int g_uiWindowHeight = 800;

//Trail size
static int trail_size = 2500;

// Convert float to string with specified precision
std::string floatToString(float value, int precision) {
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(precision) << value;
    return oss.str();
}

// Convert radians to degrees string
std::string angleToString(float radians) {
    float degrees = radians * 180.0f / M_PI;
    // Normalize to 0-360 range
    while (degrees < 0) degrees += 360;
    while (degrees >= 360) degrees -= 360;
    return floatToString(degrees, 1) + "°";
}

// Convert degrees string to radians
float stringToAngle(const std::string& degrees) {
    float deg = std::stof(degrees);
    return deg * M_PI / 180.0f;
}

// Main information display
void drawInfo(const std::vector<Body>& bodies, const UIState& uiState) {
    if (!uiState.showInfo) return;
    
    // Fixed panel position - always stick to the left edge
    float aspectRatio = getAspectRatio();
    float panelLeft = -aspectRatio + 0.02f;  // Always start from screen left edge + small margin
    float panelWidth = 0.6f;  // Fixed width in normalized coordinates
    float panelRight = panelLeft + panelWidth;
    
    // Semi-transparent background panel
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(0.0f, 0.0f, 0.0f, 0.8f);
    glBegin(GL_QUADS);
    glVertex2f(panelLeft, 0.95f); glVertex2f(panelRight, 0.95f);
    glVertex2f(panelRight, -0.95f); glVertex2f(panelLeft, -0.95f);
    glEnd();
    glDisable(GL_BLEND);
    
    glColor3f(1.0f, 1.0f, 1.0f);
    float textSize = getResponsiveTextSize(BASE_TEXT_SIZE);
    float textLeftMargin = panelLeft + 0.03f;  // Text margin from panel left
    
    // Title
    drawText("3-Body Simulation", textLeftMargin, 0.9f, getResponsiveTextSize(TITLE_TEXT_SIZE));
    
    // Body information
    for (size_t i = 0; i < bodies.size(); i++) {
        float yPos = 0.7f - i * 0.18f;
        
        // Body color for identification
        glColor3f(bodies[i].r, bodies[i].g, bodies[i].b);
        
        // Body number
        std::string bodyNum = "Body " + std::to_string(i + 1);
        drawText(bodyNum, textLeftMargin, yPos, textSize);
        
        glColor3f(0.9f, 0.9f, 0.9f);  // Light gray
        
        // Position info
        std::string posInfo = "Pos: " + floatToString(bodies[i].x, 2) + ", " + floatToString(bodies[i].y, 2);
        drawText(posInfo, textLeftMargin, yPos - 0.035f, getResponsiveTextSize(INFO_TEXT_SIZE));
        
        // Velocity info
        std::string velInfo = "Vel: " + floatToString(bodies[i].vx, 2) + ", " + floatToString(bodies[i].vy, 2);
        drawText(velInfo, textLeftMargin, yPos - 0.065f, getResponsiveTextSize(INFO_TEXT_SIZE));
        
        // Mass and speed
        float speed = sqrt(bodies[i].vx * bodies[i].vx + bodies[i].vy * bodies[i].vy);
        std::string statsInfo = "Mass: " + floatToString(bodies[i].mass, 0) + " Speed: " + floatToString(speed, 3);
        drawText(statsInfo, textLeftMargin, yPos - 0.095f, getResponsiveTextSize(INFO_TEXT_SIZE));
        
        // Separator line
        glColor3f(0.3f, 0.3f, 0.3f);
        glBegin(GL_LINES);
        glVertex2f(textLeftMargin, yPos - 0.13f);
        glVertex2f(panelRight - 0.05f, yPos - 0.13f);
        glEnd();
    }
    
    // Selected body indicator
    if (uiState.selectedBody != -1) {
        glColor3f(1.0f, 1.0f, 0.0f);  // Yellow
        drawText("SELECTED", textLeftMargin + 0.4f, 0.7f - uiState.selectedBody * 0.18f, textSize * 0.6f);
    }
    
    // Zoom info
    glColor3f(0.7f, 0.9f, 0.7f);  // Light green
    std::string zoomInfo = "Zoom: " + floatToString(uiState.zoomLevel, 2) + "x";
    drawText(zoomInfo, textLeftMargin, -0.45f, textSize);
    
    // Controls info
    glColor3f(0.7f, 0.7f, 0.9f);
    drawText("Controls", textLeftMargin, -0.55f, textSize);
    
    glColor3f(0.8f, 0.8f, 0.8f);
    drawText("Mouse: Drag bodies", textLeftMargin, -0.6f, getResponsiveTextSize(INFO_TEXT_SIZE));
    drawText("Wheel: Zoom", textLeftMargin, -0.65f, getResponsiveTextSize(INFO_TEXT_SIZE));
    drawText("Space: Pause", textLeftMargin, -0.7f, getResponsiveTextSize(INFO_TEXT_SIZE));
    drawText("V: Edit velocity", textLeftMargin, -0.75f, getResponsiveTextSize(INFO_TEXT_SIZE));
    drawText("M: Edit mass", textLeftMargin, -0.8f, getResponsiveTextSize(INFO_TEXT_SIZE));
    drawText("T: Toggle trails", textLeftMargin, -0.85f, getResponsiveTextSize(INFO_TEXT_SIZE));
    drawText("C: Switch coords", textLeftMargin, -0.9f, getResponsiveTextSize(INFO_TEXT_SIZE));
    drawText("R: Reset", textLeftMargin, -0.95f, getResponsiveTextSize(INFO_TEXT_SIZE));
    
    // Status display - 화면 오른쪽 위 위치 조정
    if (uiState.paused) {
        glColor3f(1.0f, 0.2f, 0.2f);  // Red
        float statusX = (aspectRatio > 1.5f) ? 0.7f : 0.4f; // 종횡비에 따라 위치 조정
        drawText("PAUSED", statusX, 0.95f, getResponsiveTextSize(STATUS_TEXT_SIZE));
    }
}

// Draw direction arrow during dragging
void drawDirectionArrow(const UIState& uiState) {
    if (!uiState.draggingDirection) return;
    
    float startX = uiState.dragStartX;
    float startY = uiState.dragStartY;
    float endX = uiState.dragCurrentX;
    float endY = uiState.dragCurrentY;
    
    // Calculate arrow vector
    float dx = endX - startX;
    float dy = endY - startY;
    float length = sqrt(dx * dx + dy * dy);
    
    if (length < 0.01f) return; // Too small to draw
    
    // Normalize direction
    float dirX = dx / length;
    float dirY = dy / length;
    
    // Arrow head size
    float headSize = 0.03f;
    float headAngle = 0.5f; // radians
    
    // Arrow head points
    float head1X = endX - headSize * (dirX * cos(headAngle) - dirY * sin(headAngle));
    float head1Y = endY - headSize * (dirX * sin(headAngle) + dirY * cos(headAngle));
    float head2X = endX - headSize * (dirX * cos(-headAngle) - dirY * sin(-headAngle));
    float head2Y = endY - headSize * (dirX * sin(-headAngle) + dirY * cos(-headAngle));
    
    // Draw arrow shaft
    glColor3f(1.0f, 1.0f, 0.0f); // Yellow arrow
    glLineWidth(3.0f);
    glBegin(GL_LINES);
    glVertex2f(startX, startY);
    glVertex2f(endX, endY);
    glEnd();
    
    // Draw arrow head
    glBegin(GL_TRIANGLES);
    glVertex2f(endX, endY);
    glVertex2f(head1X, head1Y);
    glVertex2f(head2X, head2Y);
    glEnd();
    
    // Draw starting point circle
    glColor3f(0.0f, 1.0f, 0.0f); // Green start point
    glBegin(GL_TRIANGLE_FAN);
    float circleRadius = 0.01f;
    glVertex2f(startX, startY);
    for (int i = 0; i <= 16; i++) {
        float angle = 2.0f * M_PI * i / 16;
        glVertex2f(startX + circleRadius * cos(angle), startY + circleRadius * sin(angle));
    }
    glEnd();
    
    glLineWidth(1.0f); // Reset line width
}

// Draw current velocity vector for selected body
void drawCurrentVelocityVector(const std::vector<Body>& bodies, const UIState& uiState) {
    if (uiState.selectedBody == -1 || uiState.selectedBody >= bodies.size()) return;
    
    const Body& body = bodies[uiState.selectedBody];
    
    // Calculate velocity vector visualization
    float scale = 0.3f; // Scale factor for visibility
    float startX = body.x;
    float startY = body.y;
    float endX = startX + body.vx * scale;
    float endY = startY + body.vy * scale;
    
    float dx = endX - startX;
    float dy = endY - startY;
    float length = sqrt(dx * dx + dy * dy);
    
    if (length < 0.005f) return; // Too small to draw
    
    // Normalize direction
    float dirX = dx / length;
    float dirY = dy / length;
    
    // Arrow head size
    float headSize = 0.02f;
    float headAngle = 0.5f;
    
    // Arrow head points
    float head1X = endX - headSize * (dirX * cos(headAngle) - dirY * sin(headAngle));
    float head1Y = endY - headSize * (dirX * sin(headAngle) + dirY * cos(headAngle));
    float head2X = endX - headSize * (dirX * cos(-headAngle) - dirY * sin(-headAngle));
    float head2Y = endY - headSize * (dirX * sin(-headAngle) + dirY * cos(-headAngle));
    
    // Draw velocity vector arrow - highlight if being dragged
    if (uiState.draggingVelocityVector) {
        glColor3f(1.0f, 1.0f, 0.0f); // Yellow when dragging
        glLineWidth(4.0f);
    } else {
        glColor3f(0.0f, 1.0f, 1.0f); // Cyan for current velocity
        glLineWidth(3.0f);
    }
    
    glBegin(GL_LINES);
    glVertex2f(startX, startY);
    glVertex2f(endX, endY);
    glEnd();
    
    // Draw arrow head
    glBegin(GL_TRIANGLES);
    glVertex2f(endX, endY);
    glVertex2f(head1X, head1Y);
    glVertex2f(head2X, head2Y);
    glEnd();
    
    // Draw interaction handle at the end
    glColor4f(1.0f, 1.0f, 1.0f, 0.8f);
    glBegin(GL_TRIANGLE_FAN);
    float handleRadius = 0.015f;
    glVertex2f(endX, endY);
    for (int i = 0; i <= 16; i++) {
        float angle = 2.0f * M_PI * i / 16;
        glVertex2f(endX + handleRadius * cos(angle), endY + handleRadius * sin(angle));
    }
    glEnd();
    
    glLineWidth(1.0f); // Reset line width
}

// Velocity editor interface
void drawVelocityEditor(const std::vector<Body>& bodies, UIState& uiState) {
    if (uiState.editMode != EditMode::VELOCITY || uiState.selectedBody == -1) return;
    
    // Responsive panel positioning
    float aspectRatio = getAspectRatio();
    float panelLeft = (aspectRatio > 1.5f) ? 0.15f : 0.1f;
    float panelRight = (aspectRatio > 1.5f) ? 0.85f : 0.9f;
    float panelTop = 0.5f;
    float panelBottom = 0.05f;
    
    // Editor background
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(0.1f, 0.1f, 0.3f, 0.9f);
    glBegin(GL_QUADS);
    glVertex2f(panelLeft, panelTop); glVertex2f(panelRight, panelTop);
    glVertex2f(panelRight, panelBottom); glVertex2f(panelLeft, panelBottom);
    glEnd();
    glDisable(GL_BLEND);
    
    // Border
    glColor3f(1.0f, 1.0f, 1.0f);
    glBegin(GL_LINE_LOOP);
    glVertex2f(panelLeft, panelTop); glVertex2f(panelRight, panelTop);
    glVertex2f(panelRight, panelBottom); glVertex2f(panelLeft, panelBottom);
    glEnd();
    
    glColor3f(1.0f, 1.0f, 1.0f);
    float textSize = getResponsiveTextSizeSmall(BASE_TEXT_SIZE);
    float textLeftMargin = panelLeft + 0.05f;
    
    // Title
    drawText("Velocity Editor - Body " + std::to_string(uiState.selectedBody + 1), textLeftMargin, 0.45f, textSize);
    
    const Body& body = bodies[uiState.selectedBody];
    
    if (uiState.coordMode == CoordinateMode::CARTESIAN) {
        // Cartesian mode
        glColor3f(0.8f, 0.8f, 0.8f);
        drawText("Current: X=" + floatToString(body.vx, 3) + " Y=" + floatToString(body.vy, 3), 
                 textLeftMargin, 0.38f, getResponsiveTextSizeSmall(textSize * 0.8f));
        
        // X field - highlight if selected or in text input
        bool xSelected = (uiState.currentEditField == 0);
        bool xInputting = (uiState.textInputMode && uiState.inputFieldIndex == 0);
        if (xSelected || xInputting) {
            glColor3f(1.0f, 1.0f, 0.0f); // Yellow for selected
        } else {
            glColor3f(0.9f, 0.9f, 0.9f); // White for unselected
        }
        drawText("Edit X:", textLeftMargin, 0.30f, getResponsiveTextSizeSmall(textSize * 0.9f));
        
        // Show input buffer if typing, otherwise show current value
        std::string xValue = (xInputting) ? uiState.inputBuffer + "|" : floatToString(uiState.editVelocityX, 3);
        drawText(xValue, textLeftMargin + 0.27f, 0.30f, getResponsiveTextSizeSmall(textSize * 0.9f));
        
        // Y field - highlight if selected or in text input
        bool ySelected = (uiState.currentEditField == 1);
        bool yInputting = (uiState.textInputMode && uiState.inputFieldIndex == 1);
        if (ySelected || yInputting) {
            glColor3f(1.0f, 1.0f, 0.0f); // Yellow for selected
        } else {
            glColor3f(0.9f, 0.9f, 0.9f); // White for unselected
        }
        drawText("Edit Y:", textLeftMargin, 0.25f, getResponsiveTextSizeSmall(textSize * 0.9f));
        
        std::string yValue = (yInputting) ? uiState.inputBuffer + "|" : floatToString(uiState.editVelocityY, 3);
        drawText(yValue, textLeftMargin + 0.27f, 0.25f, getResponsiveTextSizeSmall(textSize * 0.9f));
    } else {
        // Polar mode
        CartesianCoord vel = {body.vx, body.vy};
        PolarCoord current = cartesianToPolar(vel);
        
        glColor3f(0.8f, 0.8f, 0.8f);
        drawText("Current: R=" + floatToString(current.r, 3) + " th=" + angleToString(current.theta), 
                 textLeftMargin, 0.38f, getResponsiveTextSize(textSize * 0.8f));
        
        // R field - highlight if selected or in text input
        bool rSelected = (uiState.currentEditField == 0);
        bool rInputting = (uiState.textInputMode && uiState.inputFieldIndex == 0);
        if (rSelected || rInputting) {
            glColor3f(1.0f, 1.0f, 0.0f); // Yellow for selected
        } else {
            glColor3f(0.9f, 0.9f, 0.9f); // White for unselected
        }
        drawText("Edit R:", textLeftMargin, 0.30f, getResponsiveTextSize(textSize * 0.9f));
        
        std::string rValue = (rInputting) ? uiState.inputBuffer + "|" : floatToString(uiState.editVelocityR, 3);
        drawText(rValue, textLeftMargin + 0.27f, 0.30f, getResponsiveTextSize(textSize * 0.9f));
        
        // θ field - highlight if selected or in text input
        bool thetaSelected = (uiState.currentEditField == 1);
        bool thetaInputting = (uiState.textInputMode && uiState.inputFieldIndex == 1);
        if (thetaSelected || thetaInputting) {
            glColor3f(1.0f, 1.0f, 0.0f); // Yellow for selected
        } else {
            glColor3f(0.9f, 0.9f, 0.9f); // White for unselected
        }
        drawText("Edit th:", textLeftMargin, 0.25f, getResponsiveTextSize(textSize * 0.9f));
        
        std::string thetaValue = (thetaInputting) ? uiState.inputBuffer + "|°" : floatToString(uiState.editVelocityTheta, 1) + "°";
        drawText(thetaValue, textLeftMargin + 0.27f, 0.25f, getResponsiveTextSize(textSize * 0.9f));
    }
    
    // Instructions
    glColor3f(0.7f, 0.7f, 0.9f);
    drawText("Keys: Type numbers, TAB to switch field", textLeftMargin, 0.15f, getResponsiveTextSize(textSize * 0.7f));
    drawText("Mouse: Drag to set direction", textLeftMargin, 0.11f, getResponsiveTextSize(textSize * 0.7f));
    drawText("Enter: Apply, Esc: Cancel", textLeftMargin, 0.07f, getResponsiveTextSize(textSize * 0.7f));
    
    // Draw direction arrow if dragging
    drawDirectionArrow(uiState);
}

// Mass editor interface
void drawMassEditor(const std::vector<Body>& bodies, UIState& uiState) {
    if (uiState.editMode != EditMode::MASS || uiState.selectedBody == -1) return;
    
    // Responsive panel positioning
    float aspectRatio = getAspectRatio();
    float panelLeft = (aspectRatio > 1.5f) ? 0.15f : 0.1f;
    float panelRight = (aspectRatio > 1.5f) ? 0.85f : 0.9f;
    float panelTop = 0.5f;
    float panelBottom = 0.05f;
    
    // Editor background
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(0.3f, 0.1f, 0.1f, 0.9f); // Reddish background for mass
    glBegin(GL_QUADS);
    glVertex2f(panelLeft, panelTop); glVertex2f(panelRight, panelTop);
    glVertex2f(panelRight, panelBottom); glVertex2f(panelLeft, panelBottom);
    glEnd();
    glDisable(GL_BLEND);
    
    // Border
    glColor3f(1.0f, 1.0f, 1.0f);
    glBegin(GL_LINE_LOOP);
    glVertex2f(panelLeft, panelTop); glVertex2f(panelRight, panelTop);
    glVertex2f(panelRight, panelBottom); glVertex2f(panelLeft, panelBottom);
    glEnd();
    
    glColor3f(1.0f, 1.0f, 1.0f);
    float textSize = getResponsiveTextSizeSmall(BASE_TEXT_SIZE);
    float textLeftMargin = panelLeft + 0.05f;
    
    // Title
    drawText("Mass Editor - Body " + std::to_string(uiState.selectedBody + 1), textLeftMargin, 0.45f, textSize);
    
    const Body& body = bodies[uiState.selectedBody];
    
    // Current mass
    glColor3f(0.8f, 0.8f, 0.8f);
    drawText("Current Mass: " + floatToString(body.mass, 0) + " kg", textLeftMargin, 0.38f, getResponsiveTextSizeSmall(textSize * 0.8f));
    
    // Edit mass
    bool massInputting = (uiState.textInputMode && uiState.inputFieldIndex == 2);
    glColor3f(1.0f, 1.0f, 0.0f); // Always yellow since there's only one field
    drawText("Edit Mass:", textLeftMargin, 0.30f, getResponsiveTextSizeSmall(textSize * 0.9f));
    
    // Show input buffer if typing, otherwise show current value
    std::string massValue = (massInputting) ? uiState.inputBuffer + "| kg" : floatToString(uiState.editMass, 0) + " kg";
    drawText(massValue, textLeftMargin + 0.30f, 0.30f, getResponsiveTextSizeSmall(textSize * 0.9f));
    
    // Instructions
    glColor3f(0.9f, 0.7f, 0.7f);
    drawText("Keys: Type numbers to edit mass", textLeftMargin, 0.20f, getResponsiveTextSizeSmall(textSize * 0.7f));
    drawText("Enter: Apply, Esc: Cancel", textLeftMargin, 0.16f, getResponsiveTextSizeSmall(textSize * 0.7f));
}

// Draw coordinate system mode indicator
void drawCoordinateMode(CoordinateMode mode) {
    glColor3f(0.8f, 0.9f, 1.0f);
    float textSize = getResponsiveTextSize(BASE_TEXT_SIZE);
    
    // 화면 크기에 따라 위치 조정
    float aspectRatio = getAspectRatio();
    float coordX = (aspectRatio > 1.5f) ? 0.7f : 0.4f;
    
    std::string modeText = (mode == CoordinateMode::CARTESIAN) ? "Cartesian [X,Y]" : "Polar [R,th]";
    drawText("Coord: " + modeText, coordX, 0.8f, textSize);
    
    glColor3f(0.6f, 0.6f, 0.8f);
    drawText("Press [C] to switch", coordX, 0.76f, getResponsiveTextSize(textSize * 0.8f));
}

// Handle velocity editing initialization
void handleVelocityEdit(std::vector<Body>& bodies, UIState& uiState) {
    if (uiState.selectedBody == -1 || uiState.selectedBody >= bodies.size()) return;
    
    Body& body = bodies[uiState.selectedBody];
    
    // Set current values based on coordinate mode
    if (uiState.coordMode == CoordinateMode::CARTESIAN) {
        uiState.editVelocityX = body.vx;
        uiState.editVelocityY = body.vy;
    } else {
        // Convert to polar
        CartesianCoord vel = {body.vx, body.vy};
        PolarCoord polar = cartesianToPolar(vel);
        uiState.editVelocityR = polar.r;
        uiState.editVelocityTheta = polar.theta * 180.0f / M_PI; // Convert to degrees
    }
}

// Handle mass editing initialization
void handleMassEdit(std::vector<Body>& bodies, UIState& uiState) {
    if (uiState.selectedBody == -1 || uiState.selectedBody >= bodies.size()) return;
    
    Body& body = bodies[uiState.selectedBody];
    uiState.editMass = body.mass;
}

// Apply velocity edit to selected body
void applyVelocityEdit(std::vector<Body>& bodies, const UIState& uiState) {
    if (uiState.editMode != EditMode::VELOCITY || uiState.selectedBody == -1) return;
    
    Body& body = bodies[uiState.selectedBody];
    
    if (uiState.coordMode == CoordinateMode::CARTESIAN) {
        body.vx = uiState.editVelocityX;
        body.vy = uiState.editVelocityY;
    } else {
        PolarCoord polar = {uiState.editVelocityR, uiState.editVelocityTheta * M_PI / 180.0f};
        CartesianCoord vel = polarToCartesian(polar);
        body.vx = vel.x;
        body.vy = vel.y;
    }
}

// Apply mass edit to selected body
void applyMassEdit(std::vector<Body>& bodies, const UIState& uiState) {
    if (uiState.editMode != EditMode::MASS || uiState.selectedBody == -1) return;
    
    Body& body = bodies[uiState.selectedBody];
    body.mass = uiState.editMass;
}

// Cancel any editing
void cancelEdit(UIState& uiState) {
    if (uiState.editMode == EditMode::DELETE_CONFIRM) {
        cancelDeleteConfirm(uiState);
        return;
    }
    
    uiState.editMode = EditMode::NONE;
    uiState.editVelocityX = 0.0f;
    uiState.editVelocityY = 0.0f;
    uiState.editVelocityR = 0.0f;
    uiState.editVelocityTheta = 0.0f;
    uiState.editMass = 1000.0f;
    uiState.currentEditField = 0;
    uiState.textInputMode = false;
    uiState.inputBuffer = "";
    uiState.draggingDirection = false;
}

// Text input handling
void startTextInput(UIState& uiState, int fieldIndex) {
    uiState.textInputMode = true;
    uiState.inputFieldIndex = fieldIndex;
    uiState.inputBuffer = "";
    
    // Pre-fill with current value
    if (fieldIndex == 2) { // Mass
        uiState.inputBuffer = floatToString(uiState.editMass, 0);
    } else if (uiState.editMode == EditMode::VELOCITY) {
        if (uiState.coordMode == CoordinateMode::CARTESIAN) {
            if (fieldIndex == 0) {
                uiState.inputBuffer = floatToString(uiState.editVelocityX, 3);
            } else {
                uiState.inputBuffer = floatToString(uiState.editVelocityY, 3);
            }
        } else {
            if (fieldIndex == 0) {
                uiState.inputBuffer = floatToString(uiState.editVelocityR, 3);
            } else {
                uiState.inputBuffer = floatToString(uiState.editVelocityTheta, 1);
            }
        }
    }
}

void handleTextInput(UIState& uiState, char character) {
    if (!uiState.textInputMode) return;
    
    // 입력 제한 검사
    if (character == '.' && uiState.inputBuffer.find('.') != std::string::npos) {
        return; // 이미 소수점이 있음
    }
    if (character == '-' && !uiState.inputBuffer.empty()) {
        return; // 마이너스는 맨 처음에만
    }
    
    uiState.inputBuffer += character;
    
    // 즉시 값 적용 - 유효한 숫자인지 확인 후 적용
    applyTextInputImmediate(uiState);
}

void handleBackspace(UIState& uiState) {
    if (!uiState.textInputMode || uiState.inputBuffer.empty()) return;
    uiState.inputBuffer.pop_back();
    
    // 백스페이스 후에도 즉시 값 적용
    applyTextInputImmediate(uiState);
}

void applyTextInput(UIState& uiState) {
    if (!uiState.textInputMode || uiState.inputBuffer.empty()) return;
    
    try {
        float value = std::stof(uiState.inputBuffer);
        
        if (uiState.inputFieldIndex == 2) { // Mass
            uiState.editMass = std::max(1.0f, value);
        } else if (uiState.editMode == EditMode::VELOCITY) {
            if (uiState.coordMode == CoordinateMode::CARTESIAN) {
                if (uiState.inputFieldIndex == 0) {
                    uiState.editVelocityX = value;
                } else {
                    uiState.editVelocityY = value;
                }
            } else {
                if (uiState.inputFieldIndex == 0) {
                    uiState.editVelocityR = std::max(0.0f, value);
                } else {
                    uiState.editVelocityTheta = value;
                    // 각도를 0-360 범위로 정규화
                    while (uiState.editVelocityTheta < 0) uiState.editVelocityTheta += 360;
                    while (uiState.editVelocityTheta >= 360) uiState.editVelocityTheta -= 360;
                }
            }
        }
    } catch (const std::exception& e) {
        // 잘못된 입력은 무시
    }
    
    uiState.textInputMode = false;
    uiState.inputBuffer = "";
}

void applyTextInputImmediate(UIState& uiState) {
    if (!uiState.textInputMode || uiState.inputBuffer.empty()) return;
    
    try {
        float value = std::stof(uiState.inputBuffer);
        
        if (uiState.editMode == EditMode::ADD_BODY) {
            if (uiState.inputFieldIndex == 2) { // Mass
                uiState.newBodyMass = std::max(1.0f, value);
            } else {
                if (uiState.coordMode == CoordinateMode::CARTESIAN) {
                    if (uiState.inputFieldIndex == 0) {
                        uiState.newBodyVx = value;
                    } else {
                        uiState.newBodyVy = value;
                    }
                } else {
                    // Polar mode for new body
                    if (uiState.inputFieldIndex == 0) {
                        float theta = atan2(uiState.newBodyVy, uiState.newBodyVx);
                        uiState.newBodyVx = value * cos(theta);
                        uiState.newBodyVy = value * sin(theta);
                    } else {
                        float r = sqrt(uiState.newBodyVx * uiState.newBodyVx + uiState.newBodyVy * uiState.newBodyVy);
                        float angle = value * M_PI / 180.0f; // Convert degrees to radians
                        uiState.newBodyVx = r * cos(angle);
                        uiState.newBodyVy = r * sin(angle);
                    }
                }
            }
        } else if (uiState.inputFieldIndex == 2) { // Mass for existing body
            uiState.editMass = std::max(1.0f, value);
        } else if (uiState.editMode == EditMode::VELOCITY) {
            if (uiState.coordMode == CoordinateMode::CARTESIAN) {
                if (uiState.inputFieldIndex == 0) {
                    uiState.editVelocityX = value;
                } else {
                    uiState.editVelocityY = value;
                }
            } else {
                if (uiState.inputFieldIndex == 0) {
                    uiState.editVelocityR = std::max(0.0f, value);
                } else {
                    uiState.editVelocityTheta = value;
                    // 각도를 0-360 범위로 정규화
                    while (uiState.editVelocityTheta < 0) uiState.editVelocityTheta += 360;
                    while (uiState.editVelocityTheta >= 360) uiState.editVelocityTheta -= 360;
                }
            }
        }
    } catch (const std::exception& e) {
        // 잘못된 입력은 무시하고 계속 입력 모드 유지
    }
    
    // 텍스트 입력 모드는 유지 (uiState.textInputMode = true 그대로)
}

void cancelTextInput(UIState& uiState) {
    uiState.textInputMode = false;
    uiState.inputBuffer = "";
}

// Direction dragging for velocity
void startDirectionDrag(UIState& uiState, float x, float y) {
    uiState.draggingDirection = true;
    uiState.dragStartX = x;
    uiState.dragStartY = y;
    uiState.dragCurrentX = x;
    uiState.dragCurrentY = y;
}

void updateDirectionDrag(UIState& uiState, float x, float y) {
    if (!uiState.draggingDirection) return;
    uiState.dragCurrentX = x;
    uiState.dragCurrentY = y;
}

void applyDirectionDrag(UIState& uiState) {
    if (!uiState.draggingDirection) return;
    
    float dx = uiState.dragCurrentX - uiState.dragStartX;
    float dy = uiState.dragCurrentY - uiState.dragStartY;
    
    if (uiState.coordMode == CoordinateMode::CARTESIAN) {
        // 드래그 벡터를 속도로 사용 (스케일링)
        float scale = 2.0f; // 드래그 감도 조정
        uiState.editVelocityX = dx * scale;
        uiState.editVelocityY = dy * scale;
    } else {
        // 극좌표 모드: 거리는 크기, 각도는 방향
        float magnitude = sqrt(dx * dx + dy * dy) * 2.0f; // 스케일링
        float angle = atan2(dy, dx) * 180.0f / M_PI; // 라디안을 도로 변환
        
        // 각도를 0-360 범위로 정규화
        while (angle < 0) angle += 360;
        while (angle >= 360) angle -= 360;
        
        uiState.editVelocityR = magnitude;
        uiState.editVelocityTheta = angle;
    }
    
    uiState.draggingDirection = false;
}

// Direct velocity vector manipulation functions
bool isPointOnVelocityVector(const std::vector<Body>& bodies, const UIState& uiState, float x, float y) {
    if (uiState.selectedBody == -1 || uiState.selectedBody >= bodies.size()) return false;
    
    const Body& body = bodies[uiState.selectedBody];
    
    // Calculate velocity vector visualization
    float scale = uiState.velocityVectorScale;
    float startX = body.x;
    float startY = body.y;
    float endX = startX + body.vx * scale;
    float endY = startY + body.vy * scale;
    
    float dx = endX - startX;
    float dy = endY - startY;
    float length = sqrt(dx * dx + dy * dy);
    
    if (length < 0.005f) return false; // Too small to interact with
    
    // Check if point is near the velocity vector line or arrow head
    float tolerance = 0.03f; // Click tolerance
    
    // Distance from point to line segment
    float A = y - startY;
    float B = startX - x;
    float C = (x - startX) * (endY - startY) - (y - startY) * (endX - startX);
    
    float distance = abs(C) / length;
    
    // Check if the point is within the line segment bounds
    float dotProduct = (x - startX) * (endX - startX) + (y - startY) * (endY - startY);
    float squaredLength = length * length;
    
    if (dotProduct >= 0 && dotProduct <= squaredLength) {
        return distance <= tolerance;
    }
    
    // Also check if clicking near the arrow head
    float headDistance = sqrt((x - endX) * (x - endX) + (y - endY) * (y - endY));
    return headDistance <= tolerance;
}

void startVelocityVectorDrag(UIState& uiState, float x, float y) {
    uiState.draggingVelocityVector = true;
    uiState.dragStartX = x;
    uiState.dragStartY = y;
}

void updateVelocityVectorDrag(std::vector<Body>& bodies, UIState& uiState, float x, float y) {
    if (!uiState.draggingVelocityVector || uiState.selectedBody == -1) return;
    
    Body& body = bodies[uiState.selectedBody];
    
    // Calculate new velocity based on drag position relative to body
    float dx = x - body.x;
    float dy = y - body.y;
    
    // Apply inverse scale to convert screen space back to velocity space
    float scale = uiState.velocityVectorScale;
    body.vx = dx / scale;
    body.vy = dy / scale;
}

void finishVelocityVectorDrag(UIState& uiState) {
    uiState.draggingVelocityVector = false;
}

// Add body functions
void drawAddBodyEditor(UIState& uiState) {
    if (uiState.editMode != EditMode::ADD_BODY) return;
    
    // Responsive panel positioning
    float aspectRatio = getAspectRatio();
    float panelLeft = (aspectRatio > 1.5f) ? 0.05f : 0.05f;
    float panelRight = (aspectRatio > 1.5f) ? 0.95f : 0.95f;
    float panelTop = 0.6f;
    float panelBottom = 0.05f;
    
    // Editor background
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(0.1f, 0.3f, 0.1f, 0.9f); // Greenish background for add body
    glBegin(GL_QUADS);
    glVertex2f(panelLeft, panelTop); glVertex2f(panelRight, panelTop);
    glVertex2f(panelRight, panelBottom); glVertex2f(panelLeft, panelBottom);
    glEnd();
    glDisable(GL_BLEND);
    
    // Border
    glColor3f(1.0f, 1.0f, 1.0f);
    glBegin(GL_LINE_LOOP);
    glVertex2f(panelLeft, panelTop); glVertex2f(panelRight, panelTop);
    glVertex2f(panelRight, panelBottom); glVertex2f(panelLeft, panelBottom);
    glEnd();
    
    glColor3f(1.0f, 1.0f, 1.0f);
    float textSize = getResponsiveTextSize(BASE_TEXT_SIZE);
    float textLeftMargin = panelLeft + 0.05f;
    
    // Title
    drawText("Add New Body", textLeftMargin, 0.55f, textSize);
    
    // Position info
    glColor3f(0.8f, 0.8f, 0.8f);
    drawText("Position: " + floatToString(uiState.newBodyX, 3) + ", " + floatToString(uiState.newBodyY, 3), 
             textLeftMargin, 0.48f, getResponsiveTextSize(textSize * 0.8f));
    
    if (uiState.coordMode == CoordinateMode::CARTESIAN) {
        // Cartesian velocity input
        
        // Vx field
        bool vxInputting = (uiState.textInputMode && uiState.inputFieldIndex == 0);
        if (vxInputting) {
            glColor3f(1.0f, 1.0f, 0.0f); // Yellow for selected
        } else {
            glColor3f(0.9f, 0.9f, 0.9f); // White for unselected
        }
        drawText("Velocity X:", textLeftMargin, 0.40f, getResponsiveTextSize(textSize * 0.9f));
        
        std::string vxValue = (vxInputting) ? uiState.inputBuffer + "|" : floatToString(uiState.newBodyVx, 3);
        drawText(vxValue, textLeftMargin + 0.37f, 0.40f, getResponsiveTextSize(textSize * 0.9f));
        
        // Vy field
        bool vyInputting = (uiState.textInputMode && uiState.inputFieldIndex == 1);
        if (vyInputting) {
            glColor3f(1.0f, 1.0f, 0.0f); // Yellow for selected
        } else {
            glColor3f(0.9f, 0.9f, 0.9f); // White for unselected
        }
        drawText("Velocity Y:", 0.15f, 0.35f, textSize * 0.9f);
        
        std::string vyValue = (vyInputting) ? uiState.inputBuffer + "|" : floatToString(uiState.newBodyVy, 3);
        drawText(vyValue, 0.52f, 0.35f, textSize * 0.9f);
    } else {
        // Polar velocity input
        float r = sqrt(uiState.newBodyVx * uiState.newBodyVx + uiState.newBodyVy * uiState.newBodyVy);
        float theta = atan2(uiState.newBodyVy, uiState.newBodyVx) * 180.0f / M_PI;
        while (theta < 0) theta += 360;
        
        // R field
        bool rInputting = (uiState.textInputMode && uiState.inputFieldIndex == 0);
        if (rInputting) {
            glColor3f(1.0f, 1.0f, 0.0f);
        } else {
            glColor3f(0.9f, 0.9f, 0.9f);
        }
        drawText("Velocity R:", 0.15f, 0.40f, textSize * 0.9f);
        
        std::string rValue = (rInputting) ? uiState.inputBuffer + "|" : floatToString(r, 3);
        drawText(rValue, 0.52f, 0.40f, textSize * 0.9f);
        
        // Theta field
        bool thetaInputting = (uiState.textInputMode && uiState.inputFieldIndex == 1);
        if (thetaInputting) {
            glColor3f(1.0f, 1.0f, 0.0f);
        } else {
            glColor3f(0.9f, 0.9f, 0.9f);
        }
        drawText("Velocity θ:", 0.15f, 0.35f, textSize * 0.9f);
        
        std::string thetaValue = (thetaInputting) ? uiState.inputBuffer + "|°" : floatToString(theta, 1) + "°";
        drawText(thetaValue, 0.52f, 0.35f, textSize * 0.9f);
    }
    
    // Mass field
    bool massInputting = (uiState.textInputMode && uiState.inputFieldIndex == 2);
    if (massInputting) {
        glColor3f(1.0f, 1.0f, 0.0f);
    } else {
        glColor3f(0.9f, 0.9f, 0.9f);
    }
    drawText("Mass:", 0.15f, 0.30f, textSize * 0.9f);
    
    std::string massValue = (massInputting) ? uiState.inputBuffer + "| kg" : floatToString(uiState.newBodyMass, 0) + " kg";
    drawText(massValue, 0.52f, 0.30f, textSize * 0.9f);
    
    // Instructions
    glColor3f(0.7f, 0.9f, 0.7f);
    drawText("Keys: Type numbers, TAB to switch field", 0.15f, 0.20f, textSize * 0.7f);
    drawText("C: Switch coordinate mode", 0.15f, 0.16f, textSize * 0.7f);
    drawText("Enter: Add body, Esc: Cancel", 0.15f, 0.12f, textSize * 0.7f);
}

void drawAddBodyPreview(const UIState& uiState) {
    if (!uiState.addBodyMode) return;
    
    // Draw preview circle at new body position
    glColor4f(0.0f, 1.0f, 0.0f, 0.5f); // Semi-transparent green
    glBegin(GL_TRIANGLE_FAN);
    float radius = 0.03f;
    glVertex2f(uiState.newBodyX, uiState.newBodyY);
    for (int i = 0; i <= 32; i++) {
        float angle = 2.0f * M_PI * i / 32;
        glVertex2f(uiState.newBodyX + radius * cos(angle), 
                   uiState.newBodyY + radius * sin(angle));
    }
    glEnd();
    
    // Draw preview velocity vector
    if (uiState.newBodyVx != 0.0f || uiState.newBodyVy != 0.0f) {
        float scale = 0.3f;
        float endX = uiState.newBodyX + uiState.newBodyVx * scale;
        float endY = uiState.newBodyY + uiState.newBodyVy * scale;
        
        glColor3f(0.0f, 1.0f, 0.0f);
        glLineWidth(2.0f);
        glBegin(GL_LINES);
        glVertex2f(uiState.newBodyX, uiState.newBodyY);
        glVertex2f(endX, endY);
        glEnd();
        
        // Simple arrow head
        float dx = endX - uiState.newBodyX;
        float dy = endY - uiState.newBodyY;
        float length = sqrt(dx * dx + dy * dy);
        if (length > 0.01f) {
            float dirX = dx / length;
            float dirY = dy / length;
            float headSize = 0.015f;
            
            glBegin(GL_TRIANGLES);
            glVertex2f(endX, endY);
            glVertex2f(endX - headSize * dirX + headSize * dirY * 0.5f, 
                       endY - headSize * dirY - headSize * dirX * 0.5f);
            glVertex2f(endX - headSize * dirX - headSize * dirY * 0.5f, 
                       endY - headSize * dirY + headSize * dirX * 0.5f);
            glEnd();
        }
        
        glLineWidth(1.0f);
    }
}

void startAddBodyMode(UIState& uiState) {
    uiState.addBodyMode = true;
    uiState.editMode = EditMode::ADD_BODY;
    uiState.newBodyVx = 0.0f;
    uiState.newBodyVy = 0.0f;
    uiState.newBodyMass = 1000.0f;
    uiState.currentEditField = 0;
    uiState.textInputMode = false;
    uiState.inputBuffer = "";
}

void setNewBodyPosition(UIState& uiState, float x, float y) {
    uiState.newBodyX = x;
    uiState.newBodyY = y;
}

void handleAddBodyEdit(UIState& uiState) {
    // This function handles the add body editing initialization
    // Similar to handleVelocityEdit and handleMassEdit
}

void applyAddBodyEdit(std::vector<Body>& bodies, const UIState& uiState) {
    if (uiState.editMode != EditMode::ADD_BODY) return;
    
    // Create new body with random color
    Body newBody;
    newBody.x = uiState.newBodyX;
    newBody.y = uiState.newBodyY;
    newBody.vx = uiState.newBodyVx;
    newBody.vy = uiState.newBodyVy;
    newBody.mass = uiState.newBodyMass;
    
    // Random color
    newBody.r = 0.3f + (rand() % 7) * 0.1f;
    newBody.g = 0.3f + (rand() % 7) * 0.1f;
    newBody.b = 0.3f + (rand() % 7) * 0.1f;
    
    bodies.push_back(newBody);
}

void cancelAddBody(UIState& uiState) {
    uiState.addBodyMode = false;
    uiState.editMode = EditMode::NONE;
    uiState.textInputMode = false;
    uiState.inputBuffer = "";
    uiState.currentEditField = 0;
}

// Delete body functions
void startDeleteConfirm(UIState& uiState) {
    if (uiState.selectedBody == -1) return;
    
    uiState.editMode = EditMode::DELETE_CONFIRM;
    std::cout << "Delete confirmation started for Body " << uiState.selectedBody + 1 << std::endl;
}

void drawDeleteConfirm(const std::vector<Body>& bodies, const UIState& uiState) {
    if (uiState.editMode != EditMode::DELETE_CONFIRM || uiState.selectedBody == -1) return;
    
    // Responsive panel positioning
    float aspectRatio = getAspectRatio();
    float panelLeft = (aspectRatio > 1.5f) ? 0.2f : 0.15f;
    float panelRight = (aspectRatio > 1.5f) ? 0.8f : 0.85f;
    float panelTop = 0.4f;
    float panelBottom = 0.1f;
    
    // Semi-transparent overlay
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(0.0f, 0.0f, 0.0f, 0.7f);
    glBegin(GL_QUADS);
    glVertex2f(-2.0f, 2.0f); glVertex2f(2.0f, 2.0f);
    glVertex2f(2.0f, -2.0f); glVertex2f(-2.0f, -2.0f);
    glEnd();
    
    // Delete confirmation background (red tint)
    glColor4f(0.4f, 0.1f, 0.1f, 0.95f);
    glBegin(GL_QUADS);
    glVertex2f(panelLeft, panelTop); glVertex2f(panelRight, panelTop);
    glVertex2f(panelRight, panelBottom); glVertex2f(panelLeft, panelBottom);
    glEnd();
    glDisable(GL_BLEND);
    
    // Border
    glColor3f(1.0f, 0.3f, 0.3f);
    glLineWidth(3.0f);
    glBegin(GL_LINE_LOOP);
    glVertex2f(panelLeft, panelTop); glVertex2f(panelRight, panelTop);
    glVertex2f(panelRight, panelBottom); glVertex2f(panelLeft, panelBottom);
    glEnd();
    glLineWidth(1.0f);
    
    float textSize = getResponsiveTextSizeSmall(BASE_TEXT_SIZE);
    float textLeftMargin = panelLeft + 0.05f;
    
    // Warning title
    glColor3f(1.0f, 0.4f, 0.4f);
    drawText("DELETE BODY?", textLeftMargin, 0.32f, textSize * 1.2f);
    
    // Body info
    const Body& body = bodies[uiState.selectedBody];
    glColor3f(1.0f, 0.8f, 0.8f);
    drawText("Body " + std::to_string(uiState.selectedBody + 1) + " - Mass: " + floatToString(body.mass, 0) + " kg", 
             textLeftMargin, 0.28f, textSize * 0.9f);
    drawText("Position: (" + floatToString(body.x, 2) + ", " + floatToString(body.y, 2) + ")",
             textLeftMargin, 0.24f, textSize * 0.8f);
    
    // Warning message
    glColor3f(1.0f, 1.0f, 0.6f);
    if (bodies.size() <= 2) {
        drawText("Cannot delete - minimum 2 bodies required!", textLeftMargin, 0.19f, textSize * 0.8f);
    } else {
        drawText("This action cannot be undone!", textLeftMargin, 0.19f, textSize * 0.8f);
    }
    
    // Instructions
    glColor3f(0.9f, 0.9f, 0.9f);
    drawText("ENTER: Confirm Delete    ESC: Cancel", textLeftMargin, 0.14f, textSize * 0.9f);
}

bool deleteSelectedBody(std::vector<Body>& bodies, UIState& uiState) {
    if (uiState.selectedBody == -1 || uiState.selectedBody >= bodies.size()) return false;
    
    // Don't allow deletion if less than 3 bodies (need minimum 2 to remain)
    if (bodies.size() <= 2) {
        std::cout << "Cannot delete - minimum 2 bodies required for simulation" << std::endl;
        return false;
    }
    
    std::cout << "Deleting Body " << uiState.selectedBody + 1 << std::endl;
    
    // Remove the selected body
    bodies.erase(bodies.begin() + uiState.selectedBody);
    
    // Reset UI state
    uiState.selectedBody = -1;
    uiState.editMode = EditMode::NONE;
    uiState.dragging = false;
    
    std::cout << "Body deleted. " << bodies.size() << " bodies remaining." << std::endl;
    return true;
}

void cancelDeleteConfirm(UIState& uiState) {
    uiState.editMode = EditMode::NONE;
    std::cout << "Delete cancelled" << std::endl;
}

// Window size management functions
void setWindowSize(int width, int height) {
    g_uiWindowWidth = width;
    g_uiWindowHeight = height;
}

void getWindowSize(int& width, int& height) {
    width = g_uiWindowWidth;
    height = g_uiWindowHeight;
}

float getAspectRatio() {
    return (float)g_uiWindowWidth / (float)g_uiWindowHeight;
}

float getResponsiveTextSize(float baseSize) {
    // 창 크기에 따라 텍스트 크기 조정
    float aspectRatio = getAspectRatio();
    float scaleFactor = 1.0f;
    
    // 기본 해상도 (1200x800)에서 최적화되었으므로 이를 기준으로 스케일링
    float referenceWidth = 1200.0f;
    float referenceHeight = 800.0f;
    
    // 창이 너무 작거나 클 때 텍스트 크기 조정
    float widthScale = g_uiWindowWidth / referenceWidth;
    float heightScale = g_uiWindowHeight / referenceHeight;
    scaleFactor = std::min(widthScale, heightScale);
    
    // 창이 클 때는 텍스트 크기를 더 작게 조정
    if (scaleFactor > 1.0f) {
        scaleFactor = 1.0f + (scaleFactor - 1.0f) * 0.6f; // 60%로 스케일링 감소
    }
    
    // 최소/최대 크기 제한 - 최대 크기를 더 작게 제한
    scaleFactor = std::max(0.5f, std::min(scaleFactor, 1.4f));
    
    return baseSize * scaleFactor;
}

float getResponsiveTextSizeSmall(float baseSize) {
    // 편집 창용 더 작은 텍스트 크기 - 최대화 시에 더 작게
    float aspectRatio = getAspectRatio();
    float scaleFactor = 1.0f;
    
    // 기본 해상도 (1200x800)에서 최적화되었으므로 이를 기준으로 스케일링
    float referenceWidth = 1200.0f;
    float referenceHeight = 800.0f;
    
    // 창이 너무 작거나 클 때 텍스트 크기 조정
    float widthScale = g_uiWindowWidth / referenceWidth;
    float heightScale = g_uiWindowHeight / referenceHeight;
    scaleFactor = std::min(widthScale, heightScale);
    
    // 창이 클 때는 텍스트 크기를 더욱 작게 조정
    if (scaleFactor > 1.0f) {
        scaleFactor = 1.0f + (scaleFactor - 1.0f) * 0.4f; // 40%로 스케일링 더 감소
    }
    
    // 최소/최대 크기 제한 - 편집창용으로 더 작게
    scaleFactor = std::max(0.4f, std::min(scaleFactor, 1.1f));
    
    return baseSize * scaleFactor;
}

// Orbit trail functions
void initializeTrails(OrbitTrails& trails, size_t numBodies) {
    trails.trails.clear();
    trails.trails.resize(numBodies);
}

void updateTrails(OrbitTrails& trails, const std::vector<Body>& bodies) {
    // Resize trails if number of bodies changed
    if (trails.trails.size() != bodies.size()) {
        initializeTrails(trails, bodies.size());
    }
    
    // Add current positions to trails
    for (size_t i = 0; i < bodies.size(); i++) {
        auto& trail = trails.trails[i];
        
        // Add new point
        TrailPoint newPoint;
        newPoint.x = bodies[i].x;
        newPoint.y = bodies[i].y;
        newPoint.alpha = 1.0f;  // Full opacity for new point (will be updated below)
        
        trail.push_back(newPoint);
        
        // Remove old points if trail is too long
        while (trail.size() > trail_size) {
            trail.erase(trail.begin());
        }
        
        // Update alpha values for fade effect with exponential decay
        // j=0은 가장 오래된 점, j=trail.size()-1은 가장 새로운 점
        for (size_t j = 0; j < trail.size(); j++) {
            float normalizedAge = (float)j / (float)(trail.size() - 1);  // 0 (오래됨) ~ 1 (새로움)
            // 지수적 감쇠: 새로운 점은 진하고, 오래된 점으로 갈수록 빠르게 사라짐
            trail[j].alpha = pow(normalizedAge, 0.5f) * 0.9f + 0.05f;  // 0.05 ~ 0.95
        }
    }
}

void drawTrails(const OrbitTrails& trails, const std::vector<Body>& bodies, bool showTrails) {
    if (!showTrails) return;
    
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    for (size_t i = 0; i < trails.trails.size() && i < bodies.size(); i++) {
        const auto& trail = trails.trails[i];
        
        if (trail.size() < 2) continue;  // Need at least 2 points for a line
        
        // Draw trail as connected line segments with fade effect
        glLineWidth(2.0f);
        glBegin(GL_LINE_STRIP);
        
        for (size_t j = 0; j < trail.size(); j++) {
            float alpha = trail[j].alpha;
            glColor4f(bodies[i].r, bodies[i].g, bodies[i].b, alpha);
            glVertex2f(trail[j].x, trail[j].y);
        }
        
        glEnd();
    }
    
    glLineWidth(1.0f);  // Reset line width
    glDisable(GL_BLEND);
}

void clearTrails(OrbitTrails& trails) {
    for (auto& trail : trails.trails) {
        trail.clear();
    }
}

// Zoom functions implementation
void updateZoom(UIState& uiState, float deltaZoom, float mouseX, float mouseY) {
    float oldZoom = uiState.zoomLevel;
    
    // Apply zoom change
    uiState.zoomLevel += deltaZoom;
    
    // Clamp zoom level to limits
    if (uiState.zoomLevel < uiState.minZoom) {
        uiState.zoomLevel = uiState.minZoom;
    }
    if (uiState.zoomLevel > uiState.maxZoom) {
        uiState.zoomLevel = uiState.maxZoom;
    }
    
    // If zoom level didn't actually change, return early
    if (uiState.zoomLevel == oldZoom) {
        return;
    }
    
    // Update zoom center based on mouse position
    // If mouse coordinates are provided (non-zero), zoom towards mouse position
    if (mouseX != 0.0f || mouseY != 0.0f) {
        // Convert mouse position to world coordinates before zoom
        float worldX, worldY;
        screenToWorld(mouseX, mouseY, worldX, worldY, uiState, g_uiWindowWidth, g_uiWindowHeight);
        
        // Adjust zoom center to zoom towards the mouse position
        float zoomRatio = uiState.zoomLevel / oldZoom;
        uiState.zoomCenterX = worldX - (worldX - uiState.zoomCenterX) / zoomRatio;
        uiState.zoomCenterY = worldY - (worldY - uiState.zoomCenterY) / zoomRatio;
    }
    
    std::cout << "Zoom level: " << std::fixed << std::setprecision(2) << uiState.zoomLevel 
              << " (Center: " << uiState.zoomCenterX << ", " << uiState.zoomCenterY << ")" << std::endl;
}

void resetZoom(UIState& uiState) {
    uiState.zoomLevel = 1.0f;
    uiState.zoomCenterX = 0.0f;
    uiState.zoomCenterY = 0.0f;
    std::cout << "Zoom reset to default" << std::endl;
}

void applyZoomToProjection(const UIState& uiState, int windowWidth, int windowHeight) {
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    
    float aspect = (float)windowWidth / (float)windowHeight;
    
    // Calculate the view bounds based on zoom level and center
    float baseWidth, baseHeight;
    if (aspect > 1.0f) {
        baseWidth = aspect / uiState.zoomLevel;
        baseHeight = 1.0f / uiState.zoomLevel;
    } else {
        baseWidth = 1.0f / uiState.zoomLevel;
        baseHeight = (1.0f / aspect) / uiState.zoomLevel;
    }
    
    // Apply zoom center offset
    float left = -baseWidth + uiState.zoomCenterX;
    float right = baseWidth + uiState.zoomCenterX;
    float bottom = -baseHeight + uiState.zoomCenterY;
    float top = baseHeight + uiState.zoomCenterY;
    
    glOrtho(left, right, bottom, top, -1.0f, 1.0f);
    
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void worldToScreen(float worldX, float worldY, float& screenX, float& screenY, 
                   const UIState& uiState, int windowWidth, int windowHeight) {
    float aspect = (float)windowWidth / (float)windowHeight;
    
    // Calculate view bounds
    float baseWidth, baseHeight;
    if (aspect > 1.0f) {
        baseWidth = aspect / uiState.zoomLevel;
        baseHeight = 1.0f / uiState.zoomLevel;
    } else {
        baseWidth = 1.0f / uiState.zoomLevel;
        baseHeight = (1.0f / aspect) / uiState.zoomLevel;
    }
    
    float left = -baseWidth + uiState.zoomCenterX;
    float right = baseWidth + uiState.zoomCenterX;
    float bottom = -baseHeight + uiState.zoomCenterY;
    float top = baseHeight + uiState.zoomCenterY;
    
    // Convert world to normalized device coordinates
    float ndcX = (worldX - left) / (right - left) * 2.0f - 1.0f;
    float ndcY = (worldY - bottom) / (top - bottom) * 2.0f - 1.0f;
    
    // Convert to screen coordinates
    screenX = (ndcX + 1.0f) * 0.5f * windowWidth;
    screenY = (1.0f - ndcY) * 0.5f * windowHeight;
}

void screenToWorld(float screenX, float screenY, float& worldX, float& worldY, 
                   const UIState& uiState, int windowWidth, int windowHeight) {
    float aspect = (float)windowWidth / (float)windowHeight;
    
    // Calculate view bounds
    float baseWidth, baseHeight;
    if (aspect > 1.0f) {
        baseWidth = aspect / uiState.zoomLevel;
        baseHeight = 1.0f / uiState.zoomLevel;
    } else {
        baseWidth = 1.0f / uiState.zoomLevel;
        baseHeight = (1.0f / aspect) / uiState.zoomLevel;
    }
    
    float left = -baseWidth + uiState.zoomCenterX;
    float right = baseWidth + uiState.zoomCenterX;
    float bottom = -baseHeight + uiState.zoomCenterY;
    float top = baseHeight + uiState.zoomCenterY;
    
    // Convert screen to normalized device coordinates
    float ndcX = (screenX / windowWidth) * 2.0f - 1.0f;
    float ndcY = 1.0f - (screenY / windowHeight) * 2.0f;
    
    // Convert to world coordinates
    worldX = left + (ndcX + 1.0f) * 0.5f * (right - left);
    worldY = bottom + (ndcY + 1.0f) * 0.5f * (top - bottom);
}

// Camera panning functions implementation
void startPanning(UIState& uiState, float mouseX, float mouseY) {
    uiState.panning = true;
    uiState.panStartX = mouseX;
    uiState.panStartY = mouseY;
    uiState.panStartCenterX = uiState.zoomCenterX;
    uiState.panStartCenterY = uiState.zoomCenterY;
    std::cout << "Started panning at (" << mouseX << ", " << mouseY << ")" << std::endl;
}

void updatePanning(UIState& uiState, float mouseX, float mouseY, int windowWidth, int windowHeight) {
    if (!uiState.panning) return;
    
    float aspect = (float)windowWidth / (float)windowHeight;
    
    // Calculate how much the mouse moved in screen coordinates
    float deltaScreenX = mouseX - uiState.panStartX;
    float deltaScreenY = mouseY - uiState.panStartY;
    
    // Convert screen delta to world delta
    float baseWidth, baseHeight;
    if (aspect > 1.0f) {
        baseWidth = aspect / uiState.zoomLevel;
        baseHeight = 1.0f / uiState.zoomLevel;
    } else {
        baseWidth = 1.0f / uiState.zoomLevel;
        baseHeight = (1.0f / aspect) / uiState.zoomLevel;
    }
    
    // Convert screen movement to world movement
    float deltaWorldX = -(deltaScreenX / windowWidth) * 2.0f * baseWidth;
    float deltaWorldY = (deltaScreenY / windowHeight) * 2.0f * baseHeight;
    
    // Update zoom center to pan the view
    uiState.zoomCenterX = uiState.panStartCenterX + deltaWorldX;
    uiState.zoomCenterY = uiState.panStartCenterY + deltaWorldY;
}

void stopPanning(UIState& uiState) {
    uiState.panning = false;
    std::cout << "Stopped panning" << std::endl;
}

// Crosshair display function
void drawCrosshair(const UIState& uiState) {
    if (!uiState.showCrosshair) return;
    
    glColor3f(0.8f, 0.8f, 0.8f);  // Light gray color
    glLineWidth(1.0f);
    
    // Draw horizontal line
    glBegin(GL_LINES);
    glVertex2f(-0.03f, 0.0f);  // Left side of crosshair
    glVertex2f(0.03f, 0.0f);   // Right side of crosshair
    glEnd();
    
    // Draw vertical line  
    glBegin(GL_LINES);
    glVertex2f(0.0f, -0.03f);  // Bottom of crosshair
    glVertex2f(0.0f, 0.03f);   // Top of crosshair
    glEnd();
    
    // Draw small circle at center
    glPointSize(3.0f);
    glBegin(GL_POINTS);
    glVertex2f(0.0f, 0.0f);
    glEnd();
    glPointSize(1.0f);  // Reset point size
}

// Screen-fixed UI functions (not affected by zoom/pan)
void setupScreenFixedProjection(int windowWidth, int windowHeight) {
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    
    // Set up standard orthographic projection for screen-fixed UI
    float aspect = (float)windowWidth / (float)windowHeight;
    if (aspect > 1.0f) {
        glOrtho(-aspect, aspect, -1.0f, 1.0f, -1.0f, 1.0f);
    } else {
        glOrtho(-1.0f, 1.0f, -1.0f/aspect, 1.0f/aspect, -1.0f, 1.0f);
    }
    
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void restoreWorldProjection(const UIState& uiState, int windowWidth, int windowHeight) {
    applyZoomToProjection(uiState, windowWidth, windowHeight);
}

void drawInfoFixed(const std::vector<Body>& bodies, const UIState& uiState, int windowWidth, int windowHeight) {
    if (!uiState.showInfo) return;
    
    // Save current projection matrix
    GLfloat projMatrix[16];
    glGetFloatv(GL_PROJECTION_MATRIX, projMatrix);
    
    // Set up screen-fixed projection
    setupScreenFixedProjection(windowWidth, windowHeight);
    
    // Fixed panel position - always stick to the left edge
    float aspectRatio = (float)windowWidth / (float)windowHeight;
    float panelLeft = -aspectRatio + 0.02f;  // Always start from screen left edge + small margin
    float panelWidth = 0.6f;  // Fixed width in normalized coordinates
    float panelRight = panelLeft + panelWidth;
    
    // Semi-transparent background panel
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(0.0f, 0.0f, 0.0f, 0.8f);
    glBegin(GL_QUADS);
    glVertex2f(panelLeft, 0.95f); glVertex2f(panelRight, 0.95f);
    glVertex2f(panelRight, -0.95f); glVertex2f(panelLeft, -0.95f);
    glEnd();
    glDisable(GL_BLEND);
    
    glColor3f(1.0f, 1.0f, 1.0f);
    float textSize = getResponsiveTextSize(BASE_TEXT_SIZE);
    float textLeftMargin = panelLeft + 0.03f;  // Text margin from panel left
    
    // Title
    drawText("3-Body Simulation", textLeftMargin, 0.9f, getResponsiveTextSize(TITLE_TEXT_SIZE));
    
    // Body information
    for (size_t i = 0; i < bodies.size(); i++) {
        float yPos = 0.7f - i * 0.18f;
        
        // Body label background (clickable area indicator)
        float labelLeft = textLeftMargin - 0.01f;
        float labelRight = textLeftMargin + 0.16f;
        float labelTop = yPos + 0.02f;
        float labelBottom = yPos - 0.02f;
        
        // Highlight if this body is selected
        if ((int)i == uiState.selectedBody) {
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            glColor4f(bodies[i].r, bodies[i].g, bodies[i].b, 0.3f); // Semi-transparent body color
        } else {
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            glColor4f(0.2f, 0.2f, 0.2f, 0.4f); // Dark gray for clickable area
        }
        
        glBegin(GL_QUADS);
        glVertex2f(labelLeft, labelTop);
        glVertex2f(labelRight, labelTop);
        glVertex2f(labelRight, labelBottom);
        glVertex2f(labelLeft, labelBottom);
        glEnd();
        glDisable(GL_BLEND);
        
        // Body color for identification
        glColor3f(bodies[i].r, bodies[i].g, bodies[i].b);
        
        // Body number
        std::string bodyNum = "Body " + std::to_string(i + 1);
        drawText(bodyNum, textLeftMargin, yPos, textSize);
        
        glColor3f(0.9f, 0.9f, 0.9f);  // Light gray
        
        // Position info
        std::string posInfo = "Pos: " + floatToString(bodies[i].x, 2) + ", " + floatToString(bodies[i].y, 2);
        drawText(posInfo, textLeftMargin, yPos - 0.035f, getResponsiveTextSize(INFO_TEXT_SIZE));
        
        // Velocity info
        std::string velInfo = "Vel: " + floatToString(bodies[i].vx, 2) + ", " + floatToString(bodies[i].vy, 2);
        drawText(velInfo, textLeftMargin, yPos - 0.065f, getResponsiveTextSize(INFO_TEXT_SIZE));
        
        // Mass and speed
        float speed = sqrt(bodies[i].vx * bodies[i].vx + bodies[i].vy * bodies[i].vy);
        std::string statsInfo = "Mass: " + floatToString(bodies[i].mass, 0) + " Speed: " + floatToString(speed, 3);
        drawText(statsInfo, textLeftMargin, yPos - 0.095f, getResponsiveTextSize(INFO_TEXT_SIZE));
        
        // Separator line
        glColor3f(0.3f, 0.3f, 0.3f);
        glBegin(GL_LINES);
        glVertex2f(textLeftMargin, yPos - 0.13f);
        glVertex2f(panelRight - 0.05f, yPos - 0.13f);
        glEnd();
    }
    
    // Selected body indicator
    if (uiState.selectedBody != -1) {
        glColor3f(1.0f, 1.0f, 0.0f);  // Yellow
        drawText("SELECTED", textLeftMargin + 0.4f, 0.7f - uiState.selectedBody * 0.18f, textSize * 0.6f);
    }
    
    // Controls info
    glColor3f(0.7f, 0.7f, 0.9f);
    drawText("Controls", textLeftMargin, -0.55f, textSize);
    
    glColor3f(0.8f, 0.8f, 0.8f);
    drawText("Left Click: Move bodies", textLeftMargin, -0.6f, getResponsiveTextSize(INFO_TEXT_SIZE));
    drawText("Right Click: Pan view", textLeftMargin, -0.65f, getResponsiveTextSize(INFO_TEXT_SIZE));
    drawText("Wheel: Zoom", textLeftMargin, -0.7f, getResponsiveTextSize(INFO_TEXT_SIZE));
    drawText("Space: Pause", textLeftMargin, -0.75f, getResponsiveTextSize(INFO_TEXT_SIZE));
    drawText("H: Toggle crosshair", textLeftMargin, -0.8f, getResponsiveTextSize(INFO_TEXT_SIZE));
    drawText("R: Reset", textLeftMargin, -0.85f, getResponsiveTextSize(INFO_TEXT_SIZE));
    drawText("ESC: Exit", textLeftMargin, -0.9f, getResponsiveTextSize(INFO_TEXT_SIZE));
    
    // Restore original projection matrix
    glMatrixMode(GL_PROJECTION);
    glLoadMatrixf(projMatrix);
    glMatrixMode(GL_MODELVIEW);
}

void drawCoordinateModeFixed(CoordinateMode mode, int windowWidth, int windowHeight) {
    // Save current projection matrix
    GLfloat projMatrix[16];
    glGetFloatv(GL_PROJECTION_MATRIX, projMatrix);
    
    // Set up screen-fixed projection
    setupScreenFixedProjection(windowWidth, windowHeight);
    
    float aspectRatio = (float)windowWidth / (float)windowHeight;
    
    // Position coordinate mode display at bottom right
    glColor3f(0.8f, 0.8f, 0.8f);
    std::string modeText = (mode == CoordinateMode::CARTESIAN) ? "Cartesian [x,y]" : "Polar [r,th]";
    drawText(modeText, aspectRatio - 0.35f, -0.9f, getResponsiveTextSize(INFO_TEXT_SIZE));
    
    // Restore original projection matrix
    glMatrixMode(GL_PROJECTION);
    glLoadMatrixf(projMatrix);
    glMatrixMode(GL_MODELVIEW);
}

// UI interaction functions
void screenToUICoords(float screenX, float screenY, float& uiX, float& uiY, int windowWidth, int windowHeight) {
    float aspectRatio = (float)windowWidth / (float)windowHeight;
    
    // Convert screen coordinates to normalized UI coordinates (-1 to 1)
    uiX = (screenX / windowWidth) * 2.0f - 1.0f;
    uiY = 1.0f - (screenY / windowHeight) * 2.0f;
    
    // Apply aspect ratio correction
    if (aspectRatio > 1.0f) {
        uiX *= aspectRatio;
    } else {
        uiY /= aspectRatio;
    }
}

int checkBodyLabelClick(float mouseX, float mouseY, const std::vector<Body>& bodies, int windowWidth, int windowHeight) {
    float uiX, uiY;
    screenToUICoords(mouseX, mouseY, uiX, uiY, windowWidth, windowHeight);
    
    float aspectRatio = (float)windowWidth / (float)windowHeight;
    float panelLeft = -aspectRatio + 0.02f;
    float textLeftMargin = panelLeft + 0.03f;
    
    // Check each body label area
    for (size_t i = 0; i < bodies.size(); i++) {
        float yPos = 0.7f - i * 0.18f;
        
        // Define clickable area for body label (approximate text bounds)
        float labelLeft = textLeftMargin;
        float labelRight = textLeftMargin + 0.15f; // Approximate width of "Body X" text
        float labelTop = yPos + 0.02f;
        float labelBottom = yPos - 0.02f;
        
        // Check if click is within this body's label area
        if (uiX >= labelLeft && uiX <= labelRight && 
            uiY >= labelBottom && uiY <= labelTop) {
            return (int)i;  // Return body index
        }
    }
    
    return -1;  // No body label clicked
}

// Body-attached editors
void drawVelocityEditorAttached(const std::vector<Body>& bodies, UIState& uiState) {
    if (uiState.editMode != EditMode::VELOCITY || uiState.selectedBody == -1) return;
    
    const Body& body = bodies[uiState.selectedBody];
    
    // Position the editor near the selected body
    float editorX = body.x + 0.15f;  // Offset to the right of the body
    float editorY = body.y + 0.1f;   // Offset above the body
    float editorWidth = 0.4f;
    float editorHeight = 0.28f;      // Increased height for better spacing
    
    // Clamp to screen bounds (approximate)
    if (editorX + editorWidth > 1.5f) editorX = body.x - editorWidth - 0.15f;  // Move to left
    if (editorY + editorHeight > 1.0f) editorY = body.y - editorHeight - 0.1f; // Move below
    if (editorX < -1.5f) editorX = -1.4f;
    if (editorY < -1.0f) editorY = -0.9f;
    
    float panelLeft = editorX;
    float panelRight = editorX + editorWidth;
    float panelTop = editorY + editorHeight;
    float panelBottom = editorY;
    
    // Editor background
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(0.1f, 0.1f, 0.3f, 0.9f);
    glBegin(GL_QUADS);
    glVertex2f(panelLeft, panelTop); glVertex2f(panelRight, panelTop);
    glVertex2f(panelRight, panelBottom); glVertex2f(panelLeft, panelBottom);
    glEnd();
    
    // Border
    glColor3f(body.r, body.g, body.b);  // Use body color for border
    glLineWidth(2.0f);
    glBegin(GL_LINE_LOOP);
    glVertex2f(panelLeft, panelTop); glVertex2f(panelRight, panelTop);
    glVertex2f(panelRight, panelBottom); glVertex2f(panelLeft, panelBottom);
    glEnd();
    glLineWidth(1.0f);
    
    // Connection line to body
    glColor4f(body.r, body.g, body.b, 0.6f);
    glBegin(GL_LINES);
    glVertex2f(body.x, body.y);
    glVertex2f(panelLeft, panelBottom + editorHeight * 0.5f);
    glEnd();
    glDisable(GL_BLEND);
    
    glColor3f(1.0f, 1.0f, 1.0f);
    float textSize = getResponsiveTextSizeSmall(INFO_TEXT_SIZE);
    float textLeftMargin = panelLeft + 0.02f;
    
    // Title
    glColor3f(body.r, body.g, body.b);
    std::string title = "Body " + std::to_string(uiState.selectedBody + 1) + " Velocity";
    drawText(title, textLeftMargin, panelTop - 0.04f, textSize);
    
    glColor3f(1.0f, 1.0f, 1.0f);
    
    // Current values display
    std::string coordLabel = (uiState.coordMode == CoordinateMode::CARTESIAN) ? "Cartesian [x,y]" : "Polar [r,th]";
    drawText(coordLabel, textLeftMargin, panelTop - 0.08f, textSize * 0.8f);
    
    if (uiState.coordMode == CoordinateMode::CARTESIAN) {
        // Cartesian mode
        std::string vxLabel = "Vx: " + floatToString(uiState.editVelocityX, 3);
        std::string vyLabel = "Vy: " + floatToString(uiState.editVelocityY, 3);
        
        if (uiState.currentEditField == 0) glColor3f(1.0f, 1.0f, 0.0f);
        drawText(vxLabel, textLeftMargin, panelTop - 0.12f, textSize);
        glColor3f(1.0f, 1.0f, 1.0f);
        
        if (uiState.currentEditField == 1) glColor3f(1.0f, 1.0f, 0.0f);
        drawText(vyLabel, textLeftMargin, panelTop - 0.16f, textSize);
        glColor3f(1.0f, 1.0f, 1.0f);
    } else {
        // Polar mode
        std::string vrLabel = "Vr: " + floatToString(uiState.editVelocityR, 3);
        std::string vthetaLabel = "Vth: " + floatToString(uiState.editVelocityTheta, 1) + "°";
        
        if (uiState.currentEditField == 0) glColor3f(1.0f, 1.0f, 0.0f);
        drawText(vrLabel, textLeftMargin, panelTop - 0.12f, textSize);
        glColor3f(1.0f, 1.0f, 1.0f);
        
        if (uiState.currentEditField == 1) glColor3f(1.0f, 1.0f, 0.0f);
        drawText(vthetaLabel, textLeftMargin, panelTop - 0.16f, textSize);
        glColor3f(1.0f, 1.0f, 1.0f);
    }
    
    // Input prompt
    if (uiState.textInputMode) {
        glColor3f(1.0f, 1.0f, 0.0f);
        std::string inputPrompt = "Input: " + uiState.inputBuffer + "_";
        drawText(inputPrompt, textLeftMargin, panelTop - 0.20f, textSize);
    }
    
    // Controls
    glColor3f(0.8f, 0.8f, 0.8f);
    drawText("TAB: Switch field | Enter: Apply | ESC: Cancel", textLeftMargin, panelBottom + 0.02f, textSize * 0.7f);
}

void drawMassEditorAttached(const std::vector<Body>& bodies, UIState& uiState) {
    if (uiState.editMode != EditMode::MASS || uiState.selectedBody == -1) return;
    
    const Body& body = bodies[uiState.selectedBody];
    
    // Position the editor near the selected body
    float editorX = body.x + 0.15f;  // Offset to the right of the body
    float editorY = body.y + 0.05f;  // Offset above the body
    float editorWidth = 0.35f;
    float editorHeight = 0.22f;      // Increased height to prevent text overlap
    
    // Clamp to screen bounds (approximate)
    if (editorX + editorWidth > 1.5f) editorX = body.x - editorWidth - 0.15f;  // Move to left
    if (editorY + editorHeight > 1.0f) editorY = body.y - editorHeight - 0.1f; // Move below
    if (editorX < -1.5f) editorX = -1.4f;
    if (editorY < -1.0f) editorY = -0.9f;
    
    float panelLeft = editorX;
    float panelRight = editorX + editorWidth;
    float panelTop = editorY + editorHeight;
    float panelBottom = editorY;
    
    // Editor background
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(0.3f, 0.1f, 0.1f, 0.9f); // Reddish background for mass
    glBegin(GL_QUADS);
    glVertex2f(panelLeft, panelTop); glVertex2f(panelRight, panelTop);
    glVertex2f(panelRight, panelBottom); glVertex2f(panelLeft, panelBottom);
    glEnd();
    
    // Border
    glColor3f(body.r, body.g, body.b);  // Use body color for border
    glLineWidth(2.0f);
    glBegin(GL_LINE_LOOP);
    glVertex2f(panelLeft, panelTop); glVertex2f(panelRight, panelTop);
    glVertex2f(panelRight, panelBottom); glVertex2f(panelLeft, panelBottom);
    glEnd();
    glLineWidth(1.0f);
    
    // Connection line to body
    glColor4f(body.r, body.g, body.b, 0.6f);
    glBegin(GL_LINES);
    glVertex2f(body.x, body.y);
    glVertex2f(panelLeft, panelBottom + editorHeight * 0.5f);
    glEnd();
    glDisable(GL_BLEND);
    
    glColor3f(1.0f, 1.0f, 1.0f);
    float textSize = getResponsiveTextSizeSmall(INFO_TEXT_SIZE);
    float textLeftMargin = panelLeft + 0.02f;
    
    // Title
    glColor3f(body.r, body.g, body.b);
    std::string title = "Body " + std::to_string(uiState.selectedBody + 1) + " Mass";
    drawText(title, textLeftMargin, panelTop - 0.03f, textSize);
    
    // Current mass display
    glColor3f(1.0f, 1.0f, 0.0f);
    std::string massLabel = "Mass: " + floatToString(uiState.editMass, 1);
    drawText(massLabel, textLeftMargin, panelTop - 0.07f, textSize);
    
    // Current mass from body
    glColor3f(1.0f, 1.0f, 1.0f);
    std::string currentMass = "Current: " + floatToString(body.mass, 1);
    drawText(currentMass, textLeftMargin, panelTop - 0.11f, textSize * 0.8f);
    
    // Input prompt
    if (uiState.textInputMode) {
        glColor3f(1.0f, 1.0f, 0.0f);
        std::string inputPrompt = "Input: " + uiState.inputBuffer + "_";
        drawText(inputPrompt, textLeftMargin, panelTop - 0.15f, textSize);
    }
    
    // Controls
    glColor3f(0.8f, 0.8f, 0.8f);
    drawText("Enter: Apply | ESC: Cancel", textLeftMargin, panelBottom + 0.02f, textSize * 0.7f);
}

// 카메라 추적 업데이트
void updateCameraFollow(UIState& uiState, const std::vector<Body>& bodies) {
    if (uiState.followMode && uiState.followTarget != -1 && 
        uiState.followTarget < bodies.size()) {
        const Body& target = bodies[uiState.followTarget];
        uiState.zoomCenterX = target.x;
        uiState.zoomCenterY = target.y;
    }
}