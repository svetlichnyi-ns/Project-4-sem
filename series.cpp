#include <iostream>
#include <iomanip>
#include <vector>
#include <stdio.h>
#include <omp.h>
#include <cmath>

#define PI_25_DIGITS 3.141592653589793238462643

int calculation_of_pi (int way, int N) {
  if (!((way == 0) || (way == 1) || (way == 2) || (way == 3) || (way == 4))) {
    std::cerr << "Undefined method\n";
    return -1;
  }
  long double pi = 0.l;  // initial value for addition
  long double pi_Wallis = 1.l;  // initial value for multiplication
  long double start = omp_get_wtime();  // the beginning of the algorithm
  #pragma omp parallel reduction(+:pi)
  {
    #pragma omp parallel for
    for (int i = 0; i < N; i++) {
      switch (way) {   // there're several series, approximating number pi
        case 0:  // Gregory-Leibniz series
          pi += powl(-1.l, i) / (2.l * i + 1.l);
          break;
        case 1:  // Madhava series
          pi += powl(-1.l, i) / (2.l * i + 1.l) / powl(3.l, i);
          break;
        case 2:  // Nilakantha series
          pi += powl(-1.l, i) / (2.l * i + 2.l) / (2.l * i + 3.l) / (2.l * i + 4.l);
          break;
        case 3:  // Euler series
          pi += 1.l / powl(i + 1.l, 2.l);
          break;
        case 4:  // Wallis' formula
          pi_Wallis *= powl((2.l * i + 2.l), 2.l) / (2.l * i + 1.l) / (2.l * i + 3.l);
          break;
        default:
          break;
      }
    }
  }
  std::cout << "The value of number PI for the first ";
  std::cout << std::setprecision(15);
  switch (way) {
    case 0:
      std::cout << N << " terms of Gregory-Leibniz series: " << 4.l * pi << ";\n";
      std::cout << "The calculation error is: " << fabsl(4.l * pi - PI_25_DIGITS) << ";\n";
      break;
    case 1:
      std::cout << N << " terms of Madhava series: " << sqrtl(12.l) * pi << ";\n";
      std::cout << "The calculation error is: " << fabsl(sqrtl(12.l) * pi - PI_25_DIGITS) << ";\n";
      break;
    case 2:
      std::cout << N + 1 << " terms of Nilakantha series: " << 4.l * pi + 3.l << ";\n";
      std::cout << "The calculation error is: " << fabsl((4.l * pi + 3.l) - PI_25_DIGITS) << ";\n";
      break;
    case 3:
      std::cout << N << " terms of Euler series: " << sqrtl(6.l * pi) << ";\n";
      std::cout << "The calculation error is: " << fabsl(sqrtl(6.l * pi) - PI_25_DIGITS) << ";\n";
      break;
    case 4:
      std::cout << 2*N + 1 << " factors in Wallis' formula: " << 2.l * pi_Wallis << ";\n";
      std::cout << "The calculation error is: " << fabsl(2.l * pi_Wallis - PI_25_DIGITS) << ";\n";
      break;
    default:
      break;
  }
  long double end = omp_get_wtime();  // the end of the algorithm
  std::cout << "It took " << std::setprecision(6) << end - start << " seconds to calculate PI.\n";
  return 0;
}

int main() {
  std::vector<int> ways = {};  // vector of ways of calculation of pi
  for (int i = 0; i < 5; i++)
    ways.push_back(i);
  for (int j = 0; j < (int) ways.size(); j++) {
    for (int i = 10; i < 1'000'000'000; i *= 10) {
      if (calculation_of_pi(ways.at(j), i) == -1)
        return -1;
    }
    std::cout << '\n';
  }
  return 0;
}