#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Graphics/Texture.hpp>


class Zombie {

    private:
        // How fast is each zombie type?
        const float BLOATER_SPEED = 40;
        const float CHASER_SPEED = 80;
        const float CRAWLER_SPEED = 20;
    
        // How tough is each zombie type
        const float BLOATER_HEALTH = 5;
        const float CHASER_HEALTH = 1;
        const float CRAWLER_HEALTH = 3;

        // Where is this zombie?
        sf::Vector2f m_Position;

        // A sprite for this zombie
        sf::Sprite m_Sprite;

        // How fast can this one move?
        float m_Speed;

        // How much health has it got?
        float m_Health;

        // Is it still alive?
        bool m_Alive;

    public:   
        Zombie();        
        // Handle when a bullet hits a zombie
        bool hit();

        // Find out if the zombie is still alive
        bool isAlive();

        // Spawn a new zombie
        void spawn(float startX, float startY, int type);

        // Return a rectangle that is the position in the world
        sf::FloatRect getPosition();

        // Get the sprite to draw
        const sf::Sprite& getSprite() const;

        // Update the zombie each frame
        void update(float elapsedTime, sf::Vector2f playerLocation);
};