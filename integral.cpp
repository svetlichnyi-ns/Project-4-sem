#include <iostream>
#include <iomanip>
#include <functional>
#include <vector>
#include <sys/times.h>
#include <unistd.h>
#include <math.h>

long double function_1 (long double x) {
  return 4.0 / (1.0 + pow(x, 2.0));
}

long double function_2 (long double x) {
  return 6.0 / sqrt (1.0 - pow(x, 2.0));
}

long double Integral (uint8_t method, long double a, long double b, unsigned long NumOfSegments, const std::function<long double (long double)> &func) {
  const long double length = (b - a) / NumOfSegments;
  long double integral = 0.0;
  switch (method) {
    case 0:
      for (long unsigned int step = 0; step < NumOfSegments; step++) {
        const long double x_left = a + step * length;
        integral += func(x_left);
      }
      break;
    case 1:
      for (long unsigned int step = 0; step < NumOfSegments; step++) {
        const long double x_right = a + (step + 1) * length;
        integral += func(x_right);
      }
      break;
    case 2:
      for (long unsigned int step = 0; step < NumOfSegments; step++) {
        const long double x_middle = a + length * (static_cast<long double> (step) + 0.5);
        integral += func(x_middle);
      }
      break;
    case 3:
      for (long unsigned int step = 0; step < NumOfSegments; step++) {
        const long double x_1 = a + step * length;
        const long double x_2 = a + (step + 1) * length;
        integral += func(x_1) + func(x_2);
      }
      integral /= 2.0;
      break;
    case 4:
      for (long unsigned int step = 0; step < NumOfSegments; step++) {
        const long double x_left = a + step * length;
        const long double x_middle = a + (static_cast<long double> (step) + 0.5) * length;
        const long double x_right = a + (step + 1) * length;
        integral += func(x_left) + 4.0 * func(x_middle) + func(x_right);
      }
      integral /= 6.0;
      break;
    default:
      std::cerr << "Undefined method";
      return -1;
  }
  integral *= length;
  return integral;
}

void usage_of_method (uint8_t method, long double a, long double b, unsigned long NumOfSegments, const std::function<long double (long double)> &func) {
  clock_t clockStart, clockStop;
  tms tmsStart, tmsStop;
  long double pi;
  std::cout << "Wait for the results of usage of ";
  switch (method) {
    case 0:
      std::cout << "left rectangle method...\n";
      break;
    case 1:
      std::cout << "right rectangle method...\n";
      break;
    case 2:
      std::cout << "middle rectangle method...\n";
      break;
    case 3:
      std::cout << "trapezoidal method...\n";
      break;
    case 4:
      std::cout << "parabola method (Simpson's method)...\n";
      break;
    default:
      std::cerr << "Undefined method";
      return;
  }
  clockStart = times(&tmsStart);
  pi = Integral(method, a, b, NumOfSegments, func);
  clockStop = times(&tmsStop);
  std::cout << "Number PI is: " << std::setprecision(10) << pi;
  std::cout << "; it took " << std::setprecision(6) << (clockStop - clockStart) / static_cast<long double>(sysconf(_SC_CLK_TCK)) << " seconds to calculate PI." << std::endl << std::endl;
  return;
}

int main() {
  std::vector<uint8_t> methods = {0, 1, 2, 3, 4};
  std::cout << "The first integral expression" << std::endl;
  for (uint8_t i = 0; i < methods.size(); i++)
    usage_of_method(methods.at(i), 0, 1, 500'000'000, function_1);
  std::cout << "The second integral expression" << std::endl;
  for (uint8_t i = 0; i < methods.size(); i++)
    usage_of_method(methods.at(i), 0, 0.5, 500'000'000, function_2);
  return 0;
}