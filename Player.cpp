#include "Player.hpp"
#include "TextureHolder.hpp"
#include <SFML/Graphics/Rect.hpp>
#include <SFML/System/Angle.hpp>
#include <SFML/System/Vector2.hpp>
#include <cmath>


Player::Player()
    : m_Texture() // placeholder needed for constructor
    , m_Sprite(m_Texture)
{
    // Handling the texture 
    m_Sprite.setTexture(TextureHolder::GetTexture(
        "graphics/player.png"),true);

    // Set the origin 
    sf::FloatRect bounds = m_Sprite.getLocalBounds();
    m_Sprite.setOrigin(bounds.size / 2.0f);  

    // Scale down if the player is too big
    m_Sprite.setScale({0.12f, 0.12f});

    m_Speed = START_SPEED;
    m_Health = START_HEALTH;
    m_MaxHealth = START_HEALTH;    
}

void Player::spawn(sf::IntRect arena, sf::Vector2f resolution, int tileSize) {
    // Place the player in the middle of the arena
    m_Position.x = arena.position.x + arena.size.x / 2.0f;
    m_Position.y = arena.position.y + arena.size.y / 2.0f;
    
    // Copy details of arena, resolution and tile size
    m_Arena.size = arena.size;
    m_Arena.position = arena.position;
    m_TileSize = tileSize;
    m_Resolution = resolution;
}

void Player::resetPlayerStats() {
    m_Speed = START_SPEED;
    m_Health = START_HEALTH;
    m_MaxHealth = START_HEALTH;
}

sf::Time Player::getLastHitTime() {
    return m_LastHit;
}

bool Player::hit(sf::Time timeHit) {
    if (timeHit.asMilliseconds() - m_LastHit.asMilliseconds() > 200) {
        m_LastHit = timeHit;
        m_Health -= 10;
        return true;
    }
    else {
        return false;
    }
}

sf::FloatRect Player::getPosition() {
    return m_Sprite.getGlobalBounds();
}

sf::Vector2f Player::getCenter() {
    return m_Position;
}

float Player::getRotation() {
    return m_Sprite.getRotation().asDegrees();
}

const sf::Sprite& Player::getSprite() const {
    return m_Sprite;
}

int Player::getHealth() {
    return m_Health;
}

// Movement functions
void Player::moveLeft() {
    m_LeftPressed = true;
}

void Player::moveRight() {
    m_RightPressed = true;
}

void Player::moveUp() {
    m_UpPressed = true;
}

void Player::moveDown() {
    m_DownPressed = true;
}

void Player::stopLeft() {
    m_LeftPressed = false;
}

void Player::stopRight() {
    m_RightPressed = false;
}

void Player::stopUp() {
    m_UpPressed = false;
}

void Player::stopDown() {
    m_DownPressed = false;
}

// Update player
void Player::update(float elapsedTime, sf::Vector2i mousePosition) {
    if (m_UpPressed) {
        m_Position.y -= m_Speed * elapsedTime;
    }

    if (m_DownPressed) {
        m_Position.y += m_Speed * elapsedTime;
    }

    if (m_LeftPressed) {
        m_Position.x -= m_Speed * elapsedTime;
    }

    if (m_RightPressed) {
        m_Position.x += m_Speed * elapsedTime;
    }    

    // Keep the player within the arena bounds, used m_TileSize instead of
    // padding before, didn't work very well.    
    float padding = 150;    
    float leftBound = m_Arena.position.x + padding;
    float rightBound = m_Arena.position.x + m_Arena.size.x - padding;
    float topBound = m_Arena.position.y + padding;
    float bottomBound = m_Arena.position.y + m_Arena.size.y - padding;

    if (m_Position.x < leftBound) {
        m_Position.x = leftBound;
    }
    if (m_Position.x > rightBound) {
        m_Position.x = rightBound;
    }
    if (m_Position.y < topBound) {
        m_Position.y = topBound;
    }
    if (m_Position.y > bottomBound) {
        m_Position.y = bottomBound;
    }

    // Set sprite position
    m_Sprite.setPosition(m_Position);

    // Calculate rotation 
    sf::Vector2f delta;
    delta.x = mousePosition.x - m_Position.x;
    delta.y = mousePosition.y - m_Position.y;
    float angle = std::atan2(delta.y, delta.x) * 180 / 3.14159;     
    m_Sprite.setRotation(sf::degrees(angle + 86));
}

void Player::upgradeSpeed() {
    m_Speed += (START_SPEED * 0.2); // An increase of 20%
}

void Player::upgradeHealth() {
    m_Health += (START_HEALTH * 0.2);
}

void Player::increaseHealthLevel(int amount) {
    m_Health += amount;

    // But not beyone maximum
    if (m_Health > m_MaxHealth) {
        m_Health = m_MaxHealth;
    }
}
