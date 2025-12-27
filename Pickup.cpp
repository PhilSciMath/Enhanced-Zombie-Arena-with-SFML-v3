#include "Pickup.hpp"
#include "TextureHolder.hpp"
#include <SFML/Graphics/Rect.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <random>

// Because Sprite needs a constructor and thus some texture.
sf::Texture placeHolder("graphics/ammo.png");

Pickup::Pickup(int type) :    
    m_Sprite(placeHolder)
{
    
    // Store the type
    m_Type = type;

    // Associate a texture
    if (m_Type == 1) {
        m_Sprite.setTexture(
            TextureHolder::GetTexture("graphics/health.png"), true);

        // Value of the pickup
        m_Value = HEALTH_START_VALUE;
    }
    else {
        m_Sprite.setTexture(
            TextureHolder::GetTexture("graphics/ammo.png"), true);

        m_Value = AMMO_START_VALUE;
    }

    sf::Vector2f size = m_Sprite.getLocalBounds().size;
    m_Sprite.setScale({.1, .1});
    m_Sprite.setOrigin(size / 2.0f);

    m_SecondsToLive = START_SECONCS_TO_LIVE;
    m_SecondsToWait = START_WAIT_TIME;
}

void Pickup::setArena(sf::IntRect arena) {
    
    // Copy the details of the arena to m_Arena
    int padding = 140;
    m_Arena.position.x = arena.position.x + padding;
    m_Arena.position.y = arena.position.y + padding;
    m_Arena.size.x = arena.size.x - padding;
    m_Arena.size.y = arena.size.y - padding;

    spawn();
}

void Pickup::spawn() {

    // Spawn at a random location
    std::mt19937 rng(std::random_device{}());
    std::uniform_int_distribution<> 
        xRange(m_Arena.position.x, m_Arena.size.x);
    std::uniform_int_distribution<> 
        yRange(m_Arena.position.y, m_Arena.size.y);

    int x = xRange(rng);
    int y = yRange(rng);

    m_SecondsSinceSpawn = 0;
    m_Spawned = true;

    m_Sprite.setPosition({(float) x, (float) y});
}

sf::FloatRect Pickup::getPosition() {
    return m_Sprite.getGlobalBounds();
}

sf::Sprite Pickup::getSprite() {
    return m_Sprite;
}

bool Pickup::isSpawned() {
    return m_Spawned;
}

int Pickup::gotIt() {
    m_Spawned = false;
    m_SecondsSinceDeSpawn = 0;
    return m_Value;
}

void Pickup::update(float elapsedTime) {

    if (m_Spawned) {
        m_SecondsSinceSpawn += elapsedTime;
    }
    else {
        m_SecondsSinceDeSpawn += elapsedTime;
    }

    // Do we need to hide a pickup?
    if (m_SecondsSinceSpawn > m_SecondsToLive && m_Spawned) {
        // Remove the pickup from the arena, put it somewhere else
        m_Spawned = false;
        m_SecondsSinceDeSpawn = 0;
    }

    // Do we need to spawn a pickup?
    if (m_SecondsSinceDeSpawn > m_SecondsToWait && !m_Spawned) {
        spawn();        
    }
}

void Pickup::upgrade() {

    // How much to upgrade
    const float valueUp = .5;
    const float extraTime = 0.1f;

    // This upgrades the value of each pickup    
    if (m_Type == 1) {
        m_Value += (HEALTH_START_VALUE * valueUp);
    }
    else {
        m_Value += (AMMO_START_VALUE * valueUp);
    }

    // Make them more frequent and last longer    
    m_SecondsToLive += (START_SECONCS_TO_LIVE * extraTime);
    m_SecondsToWait -= (START_WAIT_TIME * extraTime);
}

