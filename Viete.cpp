#define _USE_MATH_DEFINES
#include <iostream>
#include <iomanip>
#include <cstdlib>
#include <cstdint>
#include <chrono>
#include <cmath>
#include "Viete.h"

int Viete() {
  uint64_t N;  // the number of factors that make up the product
  std::cout << "Enter the number of factors in Viete's formula: ";
  std::cin >> N;
  long double* factors = (long double*) malloc (N * sizeof(long double));  // a dynamic array of factors
  if (factors == NULL) {
    std::cerr << "Failed to allocate memory via malloc()\n";
    return -1;
  }
  std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();  // start the timer
  factors[0] = 2.l / sqrtl(2.l);  // initialize the zeroth factor
  long double pi = 2.l * factors[0];
  for (uint64_t i = 1; i < N; i++) {
    factors[i] = 2.l / sqrtl(2.l + 2.l / factors[i - 1]);  // nested radicals
    pi *= factors[i];
  }
  std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();  // stop the timer
  std::cout << "Number PI, obtained as a product of " << N << " factors, is equal to: " << std::setprecision(15) << pi << ";\n";
  std::cout << "The calculation error is: " << fabsl(pi - M_PIl) << ";\n";
  std::cout << "The calculation took " << (std::chrono::duration_cast<std::chrono::nanoseconds>(end - begin).count()) / 1'000'000'000.l << " seconds.\n\n";
  free(factors);  // free allocated memory
  return 0;
}