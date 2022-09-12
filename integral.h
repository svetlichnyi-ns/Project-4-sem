#ifndef INTEGRAL_H
#define INTEGRAL_H
#include <iostream>

class Point {  // especially for two-dimensional Monte Carlo method
  private:
    long double x, y;
  public:
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
long double function_1 (long double x);
// a function that participates in the second integral expression
long double function_2 (long double x);
// a function that participates in the third integral expression
long double function_3 (long double x);

// a function that is responsible directly for integral's calculation
long double Integral (uint8_t method, long double a, long double b, unsigned long N, long double (*func) (long double));
// a function that is in charge of timing and derivation of results
int usage_of_method (uint8_t method, long double a, long double b, unsigned long N, long double (*func) (long double));
// a function that asks a user about the preferenced number of segments of integration and runs experiments
int integral_computation();

#endif