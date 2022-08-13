#include <iostream>
#include <iomanip>
#include <vector>
#include <stdint.h>
#include <omp.h>
#include <math.h>

using namespace std;

void calculation_of_pi (unsigned int way, uint64_t N) {
  long double pi = 0;  // initial value for addition
  long double pi_Wallis = 1;  // initial value for multiplication
  long double start = omp_get_wtime();  // the beginning of the algorithm
  #pragma omp parallel reduction(+:pi)
  {
    #pragma omp parallel for
    for (uint64_t i = 0; i < N; i++) {  // "N" is the number of terms, forming number pi
      switch (way) {   // there're several series, approximating number pi
        case 0:  // the Gregory-Leibniz series
          pi += pow(-1.0, i) / (2.0 * i + 1.0);
          break;
        case 1:  // the Madhava series
          pi += pow(-1.0, i) / (2.0 * i + 1.0) / pow(3.0, i);
          break;
        case 2:  // the Nilakantha series
          pi += pow(-1.0, i) / (2.0 * i + 2.0) / (2.0 * i + 3.0) / (2.0 * i + 4.0);
          break;
        case 3:  // the Euler series
          pi += 1.0 / pow(i + 1.0, 2.0);
          break;
        case 4:  // the Wallis' formula
          pi_Wallis *= pow((2.0 * i + 2.0), 2.0) / (2.0 * i + 1.0) / (2.0 * i + 3.0);
          break;
      }
    }
  }
  long double end = omp_get_wtime();  // the end of the algorithm
  switch (way) {
    case 0:
      cout << "Значение pi для первых " << N << " слагаемых по формуле Грегори-Лейбница: " << setprecision(11) << 4.0 * pi << endl;
      cout << "Время по формуле Грегори-Лейбница: " << setprecision(6) << end - start << endl;
      break;
    case 1:
      cout << "Значение pi для первых " << N << " слагаемых по формуле Мадхавы: " << setprecision(11) << sqrtl(12.0) * pi << endl;
      cout << "Время по формуле Мадхавы: " << setprecision(6) << end - start << endl;
      break;
    case 2:
      cout << "Значение pi для первых " << N + 1 << " слагаемых по формуле Нилаканта: " << setprecision(11) << 4.0 * pi + 3.0 << endl;
      cout << "Время по формуле Нилаканта: " << setprecision(6) << end - start << endl;
      break;
    case 3:
      cout << "Значение pi для первых " << N << " слагаемых по формуле Эйлера: " << setprecision(11) << sqrtl(6.0 * pi) << endl;
      cout << "Время по формуле Эйлера: " << setprecision(6) << end - start << endl;
      break;
    case 4:
      cout << "Значение pi для первых " << 2 * N + 1 << " множителей по формуле Валлиса: " << setprecision(11) << 2 * pi_Wallis << endl;
      cout << "Время по формуле Валлиса: " << setprecision(6) << end - start << endl;
      break;
  }
  return;
}

int main() {
  vector<unsigned int> ways = {0, 1, 2, 3, 4};  // vector of ways of calculation of pi
  for (uint8_t j = 0; j < ways.size(); j++) {
    for (uint64_t i = 10; i < 10000000; i = i * 10)
      calculation_of_pi(ways.at(j), i);
    cout << endl;
  }
  return 0;
}