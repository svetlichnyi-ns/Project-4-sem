#include <iostream>
#include <iomanip>
#include <functional>
#include <assert.h>
#include <vector>
#include <cstdlib>
#include <chrono>
#include <cmath>

#define PI_25_DIGITS 3.141592653589793238462643

class Point {  // especially for two-dimensional Monte Carlo method
  private:
    long double x, y;
  public:
    Point(long double x_value, long double y_value) {  // constructor
      x = x_value;
      y = y_value;
    }
    ~Point() {}  // destructor
    long double get_X() {
      return x;
    }
    void set_X(long double x_value) {
      x = x_value;
    }
    long double get_Y() {
      return y;
    }
    void set_Y(long double y_value) {
      y = y_value;
    }
};

long double function_1 (long double x) {
  return 4.l / (1.l + powl(x, 2.l));
}

long double function_2 (long double x) {
  return 6.l / sqrtl(1.l - powl(x, 2.l));
}

long double function_3 (long double x) {
  return 2.l * sqrtl(1.l - powl(x, 2.l));
}

long double Integral (uint8_t method, long double a, long double b, unsigned long N, long double (*func) (long double)) {
  const long double length = (b - a) / N;  // the length of each segment of integration
  long double integral = 0.l;  // an initial value of the integral
  switch (method) {
    case 0:  // left rectangle method
      for (long unsigned int step = 0; step < N; step++) {
        const long double x_left = a + step * length;
        integral += func(x_left);
      }
      integral *= length;
      break;
    case 1:  // right rectangle method
      for (long unsigned int step = 0; step < N; step++) {
        const long double x_right = a + (step + 1) * length;
        integral += func(x_right);
      }
      integral *= length;
      break;
    case 2:  // middle rectangle method
      for (long unsigned int step = 0; step < N; step++) {
        const long double x_middle = a + length * (static_cast<long double> (step) + 0.5l);
        integral += func(x_middle);
      }
      integral *= length;
      break;
    case 3:  // trapezoidal method
      for (long unsigned int step = 0; step < N; step++) {
        const long double x_1 = a + step * length;
        const long double x_2 = a + (step + 1) * length;
        integral += func(x_1) + func(x_2);
      }
      integral /= 2.l;
      integral *= length;
      break;
    case 4:  // parabola method (Simpson's method)
      for (long unsigned int step = 0; step < N; step++) {
        const long double x_left = a + step * length;
        const long double x_middle = a + (static_cast<long double> (step) + 0.5l) * length;
        const long double x_right = a + (step + 1) * length;
        integral += func(x_left) + 4.l * func(x_middle) + func(x_right);
      }
      integral /= 6.l;
      integral *= length;
      break;
    case 5:  // Romberg's method
    {
      std::vector<std::vector<long double>> Romberg_integral(20, std::vector<long double> (20));  // the implementation of a square lower-triangular matrix
      Romberg_integral.front().front() = Integral(3, a, b, 1, func);  // trapezoidal integral (method 3)
      long double Romberg_length = b - a;  // current length of the segment of integration
      for (unsigned long int step = 1; step < Romberg_integral.size(); step++) {
        Romberg_length /= 2.l;
        long double trapezoidal_integral = 0.l;
        long double stepEnd = powl(2.l, static_cast<long double> (step - 1));
        for (unsigned long int tzStep = 1; tzStep <= stepEnd; tzStep++) {
          const long double deltaX = (2.l * tzStep - 1.l) * Romberg_length;
          trapezoidal_integral += func(a + deltaX);
        }
        Romberg_integral[step].front() = 0.5l * Romberg_integral[step - 1].front() + trapezoidal_integral * Romberg_length;
        for (unsigned long int rbStep = 1; rbStep <= step; rbStep++) {
          const long double k = powl(4.l, rbStep);
          Romberg_integral[step][rbStep] = (k * Romberg_integral[step][rbStep - 1] - Romberg_integral[step - 1][rbStep - 1]) / (k - 1);
        }
      }
      return Romberg_integral[19][19];  // the result contains in the lower right corner of the lower-triangular matrix
    }
    case 6:  // one-dimensional Monte Carlo method
      for (long unsigned int step = 0; step < N; step++) {
        // choose an arbitrary abscissa on each interval of integration
        const long double x_random = a + (long double) step * length + length * static_cast <long double> (rand()) / static_cast <long double> (RAND_MAX);
        integral += func(x_random);
      }
      integral *= length;
      break;
    case 7:  // two-dimensional Monte Carlo method (estimation of areas)
    {
      unsigned long int count = 0;  // counter of the number of points, which are located in the area under the graph of the function
      Point* Points = (Point*) malloc (N * sizeof(Point));  // a dynamic array of instances of class Point
      if (Points == NULL) {
        std::cout << "Failed to allocate memory via malloc()\n";
        return -1;
      }
      for (unsigned long int i = 0; i < N; i++) {
        // a random abscissa, belonging to the segment [a; b]
        Points[i].set_X(a + (b - a) * static_cast <long double> (rand()) / static_cast <long double> (RAND_MAX));
        // a random ordinate, belonging to the segment [0; 10] (the upper value is taken with a margin)
        Points[i].set_Y(10.l * static_cast <long double> (rand()) / static_cast <long double> (RAND_MAX));
        if (Points[i].get_Y() <= func(Points[i].get_X()))  // if a point occurs under the function's graph, we increment count
          count++;
      }
      integral = (10.l * (b - a)) * (long double) count / (long double) N;  // integral is calculated, based on the probability for each point to occur under the function's graph
      free(Points);
      break;
    }
    default:
      std::cerr << "Undefined method\n";
      return -1;
  }
  return integral;
}

int usage_of_method (uint8_t method, long double a, long double b, unsigned long N, long double (*func) (long double)) {
  switch (method) {
    case 0:
      std::cout << "Wait for the results of usage of left rectangle method...\n";
      break;
    case 1:
      std::cout << "Wait for the results of usage of right rectangle method...\n";
      break;
    case 2:
      std::cout << "Wait for the results of usage of middle rectangle method...\n";
      break;
    case 3:
      std::cout << "Wait for the results of usage of trapezoidal method...\n";
      break;
    case 4:
      std::cout << "Wait for the results of usage of parabola method (Simpson's method)...\n";
      break;
    case 5:
      std::cout << "Wait for the results of usage of Romberg's method...\n";
      break;
    case 6:
      std::cout << "Wait for the results of usage of one-dimensional Monte Carlo method...\n";
      break;
    case 7:
      std::cout << "Wait for the results of usage of two-dimensional Monte Carlo method (estimation of areas)...\n";
      break;
    default:
      std::cerr << "Undefined method\n";
      return -1;
  }
  std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();  // start the timer
  // in the context of methods 0-4 and 6: N is the number of segments of integration; in the 7th method, it's the number of scattered points
  if (Integral(method, a, b, N, func) == -1)
    return -1;
  long double pi = Integral(method, a, b, N, func);
  std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();  // stop the timer
  std::cout << "Number PI is: " << std::setprecision(15) << pi;
  std::cout << ";\nThe calculation error is: " << fabsl(pi - PI_25_DIGITS);
  std::cout << ";\nIt took " << (std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count()) / 1000000.l;
  std::cout << " seconds to calculate PI.\n\n";
  return 0;
}

int main() {
  std::cout << "Enter the number of parts, into which you want to split the segment of integration: ";
  unsigned long NumOfSegments;
  std::cin >> NumOfSegments;
  assert(NumOfSegments >= 1 && "The number of segments of integration must be positive");

  std::vector<uint8_t> methods = {};  // all currently available methods
  for (uint8_t i = 0; i < 8; i++)
    methods.push_back(i);
  std::cout << "The first integral expression" << std::endl;
  for (uint8_t i = 0; i < methods.size(); i++) {
    if (usage_of_method(methods.at(i), 0.l, 1.l, NumOfSegments, function_1) == -1)
      return -1;
  }
  std::cout << "The second integral expression" << std::endl;
  for (uint8_t i = 0; i < methods.size(); i++) {
    if (usage_of_method(methods.at(i), 0.l, 0.5l, NumOfSegments, function_2) == -1)
      return -1;
  }
  std::cout << "The third integral expression" << std::endl;
  for (uint8_t i = 0; i < methods.size(); i++) {
    if (usage_of_method(methods.at(i), -1.l, 1.l, NumOfSegments, function_3) == -1)
      return -1;
  }
  return 0;
}