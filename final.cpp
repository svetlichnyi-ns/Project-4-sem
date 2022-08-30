#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <SFML/Window.hpp>
#include <string>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <pthread.h>
#include <assert.h>
#include <cstdlib>
#include <cstdint>
#include <chrono>
#include <cmath>
#include <functional>
#include <vector>
#include <stdio.h>
#include <omp.h>
#include <cmath>
#include "final.h"

#define PI_25_DIGITS 3.141592653589793238462643

int main() {
  sf::RenderWindow window(sf::VideoMode(1920, 1080), "Multiple windows");
  window.setFramerateLimit(30);

  sf::RectangleShape background;
  background.setSize (sf::Vector2f((float) window.getSize().x, (float) window.getSize().y));
  background.setPosition(sf::Vector2f(0.f, 0.f));
  sf::Texture background_texture;
  background_texture.loadFromFile ("pictures/background.jpg");
  background.setTexture (&background_texture);

  sf::RectangleShape header;
  header.setSize(sf::Vector2f(1650.f, 150.f));
  header.setFillColor(sf::Color::White);
  header.setOrigin(sf::Vector2f(header.getSize().x / 2.f, header.getSize().y / 2.f));
  header.setPosition(sf::Vector2f((float) window.getSize().x / 2.f, 100.f));

  sf::Font font;
  font.loadFromFile ("fonts/Times_New_Roman.ttf");
  sf::Text text;
  text.setFont (font);
  text.setCharacterSize (85);
  text.setFillColor (sf::Color::Red);
  text.setString ("Choose any method of calculation of number PI");
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
    SetButton (window, sf::Vector2f(500.f, 900.f), "Series summation", sf::Color::Black);
    SetButton (window, sf::Vector2f(1350.f, 300.f), "Mandelbrot's set", sf::Color::Black);
    SetButton (window, sf::Vector2f(1350.f, 500.f), "A spigot algorithm", sf::Color::Black);
    SetButton (window, sf::Vector2f(1350.f, 700.f), "Monte-Carlo method", sf::Color::Black);
    SetButton (window, sf::Vector2f(1350.f, 900.f), "Viete's formula", sf::Color::Black);
    window.display();
    sf::Event event;
    while (window.pollEvent(event)) {
      if (event.type == sf::Event::Closed) {
        window.close();
      }
      while ((sf::Mouse::getPosition(window).x >= 100) && (sf::Mouse::getPosition(window).x <= 900) &&
          (sf::Mouse::getPosition(window).y >= 225) && (sf::Mouse::getPosition(window).y <= 375)) {
          SetButton (window, sf::Vector2f(500.f, 300.f), "Buffon's needle problem", sf::Color::Blue);
          window.display();
          sf::Event event_scattering;
          while (window.pollEvent(event_scattering)) {
            if (event_scattering.type == sf::Event::MouseButtonPressed) {
              if (event_scattering.mouseButton.button == sf::Mouse::Left) {
                notification(window);
                Buffon_needle();
              }
            }
          }
      }
      SetButton (window, sf::Vector2f(500.f, 300.f), "Buffon's needle problem", sf::Color::Black);
      window.display();
      while ((sf::Mouse::getPosition(window).x >= 100) && (sf::Mouse::getPosition(window).x <= 900) &&
          (sf::Mouse::getPosition(window).y >= 425) && (sf::Mouse::getPosition(window).y <= 575)) {
          SetButton (window, sf::Vector2f(500.f, 500.f), "Colliding blocks", sf::Color::Blue);
          window.display();
          sf::Event event_blocks;
          while (window.pollEvent(event_blocks)) {
            if (event_blocks.type == sf::Event::MouseButtonPressed) {
              if (event_blocks.mouseButton.button == sf::Mouse::Left) {
                notification(window);
                blocks();
              }
            }
          }
      }
      SetButton (window, sf::Vector2f(500.f, 500.f), "Colliding blocks", sf::Color::Black);
      window.display();
      while ((sf::Mouse::getPosition(window).x >= 100) && (sf::Mouse::getPosition(window).x <= 900) &&
          (sf::Mouse::getPosition(window).y >= 625) && (sf::Mouse::getPosition(window).y <= 775)) {
          SetButton (window, sf::Vector2f(500.f, 700.f), "Numerical integration", sf::Color::Blue);
          window.display();
          sf::Event event_integral;
          while (window.pollEvent(event_integral)) {
            if (event_integral.type == sf::Event::MouseButtonPressed) {
              if (event_integral.mouseButton.button == sf::Mouse::Left) {
                notification(window);
                integral_computation();
              }
            }
          }
      }
      SetButton (window, sf::Vector2f(500.f, 700.f), "Numerical integration", sf::Color::Black);
      window.display();
      while ((sf::Mouse::getPosition(window).x >= 100) && (sf::Mouse::getPosition(window).x <= 900) &&
          (sf::Mouse::getPosition(window).y >= 825) && (sf::Mouse::getPosition(window).y <= 975)) {
          SetButton (window, sf::Vector2f(500.f, 900.f), "Series summation", sf::Color::Blue);
          window.display();
          sf::Event event_series;
          while (window.pollEvent(event_series)) {
            if (event_series.type == sf::Event::MouseButtonPressed) {
              if (event_series.mouseButton.button == sf::Mouse::Left) {
                notification(window);
                series();
              }
            }
          }
      }
      SetButton (window, sf::Vector2f(500.f, 900.f), "Series summation", sf::Color::Black);
      window.display();
      while ((sf::Mouse::getPosition(window).x >= 950) && (sf::Mouse::getPosition(window).x <= 1750) &&
          (sf::Mouse::getPosition(window).y >= 225) && (sf::Mouse::getPosition(window).y <= 375)) {
          SetButton (window, sf::Vector2f(1350.f, 300.f), "Mandelbrot's set", sf::Color::Blue);
          window.display();
          sf::Event event_Mandelbrot;
          while (window.pollEvent(event_Mandelbrot)) {
            if (event_Mandelbrot.type == sf::Event::MouseButtonPressed) {
              if (event_Mandelbrot.mouseButton.button == sf::Mouse::Left) {
                Mandelbrot();
                notification(window);
              }
            }
          }
      }
      SetButton (window, sf::Vector2f(1350.f, 300.f), "Mandelbrot's set", sf::Color::Black);
      window.display();
      while ((sf::Mouse::getPosition(window).x >= 950) && (sf::Mouse::getPosition(window).x <= 1750) &&
          (sf::Mouse::getPosition(window).y >= 425) && (sf::Mouse::getPosition(window).y <= 575)) {
          SetButton (window, sf::Vector2f(1350.f, 500.f), "A spigot algorithm", sf::Color::Blue);
          window.display();
          sf::Event event_spigot;
          while (window.pollEvent(event_spigot)) {
            if (event_spigot.type == sf::Event::MouseButtonPressed) {
              if (event_spigot.mouseButton.button == sf::Mouse::Left) {
                notification(window);
                spigot();
              }
            }
          }
      }
      SetButton (window, sf::Vector2f(1350.f, 500.f), "A spigot algorithm", sf::Color::Black);
      window.display();
      while ((sf::Mouse::getPosition(window).x >= 950) && (sf::Mouse::getPosition(window).x <= 1750) &&
          (sf::Mouse::getPosition(window).y >= 625) && (sf::Mouse::getPosition(window).y <= 775)) {
          SetButton (window, sf::Vector2f(1350.f, 700.f), "Monte-Carlo method", sf::Color::Blue);
          window.display();
          sf::Event event_scattering;
          while (window.pollEvent(event_scattering)) {
            if (event_scattering.type == sf::Event::MouseButtonPressed) {
              if (event_scattering.mouseButton.button == sf::Mouse::Left) {
                Monte_Carlo();
              }
            }
          }
      }
      SetButton (window, sf::Vector2f(1350.f, 700.f), "Monte-Carlo method", sf::Color::Black);
      window.display();
      while ((sf::Mouse::getPosition(window).x >= 950) && (sf::Mouse::getPosition(window).x <= 1750) &&
          (sf::Mouse::getPosition(window).y >= 825) && (sf::Mouse::getPosition(window).y <= 975)) {
          SetButton (window, sf::Vector2f(1350.f, 900.f), "Viete's formula", sf::Color::Blue);
          window.display();
          sf::Event event_Viete;
          while (window.pollEvent(event_Viete)) {
            if (event_Viete.type == sf::Event::MouseButtonPressed) {
              if (event_Viete.mouseButton.button == sf::Mouse::Left) {
                notification(window);
                Viete();
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