#include <SFML/System.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <assert.h>
#include <cmath>

// this function calculates blocks' velocities after their absolutely elastic collision
sf::Vector2f elastic_collision (sf::Vector2f initial_velocities, sf::Vector2i masses) {
  sf::Vector2f final_velocities;
  // these formulas are the result of solving a system of equations, consisting of the laws of conservation of energy and momentum
  final_velocities.x = (2 * (float) masses.y * initial_velocities.y + (float) (masses.x - masses.y) * initial_velocities.x) / (float) (masses.x + masses.y);
  final_velocities.y = (2 * (float) masses.x * initial_velocities.x + (float) (masses.y - masses.x) * initial_velocities.y) / (float) (masses.x + masses.y);
  return final_velocities;
}

int main() {
  std::cout << "Before entering masses, consider that the second block's mass should exceed the first block's mass by 1, 100, 10000 or 1000000 times.\n";
  
  int mass_1;
  std::cout << "Enter the mass of the first block: ";
  std::cin >> mass_1;
  assert(mass_1 > 0 && "The mass must be strictly positive");
  
  int mass_2;
  std::cout << "Enter the mass of the second block: ";
  std::cin >> mass_2;
  assert(mass_2 > 0 && "The mass must be strictly positive");

  float speed_1 = 0.f, speed_2;  // at the beginning, the left block is motionless, whereas the right block moves towards the left one

  sf::RenderWindow window (sf::VideoMode(1920, 1080), "Colliding blocks!");
  window.setFramerateLimit (60);

  sf::RectangleShape ground;
  ground.setSize (sf::Vector2f((float) window.getSize().x, 0.5f * (float) window.getSize().y));
  // set the position of the upper left corner of 'ground'
  ground.setPosition (sf::Vector2f(0.f, (float) window.getSize().y - ground.getSize().y));
  sf::Texture ground_texture;
  ground_texture.loadFromFile ("pictures/ground.jpg");
  ground.setTexture (&ground_texture);
  
  sf::RectangleShape sky;
  sky.setSize (sf::Vector2f((float) window.getSize().x, (float) window.getSize().y - ground.getSize().y));
  // set the position of the upper left corner of 'sky'
  sky.setPosition (sf::Vector2f(0.f, 0.f));
  sf::Texture sky_texture;
  sky_texture.loadFromFile ("pictures/sky.jpg");
  sky.setTexture (&sky_texture);
  
  sf::RectangleShape wall;
  wall.setSize (sf::Vector2f(0.3f * (float) window.getSize().x, (float) window.getSize().y - ground.getSize().y));
  // set the position of the upper left corner of 'wall'
  wall.setPosition (sf::Vector2f(0.f, 0.f));
  sf::Texture wall_texture;
  wall_texture.loadFromFile ("pictures/brick.jpg");
  wall.setTexture (&wall_texture);

  sf::RectangleShape block_1;
  block_1.setSize (sf::Vector2f(150.f, 150.f));
  block_1.setFillColor (sf::Color::Red);
  // now the first block's origin is located in its center
  block_1.setOrigin (sf::Vector2f(block_1.getSize().x / 2.f, block_1.getSize().y / 2.f));
  
  sf::RectangleShape block_2;
  block_2.setSize (sf::Vector2f(250.f, 250.f));
  block_2.setFillColor (sf::Color::Yellow);
  // now the second block's origin is located in its center
  block_2.setOrigin (sf::Vector2f(block_2.getSize().x / 2.f, block_2.getSize().y / 2.f));

  // we adjust the experiment's parameters so that the demonstration is the best in each concrete case
  switch (mass_2 / mass_1) {
    case 1:
      speed_2 = -200.f;
      block_1.setPosition (sf::Vector2f(800.f, wall.getSize().y - block_1.getSize().y / 2.f));
      block_2.setPosition (sf::Vector2f(1200.f, wall.getSize().y - block_2.getSize().y / 2.f));
      break;
    case 100:
      speed_2 = -20.f;
      block_1.setPosition (sf::Vector2f(700.f, wall.getSize().y - block_1.getSize().y / 2.f));
      block_2.setPosition (sf::Vector2f(1000.f, wall.getSize().y - block_2.getSize().y / 2.f));
      break;
    case 10000:
      speed_2 = -2.f;
      block_1.setPosition (sf::Vector2f(700.f, wall.getSize().y - block_1.getSize().y / 2.f));
      block_2.setPosition (sf::Vector2f(950.f, wall.getSize().y - block_2.getSize().y / 2.f));
      break;
    case 1000000:
      speed_2 = -0.1f;
      block_1.setPosition (sf::Vector2f(650.f, wall.getSize().y - block_1.getSize().y / 2.f));
      block_2.setPosition (sf::Vector2f(860.f, wall.getSize().y - block_2.getSize().y / 2.f));
      break;
    default:
      std::cerr << "Please, choose one of possible ratios between the masses\n";
      return -1;
  }

  sf::Font font;
  font.loadFromFile ("fonts/Times_New_Roman.ttf");

  sf::Text text;
  text.setFont (font);
  text.setCharacterSize (100);
  text.setFillColor (sf::Color::Black);
  text.setPosition (sf::Vector2f(700.f, 50.f));

  std::ostringstream initial_inscription;
  initial_inscription << 0;
  text.setString ("Number of collisions: " + initial_inscription.str());

  sf::Clock clock;  // start the timer
  int collisions = 0;  // total number of collisions
  while (window.isOpen()) {
    sf::Event event;
    while (window.pollEvent(event)) {
      if (event.type == sf::Event::Closed)
        window.close();
    }
    window.clear();
    window.draw (ground);
    window.draw (sky);
    window.draw (wall);
    window.draw (text);
    window.draw (block_1);
    window.draw (block_2);
    window.display();
    // at each iteration, we move the blocks, based on their current velocities
    block_1.move (sf::Vector2f(speed_1 * clock.getElapsedTime().asSeconds(), 0.f));
    block_2.move (sf::Vector2f(speed_2 * clock.restart().asSeconds(), 0.f));
    // checking the collision condition of two blocks
    if (fabs(block_1.getPosition().x - block_2.getPosition().x) <= (block_1.getSize().x + block_2.getSize().x) / 2.f) {
      collisions++;  // increment variable 'collisions'
      float prev_speed_1 = speed_1;  // memorize the previous speed of the first block in a temporary variable
      // recalculate blocks' velocities
      speed_1 = elastic_collision (sf::Vector2f(speed_1, speed_2), sf::Vector2i(mass_1, mass_2)).x;
      speed_2 = elastic_collision (sf::Vector2f(prev_speed_1, speed_2), sf::Vector2i(mass_1, mass_2)).y;
      do {  // here we move blocks with recalculated velocities, until the collision criterion is no longer met
        window.clear();
        window.draw(ground);
        window.draw(sky);
        window.draw(wall);
        std::ostringstream num_of_collisions;
        num_of_collisions << collisions;
        text.setString ("Number of collisions: " + num_of_collisions.str());
        window.draw (text);
        window.draw (block_1);
        window.draw (block_2);
        block_1.move (sf::Vector2f(speed_1 * clock.getElapsedTime().asSeconds(), 0.f));
        block_2.move (sf::Vector2f(speed_2 * clock.restart().asSeconds(), 0.f));
        window.display();
      } while (fabs(block_1.getPosition().x - block_2.getPosition().x) <= (block_1.getSize().x + block_2.getSize().x) / 2.f);
    }
    // checking the collision condition of the first block with the wall
    if (block_1.getPosition().x - block_1.getSize().x / 2.f <= wall.getSize().x) {
      collisions++;  // increment variable 'collisions'
      speed_1 = -speed_1;  // recalculate the speed of the first block
      do {  // here we move both blocks, until the collision condition is no longer met
        window.clear();
        window.draw(ground);
        window.draw(sky);
        window.draw(wall);
        std::ostringstream num_of_collisions;
        num_of_collisions << collisions;
        text.setString ("Number of collisions: " + num_of_collisions.str());
        window.draw(text);
        window.draw(block_1);
        window.draw(block_2);
        block_1.move (sf::Vector2f(speed_1 * clock.getElapsedTime().asSeconds(), 0.f));
        block_2.move (sf::Vector2f(speed_2 * clock.restart().asSeconds(), 0.f));
        window.display();
      } while (block_1.getPosition().x - block_1.getSize().x / 2.f <= wall.getSize().x);
    }
  }
  std::cout << "Collisions: " << collisions << std::endl;
  std::cout << "Number PI in its " << (int) log10(mass_2 / mass_1) / 2 + 1 << "th approximation is equal to: ";
  std::cout << std::setprecision((int) log10(mass_2 / mass_1) / 2 + 1) << (float) collisions / sqrtf((float) (mass_2 / mass_1)) << std::endl;
  return 0;
}