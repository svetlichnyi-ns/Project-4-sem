#include <SFML/Window.hpp>
#include <SFML/System.hpp>
#include <SFML/Graphics.hpp>
#include <cmath>
#include <chrono>
#include <random>
#include <sstream>
#include "scattering.h"

int Monte_Carlo() {
  sf::RenderWindow window(sf::VideoMode(1920, 1080), "Scattering of points");
  window.setFramerateLimit(40);  // this frame rate provides both optimal perception and rather high computational performance

  sf::RectangleShape square(sf::Vector2f(800.f, 800.f));  // creating a square
  square.setOrigin(sf::Vector2f(square.getSize().x / 2, square.getSize().y / 2));
  square.setPosition(sf::Vector2f(static_cast<float>(window.getSize().x / 2), static_cast<float>(window.getSize().y / 2)));
  square.setOutlineThickness(10.f);
  square.setOutlineColor(sf::Color::Yellow);
  window.draw(square);

  sf::RectangleShape patch(sf::Vector2f(static_cast<float>(window.getSize().x), 80.f));  // creating a black patch at the top of the screen: without it, previous inscriptions would overlap each other
  patch.setPosition(sf::Vector2f(0.f, 0.f));
  patch.setFillColor(sf::Color::Black);

  sf::CircleShape circle;  // creating a circle
  circle.setOrigin(sf::Vector2f(square.getSize().x / 2, square.getSize().y / 2));
  circle.setPosition(sf::Vector2f(static_cast<float>(window.getSize().x / 2), static_cast<float>(window.getSize().y / 2)));
  circle.setOutlineThickness(2.f);
  circle.setRadius(square.getSize().x / 2 - circle.getOutlineThickness());
  circle.setOutlineColor(sf::Color::Black);
  circle.setPointCount(10000000);  // number of triangles, forming a circle, which is close to an "ideal" one
  window.draw(circle);

  int iterations = 0;  // the total number of points, depicted on the screen

  sf::Font font;
  font.loadFromFile("fonts/a_Futurica_ExtraBold.ttf");

  sf::Text text;
  text.setFont(font);
  text.setCharacterSize(50);
  text.setFillColor(sf::Color::White);
  text.setPosition(sf::Vector2f(350.f, 15.f));

  int blue_points = 0;  // number of points, depicted inside the circle
  // prepare a random number generator
  std::random_device rd;
  std::default_random_engine eng(rd());
  std::uniform_real_distribution<> distr(0, 1);

  while (window.isOpen()) {
    sf::Event event;
    while (window.pollEvent(event)) {
      if (event.type == sf::Event::Closed)
        window.close();
    }

    sf::CircleShape point(2.f, 100000);  // we actually draw not points, but tiny circles, so that they are seen for us
    point.setOrigin(point.getRadius(), point.getRadius());
    point.setPosition(sf::Vector2f((float) (window.getSize().x / 2) - square.getSize().x / 2 + square.getSize().x * (float) distr(eng),
                                   (float) (window.getSize().y / 2) - square.getSize().y / 2 + square.getSize().y * (float) distr(eng)));
    if (powf(square.getPosition().x - point.getPosition().x, 2.f) + powf(square.getPosition().y - point.getPosition().y, 2.f) <= powf(circle.getRadius() + circle.getOutlineThickness(), 2.f)) {
      point.setFillColor(sf::Color::Blue);  // if the point occurs inside the circle or on its border, it becomes blue
      blue_points++;
    }
    else
      point.setFillColor(sf::Color::Red); // if the point occurs outside the circle, it becomes red

    iterations++;
    std::ostringstream num_of_iterations;
    num_of_iterations << iterations;
    std::ostringstream pi;
    pi << 4 * (long double) blue_points / iterations; // number pi is calculated via the probability for the point not to occur outside the circle
    text.setString ("Number of points: " + num_of_iterations.str() + "; number PI = " + pi.str());
    window.draw(point);
    window.draw(text);
    window.display();
    window.draw(patch);
  }
  return 0;
}