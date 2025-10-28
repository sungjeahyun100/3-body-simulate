#include "physics.h"
#include <algorithm>
#include <cmath>

// Physics constants
const float G = 6.67e-8f;  // Newton's gravitational constant (real physics value)
const float dt = 0.016f;    // Time interval (60 FPS)

// Coordinate conversion functions
PolarCoord cartesianToPolar(const CartesianCoord& cart) {
    PolarCoord polar;
    polar.r = sqrt(cart.x * cart.x + cart.y * cart.y);
    polar.theta = atan2(cart.y, cart.x);
    return polar;
}

CartesianCoord polarToCartesian(const PolarCoord& polar) {
    CartesianCoord cart;
    cart.x = polar.r * cos(polar.theta);
    cart.y = polar.r * sin(polar.theta);
    return cart;
}

PolarCoord velocityToPolar(float vx, float vy) {
    PolarCoord polar;
    polar.r = sqrt(vx * vx + vy * vy);  // Speed magnitude
    polar.theta = atan2(vy, vx);        // Direction angle
    return polar;
}

CartesianCoord polarToVelocity(const PolarCoord& polar) {
    CartesianCoord vel;
    vel.x = polar.r * cos(polar.theta);
    vel.y = polar.r * sin(polar.theta);
    return vel;
}

// Calculate gravitational force between two bodies
void calculateForce(const Body& a, const Body& b, float& fx, float& fy) {
    float dx = b.x - a.x;
    float dy = b.y - a.y;
    float distance = sqrt(dx * dx + dy * dy);
    
    // Prevent division by zero and extreme forces
    if (distance < 0.01f) distance = 0.01f;
    
    // F = G * m1 * m2 / r^2
    float force = G * a.mass * b.mass / (distance * distance);
    
    // Force components
    fx = force * dx / distance;
    fy = force * dy / distance;
}

// Calculate total gravitational force on one body from all other bodies
void calculateTotalForce(const Body& body, const std::vector<Body>& allBodies, float& fx, float& fy) {
    fx = 0.0f;
    fy = 0.0f;
    
    for (const Body& other : allBodies) {
        // Skip self
        if (&body == &other) continue;
        
        float force_x, force_y;
        calculateForce(body, other, force_x, force_y);
        fx += force_x;
        fy += force_y;
    }
}

// Update physics simulation (Euler method - original)
void updatePhysics(std::vector<Body>& bodies, bool paused) {
    if (paused) return;
    
    std::vector<float> fx(bodies.size(), 0.0f);
    std::vector<float> fy(bodies.size(), 0.0f);
    
    // Calculate forces between all pairs
    for (size_t i = 0; i < bodies.size(); i++) {
        for (size_t j = i + 1; j < bodies.size(); j++) {
            float force_x, force_y;
            calculateForce(bodies[i], bodies[j], force_x, force_y);
            
            // Newton's third law: action-reaction
            fx[i] += force_x;
            fy[i] += force_y;
            fx[j] -= force_x;
            fy[j] -= force_y;
        }
    }
    
    // Update velocities and positions
    for (size_t i = 0; i < bodies.size(); i++) {
        // a = F / m
        float ax = fx[i] / bodies[i].mass;
        float ay = fy[i] / bodies[i].mass;
        
        // v = v + a * dt
        bodies[i].vx += ax * dt;
        bodies[i].vy += ay * dt;
        
        // x = x + v * dt
        bodies[i].x += bodies[i].vx * dt;
        bodies[i].y += bodies[i].vy * dt;
    }
}

// Update physics simulation using RK4 method (more accurate)
void updatePhysicsRK4(std::vector<Body>& bodies, bool paused) {
    if (paused) return;
    
    size_t n = bodies.size();
    
    // Store original state
    std::vector<Body> original = bodies;
    
    // RK4 coefficients for each body
    std::vector<float> k1_x(n), k1_y(n), k1_vx(n), k1_vy(n);
    std::vector<float> k2_x(n), k2_y(n), k2_vx(n), k2_vy(n);
    std::vector<float> k3_x(n), k3_y(n), k3_vx(n), k3_vy(n);
    std::vector<float> k4_x(n), k4_y(n), k4_vx(n), k4_vy(n);
    
    // Step 1: Calculate k1
    for (size_t i = 0; i < n; i++) {
        float fx, fy;
        calculateTotalForce(bodies[i], bodies, fx, fy);
        
        k1_x[i] = dt * bodies[i].vx;
        k1_y[i] = dt * bodies[i].vy;
        k1_vx[i] = dt * fx / bodies[i].mass;
        k1_vy[i] = dt * fy / bodies[i].mass;
    }
    
    // Step 2: Calculate k2 (at t + dt/2, using k1)
    for (size_t i = 0; i < n; i++) {
        bodies[i].x = original[i].x + k1_x[i] * 0.5f;
        bodies[i].y = original[i].y + k1_y[i] * 0.5f;
        bodies[i].vx = original[i].vx + k1_vx[i] * 0.5f;
        bodies[i].vy = original[i].vy + k1_vy[i] * 0.5f;
    }
    
    for (size_t i = 0; i < n; i++) {
        float fx, fy;
        calculateTotalForce(bodies[i], bodies, fx, fy);
        
        k2_x[i] = dt * bodies[i].vx;
        k2_y[i] = dt * bodies[i].vy;
        k2_vx[i] = dt * fx / bodies[i].mass;
        k2_vy[i] = dt * fy / bodies[i].mass;
    }
    
    // Step 3: Calculate k3 (at t + dt/2, using k2)
    for (size_t i = 0; i < n; i++) {
        bodies[i].x = original[i].x + k2_x[i] * 0.5f;
        bodies[i].y = original[i].y + k2_y[i] * 0.5f;
        bodies[i].vx = original[i].vx + k2_vx[i] * 0.5f;
        bodies[i].vy = original[i].vy + k2_vy[i] * 0.5f;
    }
    
    for (size_t i = 0; i < n; i++) {
        float fx, fy;
        calculateTotalForce(bodies[i], bodies, fx, fy);
        
        k3_x[i] = dt * bodies[i].vx;
        k3_y[i] = dt * bodies[i].vy;
        k3_vx[i] = dt * fx / bodies[i].mass;
        k3_vy[i] = dt * fy / bodies[i].mass;
    }
    
    // Step 4: Calculate k4 (at t + dt, using k3)
    for (size_t i = 0; i < n; i++) {
        bodies[i].x = original[i].x + k3_x[i];
        bodies[i].y = original[i].y + k3_y[i];
        bodies[i].vx = original[i].vx + k3_vx[i];
        bodies[i].vy = original[i].vy + k3_vy[i];
    }
    
    for (size_t i = 0; i < n; i++) {
        float fx, fy;
        calculateTotalForce(bodies[i], bodies, fx, fy);
        
        k4_x[i] = dt * bodies[i].vx;
        k4_y[i] = dt * bodies[i].vy;
        k4_vx[i] = dt * fx / bodies[i].mass;
        k4_vy[i] = dt * fy / bodies[i].mass;
    }
    
    // Final step: Combine all k values with RK4 weights
    for (size_t i = 0; i < n; i++) {
        bodies[i].x = original[i].x + (k1_x[i] + 2*k2_x[i] + 2*k3_x[i] + k4_x[i]) / 6.0f;
        bodies[i].y = original[i].y + (k1_y[i] + 2*k2_y[i] + 2*k3_y[i] + k4_y[i]) / 6.0f;
        bodies[i].vx = original[i].vx + (k1_vx[i] + 2*k2_vx[i] + 2*k3_vx[i] + k4_vx[i]) / 6.0f;
        bodies[i].vy = original[i].vy + (k1_vy[i] + 2*k2_vy[i] + 2*k3_vy[i] + k4_vy[i]) / 6.0f;
    }
}

// Boundary collision variables
static float boundaryRestitution = 0.8f;  // Energy loss on collision (0=완전 비탄성, 1=완전 탄성)

void updateBodyCollisions(std::vector<Body>& bodies){
    const float restitution = 1.0f; // Perfectly elastic by default

    for (size_t i = 0; i < bodies.size(); ++i) {
        for (size_t j = i + 1; j < bodies.size(); ++j) {
            Body& a = bodies[i];
            Body& b = bodies[j];

            float radiusA = (a.size_R > 0.0f) ? a.size_R : 0.02f;
            float radiusB = (b.size_R > 0.0f) ? b.size_R : 0.02f;
            float combinedRadius = radiusA + radiusB;

            float dx = b.x - a.x;
            float dy = b.y - a.y;
            float distanceSq = dx * dx + dy * dy;

            if (distanceSq >= combinedRadius * combinedRadius) {
                continue;
            }

            float distance = std::sqrt(distanceSq);
            if (distance < 1e-6f) {
                distance = combinedRadius; // Avoid division by zero by treating as full overlap
                dx = combinedRadius;
                dy = 0.0f;
            }

            float nx = dx / distance;
            float ny = dy / distance;

            float overlap = combinedRadius - distance;
            if (overlap > 0.0f) {
                float totalMass = std::max(a.mass + b.mass, 1.0f);
                float weightA = b.mass / totalMass;
                float weightB = a.mass / totalMass;

                a.x -= nx * overlap * weightA;
                a.y -= ny * overlap * weightA;
                b.x += nx * overlap * weightB;
                b.y += ny * overlap * weightB;
            }

            float relVel = (a.vx - b.vx) * nx + (a.vy - b.vy) * ny;
            if (relVel > 0.0f) {
                continue; // Bodies already moving apart along the normal
            }

            float invMassA = (a.mass > 0.0f) ? 1.0f / a.mass : 0.0f;
            float invMassB = (b.mass > 0.0f) ? 1.0f / b.mass : 0.0f;
            float impulse = -(1.0f + restitution) * relVel;
            float invMassSum = invMassA + invMassB;

            if (invMassSum <= 0.0f) {
                continue;
            }

            impulse /= invMassSum;

            a.vx += impulse * nx * invMassA;
            a.vy += impulse * ny * invMassA;
            b.vx -= impulse * nx * invMassB;
            b.vy -= impulse * ny * invMassB;
        }
    }
}


// Boundary collision functions
void handleBoundaryCollisions(std::vector<Body>& bodies, float aspectRatio) {
    // 화면 경계 계산 (OpenGL 정규화 좌표계)
    float leftBound = -aspectRatio;
    float rightBound = aspectRatio;
    float bottomBound = -1.0f;
    float topBound = 1.0f;
    
    for (Body& body : bodies) {
        bool collided = false;
        
        // 왼쪽 벽 충돌
        if (body.x <= leftBound) {
            body.x = leftBound;
            body.vx = -body.vx * boundaryRestitution;
            collided = true;
        }
        // 오른쪽 벽 충돌
        else if (body.x >= rightBound) {
            body.x = rightBound;
            body.vx = -body.vx * boundaryRestitution;
            collided = true;
        }
        
        // 아래쪽 벽 충돌
        if (body.y <= bottomBound) {
            body.y = bottomBound;
            body.vy = -body.vy * boundaryRestitution;
            collided = true;
        }
        // 위쪽 벽 충돌
        else if (body.y >= topBound) {
            body.y = topBound;
            body.vy = -body.vy * boundaryRestitution;
            collided = true;
        }
        
        // 충돌 시 약간의 마찰 효과 (선택적)
        if (collided) {
            body.vx *= 0.98f;  // 2% 마찰 손실
            body.vy *= 0.98f;
        }
    }
}

void setBoundaryRestitution(float restitution) {
    // 0.0 ~ 1.0 범위로 제한
    boundaryRestitution = std::max(0.0f, std::min(1.0f, restitution));
}

float getBoundaryRestitution() {
    return boundaryRestitution;
}

// Utility functions
float magnitude(float x, float y) {
    return sqrt(x * x + y * y);
}

float angleBetween(float x1, float y1, float x2, float y2) {
    return atan2(y2 - y1, x2 - x1);
}