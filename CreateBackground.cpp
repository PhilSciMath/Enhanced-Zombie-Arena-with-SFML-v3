#include "ZombieArena.hpp"
#include <SFML/Graphics/PrimitiveType.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <SFML/Graphics/VertexArray.hpp>
#include <random>


int createBackground(sf::VertexArray& rVA, sf::IntRect arena) {
    
    // Size of tile/texture
    const int TILE_SIZE = 100;
    const int TILE_TYPES = 3; 
    const int VERTICES_NUM = 6; 

    int worldWidth = (arena.position.x + arena.size.x) / TILE_SIZE;
    int worldHeight = (arena.position.y + arena.size.y) / TILE_SIZE;

    // Using triangles to build rectangles
    rVA.setPrimitiveType(sf::PrimitiveType::Triangles);

    // Size of the VertexArray
    rVA.resize(worldWidth * worldHeight * VERTICES_NUM);    

    // Required for randomizing the floors   
    std::mt19937 rng(std::random_device{}()); 
    std::uniform_int_distribution<> floorChoices(1,6);

    // Start at the beginning of the VertexArray
    int currentVertex = 0;

    for (int w = 0; w < worldWidth; w++) {
        for (int h = 0; h < worldHeight; h++) {
            // Adjusting each vertex position
            // Triangle 1: top-left, bottom-left, top-right
            rVA[currentVertex + 0].position = 
                sf::Vector2f(w * TILE_SIZE, h * TILE_SIZE);
            rVA[currentVertex + 1].position = 
                sf::Vector2f(w * TILE_SIZE, (h * TILE_SIZE) + TILE_SIZE);
            rVA[currentVertex + 2].position =
                sf::Vector2f((w * TILE_SIZE) + TILE_SIZE, h * TILE_SIZE);
            
            // Triangle 2: top-right, bottom-left, bottom-right
            rVA[currentVertex + 3].position = 
                sf::Vector2f((w * TILE_SIZE) + TILE_SIZE, h * TILE_SIZE);
            rVA[currentVertex + 4].position = 
                sf::Vector2f(w * TILE_SIZE, (h * TILE_SIZE) + TILE_SIZE);
            rVA[currentVertex + 5].position = 
                sf::Vector2f((w * TILE_SIZE) + TILE_SIZE, (h * TILE_SIZE)
                + TILE_SIZE);
            
            // Adjusting the texture
            // If the rectangle is at the arena's boundary, use the wall texture
            if (h == 0 || h == worldHeight-1 || w == 0 || w == worldWidth-1) 
            {                
                rVA[currentVertex + 0].texCoords = 
                    sf::Vector2f(0, 0);
                rVA[currentVertex + 1].texCoords = 
                    sf::Vector2f(0, TILE_SIZE);
                rVA[currentVertex + 2].texCoords = 
                    sf::Vector2f(TILE_SIZE, 0);

                rVA[currentVertex + 3].texCoords = 
                    sf::Vector2f(TILE_SIZE, 0);
                rVA[currentVertex + 4].texCoords = 
                    sf::Vector2f(0, TILE_SIZE);
                rVA[currentVertex + 5].texCoords = 
                    sf::Vector2f(TILE_SIZE, TILE_SIZE);
            }
            // If the rectangle is not at the boundary, use a floor texture, a 
            // random one from the three options
            else {
                int roll = floorChoices(rng);
                int floor = 0;
                if (roll <= 4) // Adding a little bias towards floor 1
                    floor = 1;
                else if (roll == 5)
                    floor = 2;
                else
                    floor = 3;

                int verticalOffset = floor * TILE_SIZE;

                rVA[currentVertex + 0].texCoords = 
                    sf::Vector2f(0, verticalOffset);
                rVA[currentVertex + 1].texCoords = 
                    sf::Vector2f(0, verticalOffset + TILE_SIZE);
                rVA[currentVertex + 2].texCoords = 
                    sf::Vector2f(TILE_SIZE, verticalOffset);

                rVA[currentVertex + 3].texCoords = 
                    sf::Vector2f(TILE_SIZE, verticalOffset);
                rVA[currentVertex + 4].texCoords = 
                    sf::Vector2f(0, verticalOffset + TILE_SIZE);
                rVA[currentVertex + 5].texCoords = 
                    sf::Vector2f(TILE_SIZE, verticalOffset + TILE_SIZE);
            }
            
            // Prepare position for the next vertex
            currentVertex = currentVertex + VERTICES_NUM;
        }
    }

    return TILE_SIZE;
}