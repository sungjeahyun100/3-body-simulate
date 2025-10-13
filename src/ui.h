#ifndef UI_H
#define UI_H

#include <string>
#include <vector>
#include "physics.h"

// UI State management
enum class CoordinateMode {
    CARTESIAN,
    POLAR
};

enum class EditMode {
    NONE,
    VELOCITY,
    MASS,
    POSITION,
    ADD_BODY,
    DELETE_CONFIRM
};

struct UIState {
    bool showInfo = true;
    CoordinateMode coordMode = CoordinateMode::CARTESIAN;
    EditMode editMode = EditMode::NONE;
    int selectedBody = -1;
    bool dragging = false;
    bool paused = false;
    
    // Zoom system
    float zoomLevel = 1.0f;        // Current zoom level (1.0 = normal, >1.0 = zoomed in, <1.0 = zoomed out)
    float zoomCenterX = 0.0f;      // Zoom center X in world coordinates
    float zoomCenterY = 0.0f;      // Zoom center Y in world coordinates
    float minZoom = 0.1f;          // Minimum zoom level (max zoom out)
    float maxZoom = 10.0f;         // Maximum zoom level (max zoom in)
    float zoomSensitivity = 0.1f;  // Zoom sensitivity for scroll wheel
    
    // Camera panning system
    bool panning = false;          // Is currently panning the view
    float panStartX = 0.0f;        // Pan start position X (screen coordinates)
    float panStartY = 0.0f;        // Pan start position Y (screen coordinates)
    float panStartCenterX = 0.0f;  // Zoom center at pan start X
    float panStartCenterY = 0.0f;  // Zoom center at pan start Y
    bool showCrosshair = true;     // Show crosshair at screen center
    
    // Camera follow system
    bool followMode = false;       // Is camera following a body
    int followTarget = -1;         // Index of body to follow (-1 = none)
    
    // Velocity editing
    float editVelocityR = 0.0f;     // Polar magnitude
    float editVelocityTheta = 0.0f; // Polar angle (degrees)
    float editVelocityX = 0.0f;     // Cartesian X
    float editVelocityY = 0.0f;     // Cartesian Y
    int currentEditField = 0;       // 0 = first field (X or R), 1 = second field (Y or θ)
    
    // Mass editing
    float editMass = 1000.0f;       // Mass value for editing
    
    // Text input for velocity/mass editing
    std::string inputBuffer = "";   // Current text input
    bool textInputMode = false;     // Is currently inputting text
    int inputFieldIndex = 0;        // Which field is being edited (0=first, 1=second, 2=mass)
    
    // Direction dragging for velocity
    bool draggingDirection = false; // Is dragging for direction
    float dragStartX = 0.0f, dragStartY = 0.0f; // Drag start position
    float dragCurrentX = 0.0f, dragCurrentY = 0.0f; // Current drag position
    
    // Direct velocity vector manipulation
    bool draggingVelocityVector = false; // Is dragging the velocity vector directly
    float velocityVectorScale = 0.3f;    // Scale factor for velocity visualization
    
    // Add body mode
    bool addBodyMode = false;       // Is in add body mode
    float newBodyX = 0.0f, newBodyY = 0.0f; // Position for new body
    float newBodyVx = 0.0f, newBodyVy = 0.0f; // Velocity for new body
    float newBodyMass = 1000.0f;    // Mass for new body
    
    // Orbit trail system
    bool showTrails = true;         // Show orbit trails
};

// Structure for storing trail points
struct TrailPoint {
    float x, y;
    float alpha;  // Transparency (fades over time)
};

// Global trail storage
struct OrbitTrails {
    std::vector<std::vector<TrailPoint>> trails;  // One trail per body
};

// UI Drawing functions
void drawInfo(const std::vector<Body>& bodies, const UIState& uiState);
void drawDirectionArrow(const UIState& uiState);
void drawCurrentVelocityVector(const std::vector<Body>& bodies, const UIState& uiState);
void drawAddBodyEditor(UIState& uiState);
void drawAddBodyPreview(const UIState& uiState);

// Screen-fixed UI functions (not affected by zoom/pan)
void setupScreenFixedProjection(int windowWidth, int windowHeight);
void drawInfoFixed(const std::vector<Body>& bodies, const UIState& uiState, int windowWidth, int windowHeight);
void drawCoordinateModeFixed(CoordinateMode mode, int windowWidth, int windowHeight);

// UI interaction functions
int checkBodyLabelClick(float mouseX, float mouseY, const std::vector<Body>& bodies, int windowWidth, int windowHeight);
void screenToUICoords(float screenX, float screenY, float& uiX, float& uiY, int windowWidth, int windowHeight);

// Body-attached editors
void drawVelocityEditorAttached(const std::vector<Body>& bodies, UIState& uiState);
void drawMassEditorAttached(const std::vector<Body>& bodies, UIState& uiState);

// Orbit trail functions
void initializeTrails(OrbitTrails& trails, size_t numBodies);
void updateTrails(OrbitTrails& trails, const std::vector<Body>& bodies);
void drawTrails(const OrbitTrails& trails, const std::vector<Body>& bodies, bool showTrails);
void clearTrails(OrbitTrails& trails);

// Utility functions
std::string floatToString(float value, int precision = 2);
std::string angleToString(float radians); // Convert radians to degrees string
float stringToAngle(const std::string& degrees); // Convert degrees string to radians

// UI Event handling
void handleVelocityEdit(std::vector<Body>& bodies, UIState& uiState);
void applyVelocityEdit(std::vector<Body>& bodies, const UIState& uiState);
void handleMassEdit(std::vector<Body>& bodies, UIState& uiState);
void applyMassEdit(std::vector<Body>& bodies, const UIState& uiState);
void cancelEdit(UIState& uiState);

// Text input handling
void startTextInput(UIState& uiState, int fieldIndex);
void handleTextInput(UIState& uiState, char character);
void handleBackspace(UIState& uiState);
void applyTextInput(UIState& uiState);
void applyTextInputImmediate(UIState& uiState);
void cancelTextInput(UIState& uiState);

// Direction dragging
void startDirectionDrag(UIState& uiState, float x, float y);
void updateDirectionDrag(UIState& uiState, float x, float y);
void applyDirectionDrag(UIState& uiState);

// Direct velocity vector manipulation
bool isPointOnVelocityVector(const std::vector<Body>& bodies, const UIState& uiState, float x, float y);
void startVelocityVectorDrag(UIState& uiState, float x, float y);
void updateVelocityVectorDrag(std::vector<Body>& bodies, UIState& uiState, float x, float y);
void finishVelocityVectorDrag(UIState& uiState);

// Add body functions
void startAddBodyMode(UIState& uiState);
void setNewBodyPosition(UIState& uiState, float x, float y);
void handleAddBodyEdit(UIState& uiState);
void applyAddBodyEdit(std::vector<Body>& bodies, const UIState& uiState);
void cancelAddBody(UIState& uiState);

// Window size functions
// Delete body functions
void startDeleteConfirm(UIState& uiState);
void drawDeleteConfirm(const std::vector<Body>& bodies, const UIState& uiState);
bool deleteSelectedBody(std::vector<Body>& bodies, UIState& uiState);
void cancelDeleteConfirm(UIState& uiState);

// Window size functions
void setWindowSize(int width, int height);
void getWindowSize(int& width, int& height);
float getAspectRatio();
float getResponsiveTextSize(float baseSize);
float getResponsiveTextSizeSmall(float baseSize);

// Zoom functions
void updateZoom(UIState& uiState, float deltaZoom, float mouseX = 0.0f, float mouseY = 0.0f);
void resetZoom(UIState& uiState);
void applyZoomToProjection(const UIState& uiState, int windowWidth, int windowHeight);
void worldToScreen(float worldX, float worldY, float& screenX, float& screenY, const UIState& uiState, int windowWidth, int windowHeight);
void screenToWorld(float screenX, float screenY, float& worldX, float& worldY, const UIState& uiState, int windowWidth, int windowHeight);

// Camera panning functions
void startPanning(UIState& uiState, float mouseX, float mouseY);
void updatePanning(UIState& uiState, float mouseX, float mouseY, int windowWidth, int windowHeight);
void stopPanning(UIState& uiState);

// Camera follow function
void updateCameraFollow(UIState& uiState, const std::vector<Body>& bodies);

// Crosshair display
void drawCrosshair(const UIState& uiState);

#endif // UI_H