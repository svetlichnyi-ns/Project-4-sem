#include <SFML/Window.hpp>
#include <SFML/System.hpp>
#include <SFML/Graphics.hpp>
#include <math.h>
#include <sstream>

using namespace sf;
using namespace std;

int main() {
  RenderWindow window(VideoMode(1920, 1080), "Scattering of points");
  window.setFramerateLimit(40);  // this frame rate provides both optimal perception and rather high computational performance

  RectangleShape square(Vector2f(800.f, 800.f));  // creating a square
  square.setOrigin(Vector2f(square.getSize().x / 2, square.getSize().y / 2));
  square.setPosition(Vector2f(window.getSize().x / 2, window.getSize().y / 2));
  square.setOutlineThickness(10.f);
  square.setOutlineColor(Color::Yellow);
  window.draw(square);

  RectangleShape patch(Vector2f(window.getSize().x, 80.f));  // creating a black patch at the top of the screen: without it, previous inscriptions would overlap each other
  patch.setPosition(Vector2f(0.f, 0.f));
  patch.setFillColor(Color::Black);

  CircleShape circle;  // creating a circle
  circle.setOrigin(Vector2f(square.getSize().x / 2, square.getSize().y / 2));
  circle.setPosition(Vector2f(window.getSize().x / 2, window.getSize().y / 2));
  circle.setOutlineThickness(2.f);
  circle.setRadius(square.getSize().x / 2 - circle.getOutlineThickness());
  circle.setOutlineColor(Color::Black);
  circle.setPointCount(10000000);  // number of triangles, forming a circle, which is close to an "ideal" one
  window.draw(circle);

  int iterations = 0;  // the total number of points, depicted on the screen

  Font font;
  font.loadFromFile("fonts/a_Futurica_ExtraBold.ttf");

  Text text;
  text.setFont(font);
  text.setCharacterSize(50);
  text.setFillColor(Color::White);
  text.setPosition(Vector2f(350.f, 15.f));

  int blue_points = 0;  // number of points, depicted inside the circle
  
  while (window.isOpen()) {
    Event event;
    while (window.pollEvent(event)) {
      if (event.type == Event::Closed)
        window.close();
    }

    CircleShape point(2.f, 100000);  // we actually draw not points, but tiny circles, so that they are seen for us
    point.setOrigin(point.getRadius(), point.getRadius());
    point.setPosition(Vector2f((float) (window.getSize().x / 2) - square.getSize().x / 2 + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / square.getSize().x)),
                              (float) (window.getSize().y / 2) - square.getSize().y / 2 + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / square.getSize().y))));
    if (pow(square.getPosition().x - point.getPosition().x, 2.f) + pow(square.getPosition().y - point.getPosition().y, 2.f) <= pow(circle.getRadius() + circle.getOutlineThickness(), 2.f)) {
      point.setFillColor(Color::Blue);  // if the point occurs inside the circle or on its border, it becomes blue
      blue_points++;
    }
    else
      point.setFillColor(Color::Red); // if the point occurs outside the circle, it becomes red

    iterations++;
    ostringstream num_of_iterations;
    num_of_iterations << iterations;
    ostringstream pi;
    pi << 4 * (long double) blue_points / iterations; // number pi is calculated via the probability for the point not to occur outside the circle
    text.setString ("Number of iterations: " + num_of_iterations.str() + "; number pi = " + pi.str());
    window.draw(point);
    window.draw(text);
    window.display();
    window.draw(patch);
  }
  return 0;
}