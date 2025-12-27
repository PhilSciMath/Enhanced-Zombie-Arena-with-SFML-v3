#include "Bullet.hpp"
#include <SFML/Graphics/Rect.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/System/Angle.hpp>
#include <cmath>


// Constructor
Bullet::Bullet() {
    m_BulletShape.setSize(sf::Vector2f({5,5}));
}

void Bullet::shoot(float startX, float startY, float xTarget, float yTarget) {
    // Keep track of the bullet
    m_InFlight = true;
    m_Position = {startX, startY};

    // The displacement vector is d = (dx, dy)
    float dx = xTarget - startX;
    float dy = yTarget - startY;

    // The magnitude of d is |d| = sqrt(dx^2 + dy^2)
    float lenght = sqrt(dx * dx + dy * dy);
    if (lenght == 0.0f) {
        return; // We can't divide by zero can we?
    }

    // Normalize d to get a unit vector in the same direction: 
    // u = (dx, dy) / |d| (u for unit), then the velocity in the u direction
    // is u * m_BulletSpeed = 1 * m_BulletSpeed in the correct direction
    m_BulletDistanceX = (dx / lenght) * m_BulletSpeed;
    m_BulletDistanceY = (dy / lenght) * m_BulletSpeed;

    // Set a max range for the bullet
    float range = 1000;
    m_MinX = startX - range;
    m_MaxX = startX + range;
    m_MinY = startY - range;
    m_MaxY = startY + range;

    // Position the bullet ready to be drawn
    m_BulletShape.setPosition(m_Position);    
}

void Bullet::stop() {
    m_InFlight = false;
}

bool Bullet::isInFlight() {
    return m_InFlight;
}

sf::FloatRect Bullet::getPosition() {
    return m_BulletShape.getGlobalBounds();
}

sf::RectangleShape Bullet::getShape() {
    return m_BulletShape;
}

void Bullet::update(float elapsedTime) {
    // Update the position variables
    m_Position.x += m_BulletDistanceX * elapsedTime;
    m_Position.y += m_BulletDistanceY * elapsedTime;

    // Move the bullet
    m_BulletShape.setPosition(m_Position);

    // Has it gone out of range?
    if (m_Position.x < m_MinX || m_Position.x > m_MaxX ||
        m_Position.y < m_MinY || m_Position.y > m_MaxY) {
        m_InFlight = false;
    }
    
}