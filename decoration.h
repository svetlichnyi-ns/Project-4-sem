#ifndef DECORATION_H
#define DECORATION_H
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <SFML/Window.hpp>

// this function draws a button with an inscription on it
void SetButton (sf::RenderWindow& window, sf::Vector2f button_position, std::string button_content, sf::Color text_color);
// an auxiliary function that displays a notification (about the necessity to look at the console) on the screen
void notification(sf::RenderWindow& window);
// an auxiliary function that displays reference information on the screen
void reference_information(const std::string &filename, sf::Vector2f size);

#endif