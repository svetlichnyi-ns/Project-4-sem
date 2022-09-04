#include <iostream>
#include <iomanip>
#include <cstdlib>
#include <chrono>
#include "spigot.h"

int spigot() {
  std::cout << "Enter the number of digits in number PI that you want to calculate (two or more): ";
  int N;
  std::cin >> N;
  if (N < 2) {
    std::cerr << "This number should be not less than two (not less than one digit after the decimal point)\n";
    return -1;
  }
  int size = 10 * N / 3;  // the number of columns in a table (it depends on N)
  int* digits_of_pi = (int*) malloc (N * sizeof(int));  // allocation of memory for the array of digits of PI
  if (digits_of_pi == NULL) {
    std::cerr << "Failed to allocate memory via malloc() for an array <<digits_of_pi>>\n";
    return -1;
  }
  int** table = (int**) malloc ((4 * N) * sizeof(int*));  // allocation of memory for a two-dimensional array (table)
  if (table == NULL) {
    std::cerr << "Failed to allocate memory via malloc() for an array <<table>>\n";
    free(digits_of_pi);
    return -1;
  }
  for (int i = 0; i < 4 * N; i++) {
    table[i] = (int*) malloc (size * sizeof(int));
    if (table[i] == NULL) {
      free(digits_of_pi);
      free(table);
      std::cerr << "Failed to allocate memory via malloc() for the " << i << "th one-dimensional array: <<table[" << i << "]\n";
      return -1;
    }
  }
  std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();  // start the timer
  for (int i = 0; i < size; i++)  // initialization of the zeroth row by twenties
    table[0][i] = 20;
  for (int step = 0; step < N; step++) {
    // fill the last column of the table
    table[step * 4 + 1][size - 1] = 0;  // transfer
    table[step * 4 + 2][size - 1] = table[step * 4][size - 1] + table[step * 4 + 1][size - 1];  // sum
    table[step * 4 + 3][size - 1] = table[step * 4 + 2][size - 1] % (2 * (size - 1) + 1);  // remainder
    // fill the skeleton of the table
    for (int j = size - 2; j >= 1; j--) {
      table[step * 4 + 1][j] = (table[step * 4 + 2][j + 1] / (2 * (j + 1) + 1)) * (j + 1);  // transfer
      table[step * 4 + 2][j] = table[step * 4][j] + table[step * 4 + 1][j];  // sum
      table[step * 4 + 3][j] = table[step * 4 + 2][j] % (2 * j + 1);  // remainder
    }
    // fill the zeroth column of the table
    table[step * 4 + 1][0] = table[step * 4 + 2][1] / 3;
    table[step * 4 + 2][0] = table[step * 4 + 0][0] + table[step * 4 + 1][0];
    table[step * 4 + 3][0] = table[step * 4 + 2][0] % 10;
    // find a digit of PI
    digits_of_pi[step] = table[step * 4 + 2][0] / 10;
    // initialization for the following step
    if (step == N - 1)
      break;
    else {
      for (int i = 0; i < size; i++)
        table[(step + 1) * 4][i] = 10 * table[step * 4 + 3][i];
    }
  }
  // avoid overflow of individual digits
  for (int k = N - 1; k > 0; k--) {
    if (digits_of_pi[k] > 9) {
      digits_of_pi[k] -= 10;
      digits_of_pi[k - 1]++;
    }
  }
  // print the number PI
  std::cout << "Number PI is equal to: " << digits_of_pi[0] << ".";
  for (int i = 1; i < N; i++)
    std::cout << digits_of_pi[i];
  std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();  // stop the timer
  std::setprecision(10);
  std::cout << "; it took " << (std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count()) / 1000000.l;
  std::cout << " seconds to calculate PI.\n\n";
  // memory release
  free(digits_of_pi);
  for (int i = 0; i < size; i++)
    free(table[i]);
  free(table);
  return 0;
}