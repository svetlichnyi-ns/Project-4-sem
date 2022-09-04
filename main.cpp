#define _USE_MATH_DEFINES

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <SFML/Window.hpp>
#include <string>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <pthread.h>
#include <cstdlib>
#include <cstdint>
#include <chrono>
#include <cmath>
#include <functional>
#include <vector>
#include <stdio.h>
#include <cmath>
#include "decoration.h"
#include "blocks.h"
#include "Buffon.h"
#include "integral.h"
#include "Mandelbrot.h"
#include "scattering.h"
#include "series.h"
#include "spigot.h"
#include "Viete.h"

int main() {
  sf::RenderWindow window(sf::VideoMode(1920, 1080), "Menu");
  window.setFramerateLimit(30);
  // a picture on the background of menu
  sf::RectangleShape background;
  background.setSize(sf::Vector2f(window.getSize()));
  background.setPosition(sf::Vector2f(0.f, 0.f));
  sf::Texture background_texture;
  background_texture.loadFromFile("pictures/background.jpg");
  background.setTexture(&background_texture);

  sf::RectangleShape header;
  header.setSize(sf::Vector2f(1650.f, 150.f));
  header.setFillColor(sf::Color::White);
  header.setOrigin(sf::Vector2f(header.getSize().x / 2.f, header.getSize().y / 2.f));
  header.setPosition(sf::Vector2f((float) window.getSize().x / 2.f, 100.f));

  sf::Font font;
  font.loadFromFile ("fonts/Bitter_Bold_700.ttf");
  sf::Text text;
  text.setFont (font);
  text.setCharacterSize (65);
  text.setFillColor (sf::Color::Red);
  text.setString ("Choose any method of calculation of number PI");
  // so as to align text's and figure's centers
  sf::FloatRect textRect = text.getLocalBounds();
  text.setOrigin(textRect.left + textRect.width / 2.f, textRect.top + textRect.height / 2.f);
  text.setPosition (header.getPosition());
  
  window.draw(background);
  window.draw(header);
  window.draw(text);
  while (window.isOpen()) {
    SetButton (window, sf::Vector2f(500.f, 300.f), "Buffon's needle problem", sf::Color::Black);
    SetButton (window, sf::Vector2f(500.f, 500.f), "Colliding blocks", sf::Color::Black);
    SetButton (window, sf::Vector2f(500.f, 700.f), "Numerical integration", sf::Color::Black);
    SetButton (window, sf::Vector2f(500.f, 900.f), "Series' summation", sf::Color::Black);
    SetButton (window, sf::Vector2f(1350.f, 300.f), "Mandelbrot's set", sf::Color::Black);
    SetButton (window, sf::Vector2f(1350.f, 500.f), "A spigot algorithm", sf::Color::Black);
    SetButton (window, sf::Vector2f(1350.f, 700.f), "Monte-Carlo method", sf::Color::Black);
    SetButton (window, sf::Vector2f(1350.f, 900.f), "Viete's formula", sf::Color::Black);
    window.display();
    sf::Event event;
    while (window.pollEvent(event)) {
      if (event.type == sf::Event::Closed)
        window.close();
      // handling events, associated with the first button (hovering over it, pressing, moving the cursor away)
      while ((sf::Mouse::getPosition(window).x >= 100) && (sf::Mouse::getPosition(window).x <= 900) &&
             (sf::Mouse::getPosition(window).y >= 225) && (sf::Mouse::getPosition(window).y <= 375)) {
          SetButton (window, sf::Vector2f(500.f, 300.f), "Buffon's needle problem", sf::Color::Blue);
          window.display();
          while (window.pollEvent(event)) {
            if (event.type == sf::Event::MouseButtonPressed) {
              if (event.mouseButton.button == sf::Mouse::Left) {
                reference_information("pictures/Buffon.png", sf::Vector2f(846.f, 467.f));
                notification(window);
                if (Buffon_needle() == -1) {  // error handling
                  window.close();
                  return -1;
                }
              }
            }
          }
      }
      SetButton (window, sf::Vector2f(500.f, 300.f), "Buffon's needle problem", sf::Color::Black);
      window.display();
      // handling events, associated with the second button (hovering over it, pressing, moving the cursor away)
      while ((sf::Mouse::getPosition(window).x >= 100) && (sf::Mouse::getPosition(window).x <= 900) &&
             (sf::Mouse::getPosition(window).y >= 425) && (sf::Mouse::getPosition(window).y <= 575)) {
          SetButton (window, sf::Vector2f(500.f, 500.f), "Colliding blocks", sf::Color::Blue);
          window.display();
          while (window.pollEvent(event)) {
            if (event.type == sf::Event::MouseButtonPressed) {
              if (event.mouseButton.button == sf::Mouse::Left) {
                reference_information("pictures/blocks.png", sf::Vector2f(849.f, 321.f));
                notification(window);
                if (blocks() == -1) {  // error handling
                  window.close();
                  return -1;
                }
              }
            }
          }
      }
      SetButton (window, sf::Vector2f(500.f, 500.f), "Colliding blocks", sf::Color::Black);
      window.display();
      // handling events, associated with the third button (hovering over it, pressing, moving the cursor away)
      while ((sf::Mouse::getPosition(window).x >= 100) && (sf::Mouse::getPosition(window).x <= 900) &&
             (sf::Mouse::getPosition(window).y >= 625) && (sf::Mouse::getPosition(window).y <= 775)) {
          SetButton (window, sf::Vector2f(500.f, 700.f), "Numerical integration", sf::Color::Blue);
          window.display();
          while (window.pollEvent(event)) {
            if (event.type == sf::Event::MouseButtonPressed) {
              if (event.mouseButton.button == sf::Mouse::Left) {
                reference_information("pictures/integral.png", sf::Vector2f(834.f, 1011.f));
                notification(window);
                if (integral_computation() == -1) {  // error handling
                  window.close();
                  return -1;
                }  
              }
            }
          }
      }
      SetButton (window, sf::Vector2f(500.f, 700.f), "Numerical integration", sf::Color::Black);
      window.display();
      // handling events, associated with the forth button (hovering over it, pressing, moving the cursor away)
      while ((sf::Mouse::getPosition(window).x >= 100) && (sf::Mouse::getPosition(window).x <= 900) &&
             (sf::Mouse::getPosition(window).y >= 825) && (sf::Mouse::getPosition(window).y <= 975)) {
          SetButton (window, sf::Vector2f(500.f, 900.f), "Series' summation", sf::Color::Blue);
          window.display();
          while (window.pollEvent(event)) {
            if (event.type == sf::Event::MouseButtonPressed) {
              if (event.mouseButton.button == sf::Mouse::Left) {
                reference_information("pictures/series.png", sf::Vector2f(832.f, 792.f));
                notification(window);
                if (series() == -1) {  // error handling
                  window.close();
                  return -1;
                }
              }
            }
          }
      }
      SetButton (window, sf::Vector2f(500.f, 900.f), "Series' summation", sf::Color::Black);
      window.display();
      // handling events, associated with the fifth button (hovering over it, pressing, moving the cursor away)
      while ((sf::Mouse::getPosition(window).x >= 950) && (sf::Mouse::getPosition(window).x <= 1750) &&
             (sf::Mouse::getPosition(window).y >= 225) && (sf::Mouse::getPosition(window).y <= 375)) {
          SetButton (window, sf::Vector2f(1350.f, 300.f), "Mandelbrot's set", sf::Color::Blue);
          window.display();
          while (window.pollEvent(event)) {
            if (event.type == sf::Event::MouseButtonPressed) {
              if (event.mouseButton.button == sf::Mouse::Left) {
                reference_information("pictures/Mandelbrot.png", sf::Vector2f(846.f, 526.f));
                Mandelbrot();   // this function cannot return any errors
                notification(window);
              }
            }
          }
      }
      SetButton (window, sf::Vector2f(1350.f, 300.f), "Mandelbrot's set", sf::Color::Black);
      window.display();
      // handling events, associated with the sixth button (hovering over it, pressing, moving the cursor away)
      while ((sf::Mouse::getPosition(window).x >= 950) && (sf::Mouse::getPosition(window).x <= 1750) &&
             (sf::Mouse::getPosition(window).y >= 425) && (sf::Mouse::getPosition(window).y <= 575)) {
          SetButton (window, sf::Vector2f(1350.f, 500.f), "A spigot algorithm", sf::Color::Blue);
          window.display();
          while (window.pollEvent(event)) {
            if (event.type == sf::Event::MouseButtonPressed) {
              if (event.mouseButton.button == sf::Mouse::Left) {
                reference_information("pictures/spigot.png", sf::Vector2f(819.f, 280.f));
                notification(window);
                if (spigot() == -1) {  // error handling
                  window.close();
                  return -1;
                }
              }
            }
          }
      }
      SetButton (window, sf::Vector2f(1350.f, 500.f), "A spigot algorithm", sf::Color::Black);
      window.display();
      // handling events, associated with the seventh button (hovering over it, pressing, moving the cursor away)
      while ((sf::Mouse::getPosition(window).x >= 950) && (sf::Mouse::getPosition(window).x <= 1750) &&
             (sf::Mouse::getPosition(window).y >= 625) && (sf::Mouse::getPosition(window).y <= 775)) {
          SetButton (window, sf::Vector2f(1350.f, 700.f), "Monte-Carlo method", sf::Color::Blue);
          window.display();
          while (window.pollEvent(event)) {
            if (event.type == sf::Event::MouseButtonPressed) {
              if (event.mouseButton.button == sf::Mouse::Left) {
                reference_information("pictures/Monte_Carlo.png", sf::Vector2f(860.f, 445.f));
                Monte_Carlo();  // this function cannot return any errors
              }
            }
          }
      }
      SetButton (window, sf::Vector2f(1350.f, 700.f), "Monte-Carlo method", sf::Color::Black);
      window.display();
      // handling events, associated with the eighth button (hovering over it, pressing, moving the cursor away)
      while ((sf::Mouse::getPosition(window).x >= 950) && (sf::Mouse::getPosition(window).x <= 1750) &&
             (sf::Mouse::getPosition(window).y >= 825) && (sf::Mouse::getPosition(window).y <= 975)) {
          SetButton (window, sf::Vector2f(1350.f, 900.f), "Viete's formula", sf::Color::Blue);
          window.display();
          while (window.pollEvent(event)) {
            if (event.type == sf::Event::MouseButtonPressed) {
              if (event.mouseButton.button == sf::Mouse::Left) {
                reference_information("pictures/Viete.png", sf::Vector2f(761.f, 154.f));
                notification(window);
                if (Viete() == -1) {  // error handling
                  window.close();
                  return -1;
                }
              }
            }
          }
      }
      SetButton (window, sf::Vector2f(1350.f, 900.f), "Viete's formula", sf::Color::Black);
      window.display();
    }
  }
  return 0;
}