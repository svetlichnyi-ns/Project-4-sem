#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <SFML/Window.hpp>
#include <assert.h>
#include <iostream>
#include <iomanip>
#include <chrono>
#include <cmath>

int main() {
  float width;
  std::cout << "Enter the interval between neighbouring lines: ";
  std::cin >> width;
  assert(width > 0 && "The interval between neighbouring lines must be positive");

  int NumOfNeedles;
  std::cout << "Enter the number of needles you want to scatter: ";
  std::cin >> NumOfNeedles;
  assert(NumOfNeedles > 0 && "The number of needles must be positive");

  float length;
  std::cout << "Enter the length of each needle: ";
  std::cin >> length;
  assert(length > 0 && "The length of each needle must be positive");

  std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();  // start the timer
  // calculate the number of lines, based on the distance between adjacent ones
  int NumOfLines = static_cast<int>(1080.f / width) + 1;

  sf::VertexArray lines(sf::Lines, 2*NumOfLines);  // create an array of vertices, forming lines
  for (int i = 0; i < NumOfLines; i++) {  // draw lines on a plane
    lines[2*i].position = sf::Vector2f(0.f, (float) i * width);
    lines[2*i].color = sf::Color::Blue;
    lines[2*i + 1].position = sf::Vector2f(1920.f, (float) i * width);
    lines[2*i + 1].color = sf::Color::Blue;
  }

  sf::VertexArray needles(sf::Lines, 2*NumOfNeedles);  // create an array of vertices, forming needles

  int count = 0;  // the number of intersections of needles and lines
  for (int j = 0; j < NumOfNeedles; j++) {
    // randomly choose an angle for each thrown needle
    float angle = 2.f * 3.14159265358979f * static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
    // randomly choose an abscissa for the "left end" of each thrown needle
    float x = 1920.f * static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
    // randomly choose an ordinate for the "left end" of each thrown needle
    float y = 1080.f * static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
    needles[2*j].position = sf::Vector2f(x, y);
    needles[2*j].color = sf::Color::Black;
    // the position of the "right end" of a needle is determined by the position of its "left end", the needle's length and its angle
    needles[2*j + 1].position = sf::Vector2f(x + length * std::cos(angle), y + length * std::sin(angle));
    needles[2*j + 1].color = sf::Color::Black;
    // if a thrown needle crosses any line, it becomes red; otherwise, it stays black
    for (int i = 0; i < NumOfLines; i++) {
      if ((((float) i * width > needles[2*j].position.y) && ((float) i * width < needles[2*j + 1].position.y)) ||
          (((float) i * width < needles[2*j].position.y) && ((float) i * width > needles[2*j + 1].position.y)))
      {
        needles[2*j].color = sf::Color::Red;
        needles[2*j + 1].color = sf::Color::Red;
        count++;
      }
    }
  }

  std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();  // stop the timer
  // calculate number pi, based on the probability for each needle to cross any line and on current geometrical parameters
  std::cout << "Number PI is: " << std::setprecision(15) << 2.f * length / width / (float) count * (float) NumOfNeedles;
  std::cout << "; it took " << (std::chrono::duration_cast<std::chrono::nanoseconds>(end - begin).count()) / 1'000'000'000.l << " seconds to calculate it.\n";

  sf::RenderWindow window(sf::VideoMode(1920, 1080), "Buffon's needle");
  while (window.isOpen()) {
    sf::Event event;
    while (window.pollEvent(event)) {
      if (event.type == sf::Event::Closed)
        window.close();
    }
    window.clear(sf::Color::White);
    window.draw(lines);
    window.draw(needles);
    window.display();
  }
  return 0;
}