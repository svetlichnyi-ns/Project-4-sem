#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <SFML/Window.hpp>
#include <iostream>
#include <iomanip>
#include <cstdlib>
#include <cstdio>
#include <cmath>

int main() {
  sf::RenderWindow window;

  float width;
  std::cout << "Enter the interval between neighbouring lines: ";
  std::cin >> width;
  int N_of_lines = static_cast<int>(1080.f / width) + 1;  // calculate the number of lines, based on the distance between adjacent ones

  sf::VertexArray lines(sf::Lines, 2*N_of_lines);  // create an array of vertices, forming lines
  for (int i = 0; i < N_of_lines; i++) {  // draw lines on a plane
    lines[2*i].position = sf::Vector2f(0, width*i);
    lines[2*i + 1].position = sf::Vector2f(1920.f, width*i);
  }
  for (int i = 0; i < 2*N_of_lines; i++)
    lines[i].color = sf::Color::Blue;

  int N_of_needles;
  std::cout << "Enter the number of needles you want to scatter: ";
  std::cin >> N_of_needles;

  float length;
  std::cout << "Enter the length of each needle: ";
  std::cin >> length;

  sf::VertexArray needles(sf::Lines, 2*N_of_needles);  // create an array of vertices, forming needles
  float* angles = (float*) malloc (N_of_needles * sizeof(float));  // an array of angles between the direction, given by a particular needle, and the horizontal (between 0 and 360 degrees)
  if (angles == NULL) {
    std::perror("Failed to allocate memory via malloc()");
    return -1;
  }
  float* X = (float*) malloc (N_of_needles * sizeof(float));  // an array of X-coordinates of a defined needle's end (with an even index)
  if (X == NULL) {
    std::perror("Failed to allocate memory via malloc()");
    return -1;
  }
  float* Y = (float*) malloc (N_of_needles * sizeof(float));  // an array of Y-coordinates of a defined needle's end (with an even index)
  if (Y == NULL) {
    std::perror("Failed to allocate memory via malloc()");
    return -1;
  }

  int count = 0;  // the number of intersections of needles and lines
  for (int j = 0; j < N_of_needles; j++) {
    angles[j] = static_cast<float> (rand()) / (RAND_MAX / 2.f / 3.14159265358979f);  // randomly choose an angle for each thrown needle
    X[j] = static_cast<float> (rand()) / (RAND_MAX / 1920.f);  // randomly choose an abscissa for the "left end" of each thrown needle
    Y[j] = static_cast<float> (rand()) / (RAND_MAX / 1080.f);  // randomly choose an ordinate for the "left end" of each thrown needle
    needles[2*j].position = sf::Vector2f(X[j], Y[j]);
    needles[2*j].color = sf::Color::Black;
    needles[2*j + 1].position = sf::Vector2f(X[j] + length * std::cos(angles[j]), Y[j] + length * std::sin(angles[j]));  // the position of the "right end" of a needle is determined by the position of its "left end", the needle's length and its angle
    needles[2*j + 1].color = sf::Color::Black;
    // if a thrown needle crosses any line, it becomes red; otherwise, it stays black
    for (int i = 0; i < N_of_lines; i++)
      if (((width*i > needles[2*j].position.y) && (width*i < needles[2*j + 1].position.y)) || ((width*i < needles[2*j].position.y) && (width*i > needles[2*j + 1].position.y))) {
        needles[2*j].color = sf::Color::Red;
        needles[2*j + 1].color = sf::Color::Red;
        count++;
      }
    }
    std::cout.precision(10);
    std::cout << "Number PI is: " << (float) 2*length/width/count*N_of_needles << std::endl;

    window.create(sf::VideoMode(1920, 1080), "Buffon's needle");
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
    // free an allocated memory
    free(angles);
    free(X);
    free(Y);
    return 0;
}