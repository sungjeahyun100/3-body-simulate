#ifndef PHYSICS_H
#define PHYSICS_H

#include <vector>
#include <cmath>

struct Body {
    float size_R; //면적: PI*size_R^2
    float x, y, vx, vy, mass;
    float r, g, b;
};

// Coordinate system conversion
struct PolarCoord {
    float r;      // magnitude (speed)
    float theta;  // angle in radians
};

struct CartesianCoord {
    float x, y;
};

// Physics constants
extern const float G;  // Gravitational constant
extern const float dt; // Time step

// Coordinate conversion functions
PolarCoord cartesianToPolar(const CartesianCoord& cart);
CartesianCoord polarToCartesian(const PolarCoord& polar);
PolarCoord velocityToPolar(float vx, float vy);
CartesianCoord polarToVelocity(const PolarCoord& polar);

// Physics functions
void calculateForce(const Body& a, const Body& b, float& fx, float& fy);
void calculateTotalForce(const Body& body, const std::vector<Body>& allBodies, float& fx, float& fy);
void updatePhysics(std::vector<Body>& bodies, bool paused);
void updatePhysicsRK4(std::vector<Body>& bodies, bool paused);
void updateBodyCollisions(std::vector<Body>& bodies);

// Boundary collision functions
void handleBoundaryCollisions(std::vector<Body>& bodies, float aspectRatio);
void setBoundaryRestitution(float restitution);
float getBoundaryRestitution();

// Utility functions
float magnitude(float x, float y);
float angleBetween(float x1, float y1, float x2, float y2);

#endif // PHYSICS_H