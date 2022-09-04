#ifndef BLOCKS_H
#define BLOCKS_H
#include <SFML/System.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>

sf::Vector2f elastic_collision (sf::Vector2f initial_velocities, sf::Vector2i masses);
int blocks();

#endif