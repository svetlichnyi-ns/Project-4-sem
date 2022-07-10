#include <SFML/Graphics.hpp>
#include <iostream>
#include <math.h>

int main() {
    sf::RenderWindow window;
    int width;
    std::cout << "Enter the interval between neighbouring lines: ";
    std::cin >> width;
    int N_of_lines = 1080 / width - 1080 % width + 1;
    sf::VertexArray lines(sf::Lines, 2*N_of_lines);
    for (int i = 0; i < N_of_lines; i++) {
        lines[2*i].position = sf::Vector2f(0, width*i);
        lines[2*i].color = sf::Color::Blue;
        lines[2*i + 1].position = sf::Vector2f(1920, width*i);
        lines[2*i + 1].color = sf::Color::Blue;
    }
    int N_of_needles;
    std::cout << "Enter the number of needles you want to scatter: ";
    std::cin >> N_of_needles;
    int length;
    std::cout << "Enter the length of each needle: ";
    std::cin >> length;
    sf::VertexArray needles(sf::Lines, 2*N_of_needles);
    float *angles, *X, *Y;
    int count = 0;
    angles = (float*) malloc (N_of_needles * sizeof(float));
    X = (float*) malloc (N_of_needles * sizeof(float));
    Y = (float*) malloc (N_of_needles * sizeof(float));
    for (int j = 0; j < N_of_needles; j++) {
        angles[j] = (std::rand() % 3600000) / 10000.f * 3.1415926535898f / 180.f;
        X[j] = (std::rand() % 19200000) / 10000.f;
        Y[j] = (std::rand() % 10800000) / 10000.f;
        needles[2*j].position = sf::Vector2f(X[j], Y[j]);
        needles[2*j].color = sf::Color::Black;
        needles[2*j + 1].position = sf::Vector2f(X[j] + length * std::cos(angles[j]), Y[j] + length * std::sin(angles[j]));
        needles[2*j + 1].color = sf::Color::Black;
        for (int i = 0; i < N_of_lines; i++)
            if (((width*i > needles[2*j].position.y) && (width*i < needles[2*j + 1].position.y)) || ((width*i < needles[2*j].position.y) && (width*i > needles[2*j + 1].position.y))) {
                needles[2*j].color = sf::Color::Red;
                needles[2*j + 1].color = sf::Color::Red;
                count++;
            }
    }
    std::cout.precision(10);
    for (int k = 0; k < N_of_needles; k++) {
        //printf("X[%d] = %f, Y[%d] = %f, angles[%d] = %f\n", k, X[k], k, Y[k], k, angles[k]);
        std::cout << "X[" << k << "] = " << X[k] << ", Y[" << k << "] = " << Y[k] << ", angles[" << k << "] = " << angles[k] << std::endl;
    }
    std::cout << "Count: " << count << std::endl;
    std::cout << "Number PI is: " << (float) 2*length/width/count*N_of_needles << std::endl;
    window.create(sf::VideoMode(1920, 1080), "My window");
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }
        window.clear(sf::Color::White);
        window.draw(lines);
        window.draw(needles);
        window.display();
    }
    free(angles);
    free(X);
    free(Y);
    return 0;
}
