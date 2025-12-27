#pragma once
#include <SFML/Graphics/Rect.hpp>
#include <SFML/Graphics/VertexArray.hpp>
#include "Zombie.hpp"


int createBackground(sf::VertexArray& rVA, sf::IntRect arena);
Zombie* createHorde(int numZombies, sf::IntRect arena);
bool circlesIntersect(sf::Vector2f pos1, sf::Vector2f pos2);