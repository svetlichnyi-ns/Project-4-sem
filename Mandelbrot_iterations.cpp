#include <iostream>
#include <math.h>
#include <SFML/Graphics.hpp>
#include <SFML/Window/Window.hpp>

using namespace std;

int main() {
    long double eps, z;
    cout << "Enter the epsilon: " << endl;
    cin >> eps;
    long double constanta = 0.25f + eps;
    z = constanta;
    int i = 1;
    sf::Clock clock;
    sf::Time elapsed;
    #pragma omp parallel for
    for (i = 1; z < 2; i++) {
        long double tmp = z;
        z = pow(tmp, 2) + constanta;
        cout << "Number of iteration: " << i << ", " << "current value: " << (long double) z << ";" << endl;
    }
    elapsed = clock.restart();
    cout << "Number of iterations: " << i << endl;
    cout << "Elapsed time: " << elapsed.asSeconds() << endl;
    return 0;
}