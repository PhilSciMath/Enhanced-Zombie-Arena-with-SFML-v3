#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <SFML/Graphics/RectangleShape.hpp>


class Bullet {

    private:
        // Bullet's position
        sf::Vector2f m_Position;

        // What each bullet looks like
        sf::RectangleShape m_BulletShape;

        // Is the bullet traveling in air?
        bool m_InFlight = false;

        // Bullet speed
        float m_BulletSpeed = 1000;

        // What fraction of 1 pixel does the bullet travel each frame?
        // These values will be derived from m_BulletSpeed
        float m_BulletDistanceX;
        float m_BulletDistanceY;

        // Boundaries, or it flies forever
        float m_MaxX;
        float m_MinX;
        float m_MaxY;
        float m_MinY;

    public:
        // Constructor
        Bullet();

        // Stop the bullet
        void stop();

        // return the value of m_InFlight
        bool isInFlight();

        // Launch a new bullet
        void shoot(float startX, float startY, float xTarget, float yTarget);

        // Tell the calling code where the bullet is in the world
        sf::FloatRect getPosition();

        // Return the actual shape so we can draw it
        sf::RectangleShape getShape();

        // Update the bullet each frame
        void update(float elapsedTime);
};