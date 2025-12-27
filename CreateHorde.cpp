#include "ZombieArena.hpp"
#include "Zombie.hpp"
#include <SFML/Graphics/Rect.hpp>
#include <random>


Zombie* createHorde(int numZombies, sf::IntRect arena) {

    std::mt19937 rng(std::random_device{}());
    
    // Dynamically allocated on the heap, will persist after function
    Zombie* zombies = new Zombie[numZombies];

    const int OFFSET = 130; // No zombies spawning at the top of a wall
    int maxY = arena.size.y - OFFSET;
    int minY = arena.position.y + OFFSET;
    int maxX = arena.size.x - OFFSET;
    int minX = arena.position.x + OFFSET;

    std::uniform_int_distribution<> sides(0, 3);
    std::uniform_int_distribution<> yRange(minY, maxY);
    std::uniform_int_distribution<> xRange(minX, maxX);
    std::uniform_int_distribution<> zombieTypes(0, 2);

    for (int i = 0; i < numZombies; i++) {
        // On which side should the zombie spawn?
        int side = sides(rng);
        float x, y;

        switch (side) {
            case 0:
                // left
                x = minX;
                y = yRange(rng);
                break;
            case 1:
                // right
                x = maxX;
                y = yRange(rng);
                break;
            case 2:
                // top
                x = xRange(rng);
                y = minY;
                break;
            case 3:
                // bottom
                x = xRange(rng);
                y = maxY;
                break;
        }

        // Zombie type (bloater, crawler, chaser)
        int type = zombieTypes(rng);

        // Spawn the new zombie
        zombies[i].spawn(x,y, type);
    }
    
    return zombies;
}