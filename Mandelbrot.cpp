#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/System.hpp>
#include <iostream>
#include <iomanip>
#include <cmath>
#include "Mandelbrot.h"

class complex_number {
  public:
    long double real, imaginary;
};

void Mandelbrot_set (sf::VertexArray& vertex_array, int precision, long double zoom, sf::RenderWindow& window) {
  for (int i = 0; i < (int) window.getSize().y; i++) {
    for (int j = 0; j < (int) window.getSize().x; j++) {
      complex_number c;
      // drawing Mandelbrot's set, we move point (0; 0) to the center of the window
      c.real = (long double) (j - (int) window.getSize().x / 2) / zoom;
      c.imaginary = (long double) (i - (int) window.getSize().y / 2) / zoom;
      complex_number z = c;
      int k;
      for (k = 1; k <= precision; k++) {  // the recurrent sequence: z_(n+1) = (z_n)^2 + c
        complex_number tmp;
        tmp.real = z.real * z.real - z.imaginary * z.imaginary + c.real;
        tmp.imaginary = 2 * z.real * z.imaginary + c.imaginary;
        z = tmp;
        if (z.real * z.real + z.imaginary * z.imaginary > 4)
          break;
      }
      // if after a certain number of iterations (called "precision") a sequence number does not exceed 2 (a «bail-out»), it belongs to the set and the corresponding point stays black 
      if (k <= precision) {  // otherwise, it becomes red, green or blue (depending on the iteration number, at which the criterion was fulfilled)
        vertex_array [i * window.getSize().x + j].position = sf::Vector2f((float) j, (float) i);
        if (k <= precision / 4)
          // shades of red (the closer to the boundary of the set, the brighter)
          vertex_array [i * window.getSize().x + j].color = sf::Color ((sf::Uint8) (k * 255 / (precision / 4)), 0, 0);
        else if (k <= precision / 2)
          // shades of green (likewise)
          vertex_array [i * window.getSize().x + j].color = sf::Color (0, (sf::Uint8) (k * 255 / (precision / 2)), 0);
        else if (k <= precision)
          // shades of blue (likewise)
          vertex_array [i * window.getSize().x + j].color = sf::Color (0, 0, (sf::Uint8) (k * 255 / precision));
      }
    }
  }
}

void Mandelbrot_iterations(long double eps) {
  long double constanta = 0.25l + eps;
  long double z = constanta;
  int NumOfIterations;  // the number of iterations in a recurrent sequence z_(n+1) = (z_n)^2 + c, happened before its member exceeded 2
  sf::Clock clock;  // start the timer
  sf::Time elapsed;
  for (NumOfIterations = 1; z < 2.l; NumOfIterations++) {
    long double tmp = z;
    z = powl(tmp, 2.l) + constanta;
  }
  elapsed = clock.restart();  // stop the timer
  int NumOfExperiment = (int) (-log10l(eps) / 2.l);  // knowing epsilon, we can restore an experiment number
  std::cout << "Epsilon = " << std::setprecision(1 + 2 * NumOfExperiment) << eps << "; number of iterations: " << NumOfIterations << ";\n";
  std::cout << "Number PI in its " << NumOfExperiment << "th approximation: ";
  // experiment №NumOfExperiment enables to precisely gain first NumOfExperiment digits of PI
  std::cout << std::setprecision(NumOfExperiment) << (NumOfIterations / 10) / powl(10.l, NumOfExperiment - 1) << ";\n";
  std::cout << "Elapsed time: " << elapsed.asMicroseconds() / 1'000'000.l << " seconds.\n\n";
  return;
}

int Mandelbrot() {
  sf::RenderWindow window (sf::VideoMode(1920, 1080), "Mandelbrot's set");
  sf::VertexArray pointmap (sf::Points, window.getSize().x * window.getSize().y);
  
  int precision = 250;  // the maximum number of iterations for a given construction:
  // if after it "infinity criterion" has not been reached yet, we conclude that this point belongs to Mandelbrot's set; otherwise - it doesn't belong to the set
  long double zoom = 500.l;  // it is applied so as to illustrate this set on the screen in a full scale

  Mandelbrot_set (pointmap, precision, zoom, window);

  window.draw(pointmap);
  window.display();
  while (window.isOpen()) {
    sf::Event event;
    while (window.pollEvent(event)) {
      if (event.type == sf::Event::Closed)
        window.close();
    }
  }
  // organize seven experiments at different values of epsilon
  for (int j = 1; j < 8; j++) {
    long double eps = powl(100.l, -j);
    Mandelbrot_iterations(eps);
  }
  return 0;
}