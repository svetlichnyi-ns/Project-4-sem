#include <SFML/Window.hpp>
#include <SFML/System.hpp>
#include <SFML/Graphics.hpp>
#include <stdint.h>
#include <math.h>
#include <sstream>

using namespace sf;
using namespace std;

int main() {
  RenderWindow window(VideoMode(1920, 1080), "Scattering of points");
  window.setFramerateLimit(60);

  RectangleShape square(Vector2f(800.f, 800.f));
  square.setOrigin(Vector2f(square.getSize().x / 2, square.getSize().y / 2));
  square.setPosition(Vector2f(window.getSize().x / 2, window.getSize().y / 2));
  square.setOutlineThickness(10.f);
  square.setOutlineColor(Color::Yellow);

  CircleShape circle;
  circle.setOrigin(Vector2f(square.getSize().x / 2, square.getSize().y / 2));
  circle.setPosition(Vector2f(window.getSize().x / 2, window.getSize().y / 2));
  circle.setOutlineThickness(2.f);
  circle.setRadius(square.getSize().x / 2 - circle.getOutlineThickness());
  circle.setOutlineColor(Color::Black);
  circle.setPointCount(10000000);

  VertexArray points(Points, 10000000);

  vector<CircleShape> vector_of_points;
  long double iterations = 0.0;

  Font font;
  font.loadFromFile("fonts/a_Futurica_ExtraBold.ttf");

  Text text;
  text.setFont (font);
  text.setCharacterSize (50);
  text.setFillColor (Color::White);
  text.setPosition (Vector2f(350.f, 15.f));

  long double blue_points = 0.0;

  while (window.isOpen()) {
    Event event;
    while (window.pollEvent(event)) {
      if (event.type == Event::Closed)
        window.close();
    }

    window.clear();
    window.draw(square);
    window.draw(circle);

    #pragma omp parallel for
    for (int i = iterations; i < iterations + 10; i++) {
      points[i].position = Vector2f((float) (window.getSize().x / 2) - square.getSize().x / 2 + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / square.getSize().x)),
                                    (float) (window.getSize().y / 2) - square.getSize().y / 2 + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / square.getSize().y)));
      if (pow(square.getPosition().x - points[i].position.x, 2.f) + pow(square.getPosition().y - points[i].position.y, 2.f) <= pow(circle.getRadius() + circle.getOutlineThickness(), 2.f)) {
        points[i].color = Color::Blue;
        blue_points++;
      }
      else
        points[i].color = Color::Red;
      CircleShape point(2.f, 100000);
      point.setFillColor(points[i].color);
      point.setOrigin(point.getRadius(), point.getRadius());
      point.setPosition(points[i].position.x, points[i].position.y);
      vector_of_points.push_back(point);
        
    }
    iterations = iterations + 10.0;
    ostringstream num_of_iterations;
    num_of_iterations << iterations;
    ostringstream pi;
    pi << (long double) (4 * blue_points / iterations);
    text.setString ("Number of iterations: " + num_of_iterations.str() + "; number pi = " + pi.str());
    window.draw(text);
    for (int i = 0; i < vector_of_points.size(); i++)
      window.draw(vector_of_points[i]);
    window.display();
  }
  return 0;
}