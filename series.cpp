#define _USE_MATH_DEFINES
#include <iostream>
#include <iomanip>
#include <vector>
#include <stdio.h>
#include <chrono>
#include <cmath>
#include "series.h"

enum series_t {Gregory_Leibniz, Madhava, Nilakantha, Euler, Wallis};

int calculation_of_pi_via_series (series_t way, int N) {
  long double pi = 0.l;  // initial value for addition
  long double pi_Wallis = 1.l;  // initial value for multiplication
  std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();  // start the timer
  for (int i = 0; i < N; i++) {
    switch (way) {   // there're several series, approximating number PI
      case Gregory_Leibniz:
        pi += powl(-1.l, i) / (2.l * i + 1.l);
        break;
      case Madhava:
        pi += powl(-1.l, i) / (2.l * i + 1.l) / powl(3.l, i);
        break;
      case Nilakantha:
        pi += powl(-1.l, i) / (2.l * i + 2.l) / (2.l * i + 3.l) / (2.l * i + 4.l);
        break;
      case Euler:
        pi += 1.l / powl(i + 1.l, 2.l);
        break;
      case Wallis:
        pi_Wallis *= powl((2.l * i + 2.l), 2.l) / (2.l * i + 1.l) / (2.l * i + 3.l);
        break;
      default:
        std::cerr << "Undefined method\n";
        return -1;
    }
  }
  std::cout << "The value of number PI for the first ";
  std::cout << std::setprecision(15);
  switch (way) {
    case Gregory_Leibniz:
      std::cout << N << " terms of Gregory-Leibniz series: " << 4.l * pi << ";\n";
      std::cout << "The calculation error is: " << fabsl(4.l * pi - M_PIl) << ";\n";
      break;
    case Madhava:
      std::cout << N << " terms of Madhava series: " << sqrtl(12.l) * pi << ";\n";
      std::cout << "The calculation error is: " << fabsl(sqrtl(12.l) * pi - M_PIl) << ";\n";
      break;
    case Nilakantha:
      std::cout << N + 1 << " terms of Nilakantha series: " << 4.l * pi + 3.l << ";\n";
      std::cout << "The calculation error is: " << fabsl((4.l * pi + 3.l) - M_PIl) << ";\n";
      break;
    case Euler:
      std::cout << N << " terms of Euler series: " << sqrtl(6.l * pi) << ";\n";
      std::cout << "The calculation error is: " << fabsl(sqrtl(6.l * pi) - M_PIl) << ";\n";
      break;
    case Wallis:
      std::cout << 2*N + 1 << " factors in Wallis' formula: " << 2.l * pi_Wallis << ";\n";
      std::cout << "The calculation error is: " << fabsl(2.l * pi_Wallis - M_PIl) << ";\n";
      break;
    default:
      break;
  }
  std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();  // stop the timer
  std::cout << "It took " << (std::chrono::duration_cast<std::chrono::nanoseconds>(end - begin).count()) / 1'000'000'000.l << " seconds to calculate PI.\n";
  return 0;
}

int series() {
  // vector of ways of calculation of PI
  std::vector<series_t> ways = {Gregory_Leibniz, Madhava, Nilakantha, Euler, Wallis};
  for (int j = 0; j < (int) ways.size(); j++) {
    for (int i = 10; i < 100'000'000; i *= 10) {
      if (calculation_of_pi_via_series(ways.at(j), i) == -1)
        return -1;
    }
    std::cout << '\n';
  }
  return 0;
}