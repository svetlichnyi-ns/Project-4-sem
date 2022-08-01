#include <SFML/Graphics.hpp>
#include <math.h>
#include <SFML/Window/Window.hpp>
#include <iostream>

int main() {
    float speed;
    std::cout << "Enter the speed of each block: ";
    std::cin >> speed;
    sf::RenderWindow window(sf::VideoMode(1920, 1080), "Colliding blocks!");
    sf::VertexArray ground(sf::Quads, 4);
    ground[0].position = sf::Vector2f(0, 600);
    ground[1].position = sf::Vector2f(1920, 600);
    ground[2].position = sf::Vector2f(1920, 1080);
    ground[3].position = sf::Vector2f(0, 1080);
    for (int i = 0; i < 4; i++)
        ground[i].color = sf::Color::Black;
    sf::VertexArray wall(sf::Quads, 4);
    wall[0].position = sf::Vector2f(0, 0);
    wall[1].position = sf::Vector2f(300, 0);
    wall[2].position = sf::Vector2f(300, 600);
    wall[3].position = sf::Vector2f(0, 600);
    for (int i = 0; i < 4; i++)
        wall[i].color = sf::Color::Green;
    sf::VertexArray block(sf::Quads, 4);
    for (int i = 0; i < 4; i++)
        block[i].color = sf::Color::Red;
    block[0].position = sf::Vector2f(500, 600);
    block[1].position = sf::Vector2f(500, 500);
    block[2].position = sf::Vector2f(700, 500);
    block[3].position = sf::Vector2f(700, 600);
    sf::Clock clock;
    sf::Time elapsed;
    float power = 5.0;
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }
        window.clear(sf::Color::White);
        window.draw(ground);
        window.draw(wall);
        elapsed = clock.restart();
        //std::cout << "Elapsed time: " << elapsed.asSeconds() << std::endl;
        for (int i = 0; i < 4; i++)
            block[i].position.x = block[i].position.x + pow(-1, power) * speed * elapsed.asSeconds();
        if (block[0].position.x <= 300)
            power++;
        window.draw(block);
        window.display();
    }
    return 0;
}