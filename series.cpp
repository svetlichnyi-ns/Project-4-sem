#include <iostream>
#include <iomanip>
#include <vector>
#include <omp.h>
#include <math.h>

using namespace std;

void calculation_of_pi (unsigned int way, int N) {
  long double pi = 0;
  long double start, end;
  start = omp_get_wtime();
  #pragma omp parallel reduction(+:pi)
  {
    #pragma omp parallel for
    for (int i = 0; i < N; i++) {
      switch (way) {
        case 0:
          pi += (long double) 4.0 * pow(-1.0, i) / (2.0 * i + 1.0);
          break;
        case 1:
          pi += (long double) sqrtl(12.0) * pow(-1.0, i) / (2.0 * i + 1.0) / pow(3.0, i);
          break;
        case 2:
          pi += (long double) (3.0 / N);
          pi += (long double) 4.0 * pow(-1.0, i) / (i + 2.0) / (i + 3.0) / (i + 4.0);
          break;
        case 3:
          pi += (long double) 6.0 * pow(i + 1.0, 2.0);
          break;
      }
    }
  }
  end = omp_get_wtime();
  switch (way) {
    case 0:
      cout << "Значение pi для первых " << N << " слагаемых по формуле Грегори-Лейбница: " << setprecision(10) << pi << endl;
      cout << "Время по формуле Грегори-Лейбница: " << setprecision(6) << end - start << endl;
      break;
    case 1:
      cout << "Значение pi для первых " << N << " слагаемых по формуле Мадхавы: " << setprecision(10) << pi << endl;
      cout << "Время по формуле Мадхавы: " << setprecision(6) << end - start << endl;
      break;
    case 2:
      cout << "Значение pi для первых " << N << " слагаемых по формуле Нилаканта: " << setprecision(10) << pi << endl;
      cout << "Время по формуле Нилаканта: " << setprecision(6) << end - start << endl;
      break;
    case 3:
      cout << "Значение pi для первых " << N << " слагаемых по формуле Эйлера: " << setprecision(10) << sqrtl(pi) << endl;
      cout << "Время по формуле Эйлера: " << setprecision(6) << end - start << endl;
      break;
  }
  return;
}

int main() {
  vector<unsigned int> ways = {0, 1, 2, 3};
  for (int j = 0; j < ways.size(); j++) {
    for (int i = 10; i < 10000000; i = i * 10)
      calculation_of_pi(ways.at(j), i);
    cout << endl;
  }
  return 0;
}