#define _USE_MATH_DEFINES
#include <iostream>
#include <iomanip>
#include <cstdint>
#include <chrono>
#include <cmath>
#include "Viete.h"

int Viete() {
  uint64_t N;  // the number of factors (on the right side of the formula) that make up the product
  std::cout << "Enter the number of factors in Viete's formula (at least, two): ";
  std::cin >> N;
  if (N <= 1) {
    std::cerr << "The number of factors must be not less than two.\n";
    return -1;
  }
  std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();  // start the timer
  long double previous_factor = 2.l / sqrtl(2.l);
  long double pi = 2.l * previous_factor;
  long double next_factor = 2.l / sqrtl(2.l + 2.l / previous_factor);
  pi *= next_factor;
  // now we have already calculated the number PI, based on two factors
  for (uint64_t i = 2; i < N; i++) {
    previous_factor = next_factor;
    next_factor = 2.l / sqrtl(2.l + 2.l / previous_factor);
    pi *= next_factor;
  }
  std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();  // stop the timer
  std::cout << "Number PI, obtained as a product of " << N << " factors, is equal to: " << std::setprecision(15) << pi << ";\n";
  std::cout << "The calculation error is: " << fabsl(pi - M_PIl) << ";\n";
  std::cout << "The calculation took " << (std::chrono::duration_cast<std::chrono::nanoseconds>(end - begin).count()) / 1'000'000'000.l << " seconds.\n\n";
  return 0;
}