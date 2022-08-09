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
      c.real = (long double) ((j - (int) (window.getSize().x / 2)) / zoom);  // drawing Mandelbrot's set, we move point (0; 0) to the center of the window
      c.imaginary = (long double) ((i - (int) (window.getSize().y / 2)) / zoom);
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
      // if after a certain number of iterations (called "precision") a sequence number did not exceed 2 (a «bail-out»), it belongs to the set and the corresponding point stays black 
      if (k <= precision) {  // otherwise, it becomes red, green or blue (depending on the iteration number, at which the criterion was fulfilled)
        vertex_array [i * window.getSize().x + j].position = Vector2f(j, i);
        if (k <= precision / 4)
          vertex_array [i * window.getSize().x + j].color = Color (k * 255 / (precision / 4), 0, 0);  // shades of red (the closer to the boundary of the set, the brighter)
        else if (k <= precision / 2)
          vertex_array [i * window.getSize().x + j].color = Color (0, k * 255 / (precision / 2), 0); // shades of green (likewise)
        else if (k <= precision)
          vertex_array [i * window.getSize().x + j].color = Color (0, 0, k * 255 / precision); // shades of blue (likewise)
      }
    }
  }
}

int main() {
  RenderWindow window (VideoMode(1920, 1080), "Mandelbrot's set");
  window.setFramerateLimit (60);
  VertexArray pointmap (Points, window.getSize().x * window.getSize().y);

  int precision = 100;  // the maximum number of iterations for a given construction:
  // if after it "infinity criterion" has not been reached yet, we conclude that this point belongs to Mandelbrot's set; otherwise - it doesn't belong to the set
  float zoom = 500.f;  // it is applied so as to illustrate this set on the screen in a full scale

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