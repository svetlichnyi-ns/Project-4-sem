#include <SFML/System.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <iostream>
#include <math.h>

using namespace std;
using namespace sf;

Vector2f elastic_collision (Vector2f initial_velocities, Vector2f masses) {
  Vector2f final_velocities;
  final_velocities.x = (2*masses.y*initial_velocities.y+(masses.x-masses.y)*initial_velocities.x) / (masses.x+masses.y);
  final_velocities.y = (2*masses.x*initial_velocities.x+(masses.y-masses.x)*initial_velocities.y) / (masses.x+masses.y);
  return final_velocities;
}

int main() {
  float speed_1 = 0.f, speed_2 = -200.f;

  float mass_1, mass_2;
  cout << "Enter masses of two blocks: ";
  cin >> mass_1 >> mass_2;

  RenderWindow window (VideoMode(1920, 1080), "Colliding blocks!");
  window.setFramerateLimit (60);
  
  RectangleShape ground;
  ground.setSize (Vector2f(window.getSize().x, 0.5f * window.getSize().y));
  ground.setPosition (Vector2f(0.0f, window.getSize().y - ground.getSize().y));
  
  Texture ground_texture;
  ground_texture.loadFromFile ("pictures/ground.jpg");
  ground.setTexture (&ground_texture);
  
  RectangleShape sky;
  sky.setSize (Vector2f(window.getSize().x, 0.5f * window.getSize().y));
  sky.setPosition (Vector2f(0.0f, 0.0f));
  
  Texture sky_texture;
  sky_texture.loadFromFile ("pictures/sky.jpg");
  sky.setTexture (&sky_texture);
  
  RectangleShape wall;
  wall.setSize (Vector2f(0.3f * window.getSize().x, window.getSize().y - ground.getSize().y));
  wall.setPosition (Vector2f(0.0f, 0.0f));
  
  Texture wall_texture;
  wall_texture.loadFromFile ("pictures/brick.jpg");
  wall.setTexture (&wall_texture);

  RectangleShape block_1;
  block_1.setSize (Vector2f(150.f, 150.f));
  block_1.setFillColor (Color::Red);
  block_1.setOrigin (Vector2f(block_1.getSize().x / 2, block_1.getSize().y / 2));
  block_1.setPosition (Vector2f(1000.f, wall.getSize().y - block_1.getSize().y / 2));
  
  RectangleShape block_2;
  block_2.setSize (Vector2f(200.f, 200.f));
  block_2.setFillColor (Color::Yellow);
  block_2.setOrigin (Vector2f(block_2.getSize().x / 2, block_2.getSize().y / 2));
  block_2.setPosition (Vector2f(1500.f, wall.getSize().y - block_2.getSize().y / 2));
  
  Clock clock;

  while (window.isOpen()) {
    
    Event event;
    while (window.pollEvent(event)) {
      if (event.type == Event::Closed)
        window.close();
      }
    window.clear();
    window.draw(ground);
    window.draw(sky);
    window.draw(wall);

    block_1.move (Vector2f(speed_1 * clock.getElapsedTime().asSeconds(), 0.0f));
    block_2.move (Vector2f(speed_2 * clock.restart().asSeconds(), 0.0f));
    if ((float) (fabs((double) (block_1.getPosition().x - block_2.getPosition().x)) <= (block_1.getSize().x + block_2.getSize().x) / 2)) {
      float prev_speed_1 = speed_1;
      speed_1 = elastic_collision (Vector2f(speed_1, speed_2), Vector2f(mass_1, mass_2)).x;
      speed_2 = elastic_collision (Vector2f(prev_speed_1, speed_2), Vector2f(mass_1, mass_2)).y;
      block_1.move (Vector2f(speed_1 * clock.getElapsedTime().asSeconds(), 0.0f));
      block_2.move (Vector2f(speed_2 * clock.restart().asSeconds(), 0.0f));
    }
    if (block_1.getPosition().x - block_1.getSize().x / 2 <= wall.getSize().x) {
      speed_1 = -speed_1;
      block_1.move (Vector2f(speed_1 * clock.getElapsedTime().asSeconds(), 0.0f));
    }
    window.draw (block_1);
    window.draw (block_2);
    window.display();
  }
  return 0;
}