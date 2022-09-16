#define _USE_MATH_DEFINES
#include <iostream>
#include <iomanip>
#include <limits.h>
#include <thread>
#include <chrono>
#include <cmath>
#include <mutex>
#include <random>
#include <vector>
#include "integral.h"

enum methods_t {left_rectangle, right_rectangle, middle_rectangle, trapezoidal, Simpson};

long double result_0_4;  // a global variable, common for all threads (used in methods 0-4)
long double answer_6_1;  // a global variable, used in one-dimensional Monte Carlo method (method 6.1)
int count_2dim;  // a global variable, used in two-dimensional Monte Carlo method (method 6.2)
std::mutex mutex_0_4;  // it's used for threads' synchronization (used in methods 0-4)
std::mutex mutex_6_1;  // it's used for synchronization of threads, working on the first task (method 6.1)
std::mutex mutex_6_2;  // it's used for synchronization of threads, working on the second task (method 6.2)

// the arguments of a function, called on a thread, are "wrapped" in a structure (used in methods 0-4)
typedef struct {
  long double st_left, st_right;  // left and right limits of integration for a given thread
  long double st_step;  // the length of each segment of integration
  long double (*st_func) (long double);  // the function whose integral is calculated
  methods_t st_method;  // number of method, which is used for integral's calculation
} Args_0_4;

// arguments for a function, called on a thread, working on the first task (method 6.1)
typedef struct {
  long double st_left, st_right;  // left and right limits of integration for a given thread
  long double st_step;  // the length of each segment of integration
  long double (*st_func) (long double);  // the function whose integral is calculated
} Args_6_1;

// arguments for a function, called on a thread, working on the second task (method 6.2)
typedef struct {
  long st_from, st_to;  // initial and final numbers of points, scattered by a given thread
  long double st_a, st_b;  // left and right limits of integration
  long double (*st_func) (long double);  // the function whose integral is calculated
} Args_6_2;

void integral_0_4(Args_0_4* arg);  // a function, called on a thread (used in methods 0-4)
void integral_6_1(Args_6_1* arg);  // a function, called on a thread, working on the first task (method 6.1)
void integral_6_2(Args_6_2* arg);  // a function, called on a thread, working on the second task

// the function whose integral is calculated in the first integral expression
long double function_1 (long double x) {
  return 4.l / (1.l + powl(x, 2.l));
}

// the function whose integral is calculated in the second integral expression
long double function_2 (long double x) {
  return 6.l / sqrtl(1.l - powl(x, 2.l));
}

// the function whose integral is calculated in the third integral expression
long double function_3 (long double x) {
  return 2.l * sqrtl(1.l - powl(x, 2.l));
}

// a function that is called if the user chooses one of methods 0-4
int methods_0_4(methods_t method, long double a, long double b, long double (*func) (long double));
// a function that is called if the user chooses Romberg method (number 5)
void Romberg(long double a, long double b, long double (*func) (long double));
// a function that is called if the user chooses Monte Carlo methods (number 6)
int integrals_Monte_Carlo(long double a, long double b, long double (*func) (long double));

// a main function that interacts with user and launches a chosen experiment
int integral_computation() {
  std::vector<methods_t> methods = {left_rectangle, right_rectangle, middle_rectangle, trapezoidal, Simpson};
  std::cout << "The first integral expression\n";
  for (auto i : methods) {
    if (methods_0_4(i, 0.l, 1.l, function_1) == -1)
      return -1;
  }
  Romberg(0.l, 1.l, function_1);
  if (integrals_Monte_Carlo(0.l, 1.l, function_1) == -1)
    return -1;
  std::cout << "The second integral expression\n";
  for (auto i : methods) {
    if (methods_0_4(i, 0.l, 0.5l, function_2) == -1)
      return -1;
  }
  if (integrals_Monte_Carlo(0.l, 0.5l, function_2) == -1)
    return -1;
  std::cout << "The third integral expression\n";
  for (auto i : methods) {
    if (methods_0_4(i, -1.l, 1.l, function_3) == -1)
      return -1;
  }
  Romberg(-1.l, 1.l, function_3);
  if (integrals_Monte_Carlo(-1.l, 1.l, function_3) == -1)
    return -1;
  return 0;
}

void integral_0_4(Args_0_4* arg) {
  long double sum = 0.l;
  switch (arg->st_method) {
    case left_rectangle:
    {
      long i = 1L;  // iterator
      long double x = arg->st_left;
      do {
        sum += arg->st_func(x);
        x = arg->st_left + i * arg->st_step;
        i++;
      } while (x + arg->st_step <= arg->st_right);  // we prevent this cycle from completing its last iteration
      sum *= arg->st_step;
      sum += arg->st_func(x) * (arg->st_right - x);  // we add the area of the last small "left rectangle"
      break;
    }
    case right_rectangle:
    {
      long i = 2L;  // iterator
      long double x = arg->st_left + arg->st_step;
      do {
        sum += arg->st_func(x);
        x = arg->st_left + i * arg->st_step;
        i++;
      } while (x <= arg->st_right);  // we prevent this cycle from completing its last iteration
      sum *= arg->st_step;
      sum += arg->st_func(arg->st_right) * (arg->st_right - x);  // we add the area of the last small "right rectangle"
      break;
    }
    case middle_rectangle:
    {
      long i = 1L;  // iterator
      long double x = arg->st_left + 0.5l * arg->st_step;
      do {
        sum += arg->st_func(x);
        x = arg->st_left + 0.5l * arg->st_step + i * arg->st_step;
        i++;
      } while (x + 0.5l * arg->st_step <= arg->st_right);  // we prevent this cycle from completing its last iteration
      sum *= arg->st_step;
      long double x_center = 0.5l * ((x - 0.5l * arg->st_step) + (arg->st_right));
      sum += 2.l * (arg->st_right - x_center) * arg->st_func(x_center);  // we add the area of the last small "middle rectangle"
      break;
    }
    case trapezoidal:
    {
      long i = 1L;  // iterator
      long double x_1 = arg->st_left;
      long double x_2 = arg->st_left + arg->st_step;
      do {
        sum += arg->st_func(x_1) + arg->st_func(x_2);
        x_1 = arg->st_left + i * arg->st_step;
        x_2 = x_1 + arg->st_step;
        i++;
      } while (x_2 <= arg->st_right);  // we prevent this cycle from completing its last iteration
      sum /= 2.l;
      sum *= arg->st_step;
      sum += 0.5l * (arg->st_right - x_1) * (arg->st_func(x_1) + arg->st_func(arg->st_right));  // we add the area of the last small trapezoid
      break;
    }
    case Simpson:
    {
      long i = 1L;  // iterator
      long double x_left = arg->st_left;
      long double x_middle = arg->st_left + 0.5l * arg->st_step;
      long double x_right = arg->st_left + arg->st_step;
      do {
        sum += arg->st_func(x_left) + 4.l * arg->st_func(x_middle) + arg->st_func(x_right);
        x_left = arg->st_left + i * arg->st_step;
        x_middle = x_left + 0.5l * arg->st_step;
        x_right = x_left + arg->st_step;
        i++;
      } while (x_right <= arg->st_right);  // we prevent this cycle from completing its last iteration
      sum /= 6.l;
      sum *= arg->st_step;
      // make preparations for a new "parabola"
      long double x_new_left = x_left;
      long double x_new_right = arg->st_right;
      long double x_new_middle = 0.5l * (x_new_left + x_new_right);
      sum += (x_new_right - x_new_left) * (arg->st_func(x_new_left) + 4.l * arg->st_func(x_new_middle) + arg->st_func(x_new_right)) / 6.l;  // we add the area of the last small "parabola"
      break;
    }
    default:
      std::cerr << "Undefined method\n";
      return;
  }
  // the beginning of the access to the critical section, i.e. to a variable 'result_0_4'
  mutex_0_4.lock();
  result_0_4 += sum;
  // the end of the access to the critical section, i.e. to a variable 'result_0_4'
  mutex_0_4.unlock();
  return;
}

void integral_6_1(Args_6_1* arg) {
  long double sum = 0.l;
  // prepare a random number generator
  std::random_device rd;
  std::default_random_engine eng(rd());
  std::uniform_real_distribution<> distr(0, 1);
  // on each interval of integration, a thread chooses an arbitrary abscissa
  long double x_left = arg->st_left;
  do {
    long double x_random = x_left + arg->st_step * distr(eng);
    sum += arg->st_func(x_random);
    x_left += arg->st_step;
  } while (x_left + arg->st_step <= arg->st_right);  // we prevent this cycle from completing its last iteration
  sum *= arg->st_step;
  long double x_new_random = x_left + (arg->st_right - x_left) * distr(eng);
  sum += arg->st_func(x_new_random) * (arg->st_right - x_left); // we add a random remaining area
  // the beginning of the access to the critical section, i.e. to a variable 'answer_6_1'
  mutex_6_1.lock();
  answer_6_1 += sum;
  // the end of the access to the critical section, i.e. to a variable 'answer_6_1'
  mutex_6_1.unlock();
  return;
}

void integral_6_2(Args_6_2* arg) {
  // prepare a random number generator
  std::random_device rd;
  std::default_random_engine eng(rd());
  std::uniform_real_distribution<> distr(0, 1);
  // the beginning of the access to the critical section, i.e. to a variable 'count_2dim'
  mutex_6_2.lock();
  for (long j = arg->st_from; j < arg->st_to; j++) {
    // a random abscissa, belonging to the segment [a; b]
    long double x = arg->st_a + (arg->st_b - arg->st_a) * distr(eng);
    // a random ordinate, belonging to the segment [0; 10] (the upper value is taken with a margin)
    long double y = 10.l * distr(eng);
    if (y <= arg->st_func(x))
      count_2dim++;
  }
  // the end of the access to the critical section, i.e. to a variable 'count_2dim'
  mutex_6_2.unlock();
  return;
}

int methods_0_4(methods_t method, long double a, long double b, long double (*func) (long double)) {
  switch (method) {
    case left_rectangle:
      std::cout << "The left rectangle method\n";
      break;
    case right_rectangle:
      std::cout << "The right rectangle method\n";
      break;
    case middle_rectangle:
      std::cout << "The middle rectangle method\n";
      break;
    case trapezoidal:
      std::cout << "The trapezoidal method\n";
      break;
    case Simpson:
      std::cout << "The parabola method (Simpson's method)\n";
      break;
    default:
      std::cerr << "Undefined method.\n";
      return -1;
  }
  std::cout << "Enter the number of threads (not more than 30'000): ";
  int NumOfThreads;
  std::cin >> NumOfThreads;
  if (NumOfThreads < 1) {
    std::cerr << "The number of threads must be positive.\n";
    return -1;
  }
  if (NumOfThreads > 30'000) {
    std::cerr << "The program can't cope with so many threads.\n";
    return -1;
  }

  std::cout << "Enter the step of integration (preferably from 10^(-10) to 10^(-6)): ";
  long double step;
  std::cin >> step;
  if (step <= 0.l) {
    std::cerr << "The step of integration must be positive.\n";
    return -1;
  }
  if ((b - a) / step > (long double) LONG_MAX) {
    std::cerr << "The step is too small: the program can't cope with so many segments of integration.\n";
    return -1;
  }
  
  long double LengthForEachThread = (b - a) / (long double) NumOfThreads;
  if (10.l * step >= LengthForEachThread) {  // each thread should own, at least, 10 subsegments of integration
    std::cerr << "In this scenario each thread owns too few subsegments of integration.\n";
    std::cerr << "Decrease the number of threads or choose a smaller step of integration.\n";
    return -1;
  }

  std::cout << "Wait for the results, please...\n";
  // start the timer
  std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
  result_0_4 = 0.l;  // set an initial value of the integral
  std::vector<std::thread> threads(NumOfThreads);  // create a vector of threads
  std::vector<Args_0_4> VectorOfStructures(NumOfThreads);  // create a vector of structures
  for (int j = 0; j < NumOfThreads; j++) {
    VectorOfStructures[j].st_left = a + j * LengthForEachThread;
    VectorOfStructures[j].st_right = a + (j + 1) * LengthForEachThread;
    VectorOfStructures[j].st_step = step;
    VectorOfStructures[j].st_func = func;
    VectorOfStructures[j].st_method = method;
    // creation of threads
    threads[j] = std::thread(integral_0_4, &VectorOfStructures[j]);
  }
  // waiting for all threads to finish
  for (int j = 0; j < NumOfThreads; j++)
    threads[j].join();
  std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();  // stop the timer
  std::cout << "The result: " << std::setprecision(20) << result_0_4 << std::endl;
  std::cout << "The calculation error is: " << fabsl(result_0_4 - M_PIl) << std::endl;
  std::cout << "It took " << (std::chrono::duration_cast<std::chrono::nanoseconds>(end - begin).count()) / 1'000'000'000.l << " seconds to calculate it.\n\n";
  return 0;
}

void Romberg(long double a, long double b, long double (*func) (long double)) {
  std::cout << "The Romberg method\n";
  std::cout << "Wait for the results, please...\n";
  std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();  // start the timer
  std::vector<std::vector<long double>> Romberg_integral(25, std::vector<long double> (25));  // the implementation of a square lower-triangular matrix
  Romberg_integral.front().front() = 0.5l * (func(a) + func(b)) * (b - a);
  long double Romberg_length = b - a;  // current length of the segment of integration
  for (unsigned long int step = 1; step < Romberg_integral.size(); step++) {
    Romberg_length /= 2.l;
    long double trapezoidal_integral = 0.l;
    long double stepEnd = powl(2.l, static_cast<long double> (step - 1));
    for (unsigned long int tzStep = 1; tzStep <= stepEnd; tzStep++) {
      const long double deltaX = (2.l * tzStep - 1.l) * Romberg_length;
      trapezoidal_integral += func(-1.l + deltaX);
    }
    Romberg_integral[step].front() = 0.5l * Romberg_integral[step - 1].front() + trapezoidal_integral * Romberg_length;
    for (unsigned long int rbStep = 1; rbStep <= step; rbStep++) {
      const long double k = powl(4.l, rbStep);
      Romberg_integral[step][rbStep] = (k * Romberg_integral[step][rbStep - 1] - Romberg_integral[step - 1][rbStep - 1]) / (k - 1);
    }
  }
  std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();  // stop the timer
  std::cout << "The answer: " << std::setprecision(20) << Romberg_integral[24][24] << '\n';
  std::cout << "The calculation error is: " << fabsl(Romberg_integral[24][24] - M_PIl) << std::endl;
  std::cout << "It took " << (std::chrono::duration_cast<std::chrono::nanoseconds>(end - begin).count()) / 1'000'000'000.l << " seconds to calculate it.\n\n";
}

int integrals_Monte_Carlo(long double a, long double b, long double (*func) (long double)) {
  std::cout << "One-dimensional Monte Carlo method\n";
  std::cout << "Enter the number of threads for the first task (not more than 30'000): ";
  int NumOfThreads_1;
  std::cin >> NumOfThreads_1;
  if (NumOfThreads_1 < 1) {
    std::cerr << "The number of threads must be positive.\n";
    return -1;
  }
  if (NumOfThreads_1 > 30'000) {
    std::cerr << "The program can't cope with so many threads.\n";
    return -1;
  }

  std::cout << "Enter the step of integration (preferably from 10^(-10) to 10^(-6)): ";
  long double step;
  std::cin >> step;
  if (step <= 0.l) {
    std::cerr << "The step of integration must be positive.\n";
    return -1;
  }
  if ((b - a) / step > (long double) LONG_MAX) {
    std::cerr << "The step is too small: the program can't cope with so many segments of integration.\n";
    return -1;
  }

  long double LengthForEachThread = (b - a) / (long double) NumOfThreads_1;
  if (10.l * step >= LengthForEachThread) {  // each thread should own, at least, 10 subsegments of integration
    std::cerr << "In this scenario each thread owns too few subsegments of integration.\n";
    std::cerr << "Decrease the number of threads or choose a smaller step of integration.\n";
    return -1;
  }

  std::cout << "Wait for the results, please...\n";
  std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();  // start the timer
  answer_6_1 = 0;  // clear the variable before the experiment
  std::vector<std::thread> threads_1(NumOfThreads_1);  // create a vector of threads
  std::vector<Args_6_1> VectorOfStructures_1(NumOfThreads_1);  // create a vector of structures
  //long NumOfSegments = (long) ((b - a) / step);
  for (int j = 0; j < NumOfThreads_1; j++) {
    VectorOfStructures_1[j].st_left = a + j * LengthForEachThread;
    VectorOfStructures_1[j].st_right = a + (j + 1) * LengthForEachThread;
    VectorOfStructures_1[j].st_step = step;
    VectorOfStructures_1[j].st_func = func;
    // creation of threads
    threads_1[j] = std::thread(integral_6_1, &VectorOfStructures_1[j]);
  }
  // waiting for all threads to finish
  for (int j = 0; j < NumOfThreads_1; j++)
    threads_1[j].join();
  std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();  // stop the timer
  std::cout << "The answer: " << std::setprecision(20) << answer_6_1 << std::endl;
  std::cout << "The calculation error is: " << fabsl(answer_6_1 - M_PIl) << std::endl;
  std::cout << "It took " << (std::chrono::duration_cast<std::chrono::nanoseconds>(end - begin).count()) / 1'000'000'000.l << " seconds to calculate it.\n\n";

  std::cout << "Two-dimensional Monte Carlo method\n";
  std::cout << "Enter the number of threads for the second task (not more than 30'000): ";
  int NumOfThreads_2;
  std::cin >> NumOfThreads_2;
  if (NumOfThreads_2 < 1) {
    std::cerr << "The number of threads must be positive.\n";
    return -1;
  }
  if (NumOfThreads_2 > 30'000) {
    std::cout << "The program can't cope with so many threads.\n";
    return -1;
  }
  
  long NumOfPoints;
  std::cout << "Enter the number of points you want to scatter (not more than 5'000'000): ";
  std::cin >> NumOfPoints;
  if (NumOfPoints < 1L) {
    std::cerr << "The number of points must be positive.\n";
    return -1;
  }
  if (NumOfPoints > 5'000'000L) {
    std::cerr << "The program can't cope with so many points.\n";
    return -1;
  }
  std::cout << "Wait for the results, please...\n";
  begin = std::chrono::steady_clock::now();  // start the timer
  count_2dim = 0;  // clear the variable before the experiment
  long NumOfPointsPerThread = NumOfPoints / NumOfThreads_2;
  std::vector<std::thread> threads_2(NumOfThreads_2);
  std::vector<Args_6_2> VectorOfStructures_2(NumOfThreads_2);
  for (int j = 0; j < NumOfThreads_2; j++) {
    VectorOfStructures_2[j].st_from = j * NumOfPointsPerThread;
    VectorOfStructures_2[j].st_to = (j + 1) * NumOfPointsPerThread;
    VectorOfStructures_2[j].st_a = a;
    VectorOfStructures_2[j].st_b = b;
    VectorOfStructures_2[j].st_func = func;
    threads_2[j] = std::thread(integral_6_2, &VectorOfStructures_2[j]);
  }

  for (int j = 0; j < NumOfThreads_2; j++)
    threads_2[j].join();

  // scatter remaining points (if NumOfPoints isn't evenly divisible by NumOfThreads_2)
  if (NumOfPoints % NumOfThreads_2 != 0) {
    Args_6_2 RemainingPoints;
    RemainingPoints.st_from = NumOfThreads_2 * NumOfPointsPerThread;
    RemainingPoints.st_to = NumOfPoints;
    RemainingPoints.st_a = a;
    RemainingPoints.st_b = b;
    RemainingPoints.st_func = func;
    std::thread remaining_points(integral_6_2, &RemainingPoints);
    remaining_points.join();
  }
  long double answer_2dim = (10.l * (b - a)) * (long double) count_2dim / (long double) NumOfPoints;
  end = std::chrono::steady_clock::now();  // stop the timer
  std::cout << "The answer: " << std::setprecision(15) << answer_2dim << std::endl;
  std::cout << "The calculation error is: " << fabsl(answer_2dim - M_PIl) << std::endl;
  std::cout << "It took " << (std::chrono::duration_cast<std::chrono::nanoseconds>(end - begin).count()) / 1'000'000'000.l << " seconds to calculate it.\n\n";
  return 0;
}