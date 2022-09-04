#define _USE_MATH_DEFINES

#include <SFML/Window.hpp>
#include <SFML/System.hpp>
#include <SFML/Graphics.hpp>
#include <iostream>
#include <iomanip>
#include <pthread.h>
#include <cmath>
#include <assert.h>
#include <chrono>
#include "decoration.h"

// this function draws a button with an inscription on it
void SetButton (sf::RenderWindow& window, sf::Vector2f button_position, std::string button_content, sf::Color text_color) {
  sf::RectangleShape button;
  button.setSize(sf::Vector2f(800.f, 150.f));
  button.setFillColor(sf::Color::White);
  button.setOrigin(sf::Vector2f(button.getSize().x / 2.f, button.getSize().y / 2.f));
  button.setPosition(button_position);
  
  sf::Font font;
  font.loadFromFile ("fonts/Bitter_Bold_700.ttf");

  sf::Text text;
  text.setFont (font);
  text.setCharacterSize (65);
  text.setFillColor (text_color);
  text.setString ((sf::String) button_content);
  // so as to centralize the text on the button
  sf::FloatRect textRect = text.getLocalBounds();
  text.setOrigin(textRect.left + textRect.width / 2.f, textRect.top + textRect.height / 2.f);
  text.setPosition (button_position);

  window.draw(button);
  window.draw(text);
  return;
}

// an auxiliary function that displays a notification (about the necessity to look at the console) on the screen
void notification(sf::RenderWindow& window) {
  sf::RenderWindow window_notification(sf::VideoMode(1920, 1080), "Notification");
  window_notification.clear(sf::Color::White);

  sf::Font font;
  font.loadFromFile ("fonts/Bitter_Bold_700.ttf");

  sf::Text text;
  text.setFont (font);
  text.setCharacterSize (100);
  text.setFillColor (sf::Color::Red);
  text.setString ("Look at the console, please");
  sf::FloatRect textRect = text.getLocalBounds();
  text.setOrigin(textRect.left + textRect.width / 2.f, textRect.top + textRect.height / 2.f);
  text.setPosition(sf::Vector2f((float) window.getSize().x / 2.f, (float) window.getSize().y / 2.f));
  window_notification.draw(text);
  window_notification.display();
  while (window_notification.isOpen()) {
    sf::Event event;
    while (window_notification.pollEvent(event)) {
      if (event.type == sf::Event::Closed)
        window_notification.close();
    }
  }
  return;
}

// an auxiliary function that displays reference information on the screen
void reference_information(const std::string &filename, sf::Vector2f size) {
  sf::RenderWindow window_info(sf::VideoMode((unsigned int) size.x, (unsigned int) size.y), "Reference information");
  
  sf::RectangleShape info;
  info.setSize(size);
  sf::Texture info_texture;
  info_texture.loadFromFile(filename);
  info.setTexture(&info_texture);
  
  window_info.draw(info);
  window_info.display();
  // so as to continue using a program, a user should close a window with reference information first
  while (window_info.isOpen()) {
    sf::Event event;
    while (window_info.pollEvent(event)) {
      if (event.type == sf::Event::Closed)
        window_info.close();
    }
  }
  return;
}