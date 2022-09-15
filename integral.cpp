#define _USE_MATH_DEFINES
#include <iostream>
#include <iomanip>
#include <functional>
#include <vector>
#include <chrono>
#include <random>
#include <cmath>
#include "integral.h"

enum methods_t {left_rectangle, right_rectangle, middle_rectangle, trapezoidal, Simpson, Romberg, one_dim, two_dim};

class Point {  // especially for two-dimensional Monte Carlo method
  private:
    long double x, y;
  public:
    Point() {}  // constructor by default
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

// a function that participates in the first integral expression
long double function_1 (long double x) {
  return 4.l / (1.l + powl(x, 2.l));
}

// a function that participates in the second integral expression
long double function_2 (long double x) {
  return 6.l / sqrtl(1.l - powl(x, 2.l));
}

// a function that participates in the third integral expression
long double function_3 (long double x) {
  return 2.l * sqrtl(1.l - powl(x, 2.l));
}

// a function that is responsible directly for integral's calculation
long double Integral (methods_t method, long double a, long double b, unsigned long N, long double (*func) (long double)) {
  const long double length = (b - a) / N;  // the length of each segment of integration
  long double integral = 0.l;  // an initial value of the integral
  switch (method) {
    case left_rectangle:
      for (long unsigned int step = 0; step < N; step++) {
        const long double x_left = a + step * length;
        integral += func(x_left);
      }
      integral *= length;
      break;
    case right_rectangle:
      for (long unsigned int step = 0; step < N; step++) {
        const long double x_right = a + (step + 1) * length;
        integral += func(x_right);
      }
      integral *= length;
      break;
    case middle_rectangle:
      for (long unsigned int step = 0; step < N; step++) {
        const long double x_middle = a + length * (static_cast<long double> (step) + 0.5l);
        integral += func(x_middle);
      }
      integral *= length;
      break;
    case trapezoidal:
      for (long unsigned int step = 0; step < N; step++) {
        const long double x_1 = a + step * length;
        const long double x_2 = a + (step + 1) * length;
        integral += func(x_1) + func(x_2);
      }
      integral /= 2.l;
      integral *= length;
      break;
    case Simpson:
      for (long unsigned int step = 0; step < N; step++) {
        const long double x_left = a + step * length;
        const long double x_middle = a + (static_cast<long double> (step) + 0.5l) * length;
        const long double x_right = a + (step + 1) * length;
        integral += func(x_left) + 4.l * func(x_middle) + func(x_right);
      }
      integral /= 6.l;
      integral *= length;
      break;
    case Romberg:
    {
      std::vector<std::vector<long double>> Romberg_integral(20, std::vector<long double> (20));  // the implementation of a square lower-triangular matrix
      Romberg_integral.front().front() = Integral(trapezoidal, a, b, 1, func);  // trapezoidal integral (method 3)
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
    case one_dim:
    {
      // preparation of random number generator
      std::random_device rd;
      std::default_random_engine eng(rd());
      std::uniform_real_distribution<> distr(0, 1);
      for (long unsigned int step = 0; step < N; step++) {
        // choose an arbitrary abscissa on each interval of integration
        const long double x_random = a + (long double) step * length + length * distr(eng);
        integral += func(x_random);
      }
      integral *= length;
      break;
    }
    case two_dim:
    {
      unsigned long int count = 0;  // counter of the number of points, which are located in the area under the graph of the function
      std::vector <Point> Points(N);
      // preparation of random number generator
      std::random_device rd;
      std::default_random_engine eng(rd());
      std::uniform_real_distribution<> distr(0, 1);
      for (unsigned long int i = 0; i < N; i++) {
        // a random abscissa, belonging to the segment [a; b]
        Points[i].set_X(a + (b - a) * distr(eng));
        // a random ordinate, belonging to the segment [0; 10] (the upper value is taken with a margin)
        Points[i].set_Y(10.l * distr(eng));
        if (Points[i].get_Y() <= func(Points[i].get_X()))  // if a point occurs under the function's graph, we increment count
          count++;
      }
      integral = (10.l * (b - a)) * (long double) count / (long double) N;  // integral is calculated, based on the probability for each point to occur under the function's graph
      break;
    }
    default:
      std::cerr << "Undefined method\n";
      return -1.l;
  }
  return integral;
}

// a function that is in charge of timing and derivation of results
int usage_of_method (methods_t method, long double a, long double b, unsigned long N, long double (*func) (long double)) {
  switch (method) {
    case left_rectangle:
      std::cout << "Wait for the results of usage of left rectangle method...\n";
      break;
    case right_rectangle:
      std::cout << "Wait for the results of usage of right rectangle method...\n";
      break;
    case middle_rectangle:
      std::cout << "Wait for the results of usage of middle rectangle method...\n";
      break;
    case trapezoidal:
      std::cout << "Wait for the results of usage of trapezoidal method...\n";
      break;
    case Simpson:
      std::cout << "Wait for the results of usage of parabola method (Simpson's method)...\n";
      break;
    case Romberg:
      std::cout << "Wait for the results of usage of Romberg's method...\n";
      break;
    case one_dim:
      std::cout << "Wait for the results of usage of one-dimensional Monte Carlo method...\n";
      break;
    case two_dim:
      std::cout << "Wait for the results of usage of two-dimensional Monte Carlo method (estimation of areas)...\n";
      break;
    default:
      std::cerr << "Undefined method\n";
      return -1;
  }
  std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();  // start the timer
  // in the context of methods 0-4 and 6: N is the number of segments of integration; in the 7th method, it's the number of scattered points
  if (Integral(method, a, b, N, func) < 0.l)
    return -1;
  long double pi = Integral(method, a, b, N, func);
  std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();  // stop the timer
  std::cout << "Number PI is: " << std::setprecision(15) << pi;
  std::cout << ";\nThe calculation error is: " << fabsl(pi - M_PIl);
  std::cout << ";\nIt took " << (std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count()) / 1000000.l;
  std::cout << " seconds to calculate PI.\n\n";
  return 0;
}

// a function that asks a user about the preferenced number of segments of integration and runs experiments
int integral_computation() {
  std::cout << "Enter the number of parts, into which you want to split the segment of integration (not more than 150'000'000): ";
  unsigned long NumOfSegments;
  std::cin >> NumOfSegments;
  if (NumOfSegments < 1) {
    std::cerr << "The number of segments of integration must be positive.\n";
    return -1;
  }
  if (NumOfSegments > 150'000'000) {
    std::cerr << "The program can't cope with so many segments.\n";
    return -1;
  }
  // all currently available methods
  std::vector<methods_t> methods = {left_rectangle, right_rectangle, middle_rectangle, trapezoidal, Simpson, Romberg, one_dim, two_dim};
  std::cout << "The first integral expression" << std::endl;
  for (auto i : methods) {
    if (usage_of_method(i, 0.l, 1.l, NumOfSegments, function_1) == -1)
      return -1;
  }
  std::cout << "The second integral expression" << std::endl;
  for (auto i : methods) {
    if (usage_of_method(i, 0.l, 0.5l, NumOfSegments, function_2) == -1)
      return -1;
  }
  std::cout << "The third integral expression" << std::endl;
  for (auto i : methods) {
    if (usage_of_method(i, -1.l, 1.l, NumOfSegments, function_3) == -1)
      return -1;
  }
  return 0;
}