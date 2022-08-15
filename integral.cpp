#include <iostream>
#include <iomanip>
#include <sys/times.h>
#include <unistd.h>
#include <cmath>

using namespace std;

int main() {
  clock_t clockStart, clockStop;
	tms tmsStart, tmsStop;
  const unsigned long countOfSteps = 500000000;
  long double step = static_cast<long double> (1.0) / countOfSteps;
  long double sum = 0.0;
  clockStart = times(&tmsStart);
  for (unsigned long i = 0; i < countOfSteps; i++) {
    long double x = (i + 0.5) * step;
    sum += 4.0 / (1 + pow(x, 2.0));    
  }
  long double pi = sum * step;
  clockStop = times(&tmsStop);
  cout << "Number PI is: " << setprecision(10) << pi << endl;
  cout << "It took " << setprecision(6) << (clockStop - clockStart) / static_cast<long double>(sysconf(_SC_CLK_TCK)) << " seconds to calculate PI" << endl;
  return 0;
}