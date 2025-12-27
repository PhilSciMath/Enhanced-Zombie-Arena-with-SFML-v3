#include "Zombie.hpp"
#include "TextureHolder.hpp"
#include <SFML/Graphics/Rect.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/System/Angle.hpp>
#include <random>
#include <cmath>


// SFML 3: Sprite has no default constructor, so we need to build one
// and it needs a Texture, the placeholder texture helps with this.
// Do not forget the "true" value when assigning the correct texture later.
sf::Texture placeholder("graphics/zombie_crawler.png");

Zombie::Zombie() :
    m_Sprite(placeholder)
{
    m_Alive = true;
}

void Zombie::spawn(float startX, float startY, int type) {
    
    switch (type) {
        case 0:
            // Bloater
            m_Sprite.setTexture(TextureHolder::GetTexture(
                "graphics/zombie_bloater.png"), true);
        
            m_Speed = BLOATER_SPEED;
            m_Health = BLOATER_HEALTH;
            break;

        case 1:
            // Chaser
            m_Sprite.setTexture(TextureHolder::GetTexture(
                "graphics/zombie_chaser.png"), true);
            
            m_Speed = CHASER_SPEED;
            m_Health = CHASER_HEALTH;
            break;
        
        case 2:
            // Crawler
            m_Sprite.setTexture(TextureHolder::GetTexture(
                "graphics/zombie_crawler.png"), true);
            
            m_Speed = CRAWLER_SPEED;
            m_Health = CRAWLER_HEALTH;
            break;
    }

    // Modify the speed to make each zombie unique  
    std::mt19937 rng(std::random_device{}());  
    std::uniform_int_distribution<> factor(7, 10);
    float modifier = factor(rng);
    modifier /= 10; // it can be .7, .8, .9 or 1 of the base speed
    m_Speed *= modifier;

    // Initialize its location
    m_Position = {startX, startY };

    // Scale it a bit
    m_Sprite.setScale({.1f, .1f});

    // Set its origin to its center
    m_Sprite.setOrigin({m_Sprite.getLocalBounds().size.x / 2,
        m_Sprite.getLocalBounds().size.y / 2 });

    // Set its position
    m_Sprite.setPosition(m_Position);
}

bool Zombie::hit() {
    m_Health--;

    if (m_Health < 0) {
        // Dead
        m_Alive = false;
        m_Sprite.setTexture(TextureHolder::GetTexture(
            "graphics/blood.png"));
        
        return true;
    }

    // Injuried but not dead yet
    return false;
}

bool Zombie::isAlive() {
    return m_Alive;
}

sf::FloatRect Zombie::getPosition() {
    return m_Sprite.getGlobalBounds();
}

const sf::Sprite& Zombie::getSprite() const {
    return m_Sprite;
}

void Zombie::update(float elapsedTime, sf::Vector2f playerLocation) {
    float playerX = playerLocation.x;
    float playerY = playerLocation.y;

    // Update the zombie position variables
    if (m_Position.x < playerX) {
        m_Position.x = m_Position.x + m_Speed * elapsedTime;
    }

    if (m_Position.x > playerX) {
        m_Position.x = m_Position.x - m_Speed * elapsedTime;
    }

    if (m_Position.y < playerY) {
        m_Position.y = m_Position.y + m_Speed * elapsedTime;
    }

    if (m_Position.y > playerY) {
        m_Position.y = m_Position.y - m_Speed * elapsedTime;
    }    

    // Face the zombie in the correct direction
    sf::Vector2f delta = {playerX - m_Position.x, playerY - m_Position.y};
    float angle = (atan2(delta.y, delta.x) * 180) / 3.14159;
    m_Sprite.setRotation(sf::degrees(angle + 90)); 

    // Move the zombie
    m_Sprite.setPosition(m_Position);
}