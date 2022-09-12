#ifndef BLOCKS_H
#define BLOCKS_H
#include <SFML/System.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>

// this function calculates blocks' velocities after their absolutely elastic collision
sf::Vector2f elastic_collision (sf::Vector2f initial_velocities, sf::Vector2i masses);
// a main function that is responsible for graphical interpretation, interaction with user, counting PI and conclusion of results
int blocks();

#endif