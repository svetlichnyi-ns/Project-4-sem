#ifndef MANDELBROT_H
#define MANDELBROT_H
#include <SFML/Window.hpp>
#include <SFML/System.hpp>
#include <SFML/Graphics.hpp>

class complex_number {
  public:
    long double real, imaginary;
};

// this function is responsible for drawing Mandelbrot's set
void Mandelbrot_set (sf::VertexArray& vertex_array, int precision, long double zoom, sf::RenderWindow& window);
// this function implements numerical algorithm of PI's computation
void Mandelbrot_iterations(long double eps);
// this function unites opportunities of two previous ones and launches experiments
int Mandelbrot();

#endif