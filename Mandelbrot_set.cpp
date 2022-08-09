#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/System.hpp>

using namespace sf;

class complex_number {
  public:
    long double real, imaginary;
};

void Mandelbrot_set (VertexArray& vertex_array, int precision, float zoom, RenderWindow& window) {
  for (int i = 0; i < window.getSize().y; i++) {
    for (int j = 0; j < window.getSize().x; j++) {
      complex_number c;
      c.real = (long double) ((j - (int) (window.getSize().x / 2)) / zoom);
      c.imaginary = (long double) ((i - (int) (window.getSize().y / 2)) / zoom);
      complex_number z = c;
      int k;
      for (k = 1; k <= precision; k++) {
        complex_number tmp;
        tmp.real = z.real * z.real - z.imaginary * z.imaginary + c.real;
        tmp.imaginary = 2 * z.real * z.imaginary + c.imaginary;
        z = tmp;
        if (z.real * z.real + z.imaginary * z.imaginary > 4)
          break;
      }

      if (k < precision / 4) {
        vertex_array [i * window.getSize().x + j].position = Vector2f(j, i);
        Color color (k * 255 / (precision / 4), 0, 0);
        vertex_array [i * window.getSize().x + j].color = color;
      }
      else if (k < precision / 2) {
        vertex_array [i * window.getSize().x + j].position = Vector2f(j, i);
        Color color (0, k * 255 / (precision / 2), 0);
        vertex_array [i * window.getSize().x + j].color = color;
      }
      else if (k < precision) {
        vertex_array [i * window.getSize().x + j].position = Vector2f(j, i);
        Color color (0, 0, k * 255 / precision);
        vertex_array [i * window.getSize().x + j].color = color;
      }
    }
  }
}

int main() {
  RenderWindow window (VideoMode(1920, 1080), "Mandelbrot's set");
  window.setFramerateLimit (60);
  VertexArray pointmap (Points, window.getSize().x * window.getSize().y);

  int precision = 100;
  float zoom = 500.f;

  Mandelbrot_set (pointmap, precision, zoom, window);

  while (window.isOpen()) {
    Event event;
    while (window.pollEvent(event)) {
      if (event.type == Event::Closed)
        window.close();
    }
    window.clear();
    window.draw(pointmap);
    window.display();
  }
  return 0;
}