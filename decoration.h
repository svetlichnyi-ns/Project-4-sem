#ifndef DECORATION_H
#define DECORATION_H
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <SFML/Window.hpp>

void SetButton (sf::RenderWindow& window, sf::Vector2f button_position, std::string button_content, sf::Color text_color);
void notification(sf::RenderWindow& window);
void reference_information(const std::string &filename, sf::Vector2f size);

#endif