#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/System/Vector2.hpp>


class Player {

private:
    const float START_SPEED = 200;
    const float START_HEALTH = 100;

    // Player position
    sf::Vector2f m_Position;

    // Sprite and texture
    sf::Sprite m_Sprite;
    sf::Texture m_Texture;

    // Store screen resolution, arena size and tile size
    sf::Vector2f m_Resolution;
    sf::IntRect m_Arena;
    int m_TileSize;

    // Movement directions
    bool m_UpPressed;
    bool m_DownPressed;
    bool m_LeftPressed;
    bool m_RightPressed;

    // Stats
    int m_Health;
    int m_MaxHealth;
    float m_Speed;

    // Last time hit
    sf::Time m_LastHit;

public:

    Player();

    void spawn(sf::IntRect arena, sf::Vector2f resolution, int tileSize);

    void resetPlayerStats(); // Refresh stats for new game

    bool hit(sf::Time timeHit); // Handle player being hit by a zombie

    sf::Time getLastHitTime(); // Time since last hit

    sf::FloatRect getPosition(); // Gets player position

    sf::Vector2f getCenter(); // Returns the center of the player sprite

    float getRotation(); // Angle the player is facing

    const sf::Sprite& getSprite() const; // Returns sprite by reference

    // Movement functions
    void moveLeft();
    void moveRight();
    void moveUp();
    void moveDown();
    void stopLeft();
    void stopRight();
    void stopUp();
    void stopDown();

    // Update player, called every frame
    void update(float elapsedTime, sf::Vector2i mousePosition);

    // Player upgrades
    void upgradeSpeed();
    void upgradeHealth();
    void increaseHealthLevel(int amount);
    int getHealth(); // Health the player currently has
   
};