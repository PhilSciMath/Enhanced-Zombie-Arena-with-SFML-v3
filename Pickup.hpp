#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Graphics/Rect.hpp>


class Pickup {

    private:
        // Start value for health pickups
        const int HEALTH_START_VALUE = 50;
        const int AMMO_START_VALUE = 12;
        const int START_WAIT_TIME = 10;
        const int START_SECONCS_TO_LIVE = 5;

        // The sprite that represent this pickup
        sf::Sprite m_Sprite;

        // The pickup is in the arena
        sf::IntRect m_Arena;

        // Pickup's value
        int m_Value;

        // What type it is (1 = ammo, 2 = health)
        int m_Type;

        // Handle spawning and disappearing
        bool m_Spawned;
        float m_SecondsSinceSpawn;
        float m_SecondsSinceDeSpawn;
        float m_SecondsToLive;
        float m_SecondsToWait;

    public:
       
        Pickup(int type);

        // Prepare a new pickup
        void setArena(sf::IntRect arena);

        void spawn();

        // Check the position of the pickup
        sf::FloatRect getPosition();

        // Return the sprite for drawing
        sf::Sprite getSprite();

        // Pickup updates itself each frame
        void update(float elapsedTime);

        // Is it spawned?
        bool isSpawned();

        // Get the pickup
        int gotIt();

        // Upgrade the pickup's value
        void upgrade();

};