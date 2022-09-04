#ifndef MANDELBROT_H
#define MANDELBROT_H
#include <SFML/Window.hpp>
#include <SFML/System.hpp>
#include <SFML/Graphics.hpp>

class complex_number {
  public:
    long double real, imaginary;
};

void Mandelbrot_set (sf::VertexArray& vertex_array, int precision, long double zoom, sf::RenderWindow& window);
void Mandelbrot_iterations(long double eps);
int Mandelbrot();

#endif