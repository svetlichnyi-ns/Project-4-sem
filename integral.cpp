#include <iostream>
#include <iomanip>
#include <thread>
#include <chrono>
#include <cmath>
#include <mutex>
#include <random>
#include <vector>
#include "integral.h"

long double result;  // a global variable, common for all threads (used in methods 0-4)
std::mutex mutex_0_4;  // it's used for threads' synchronization (used in methods 0-4)

// the arguments of a function, called on a thread, are "wrapped" in a structure (used in methods 0-4)
typedef struct {
  int st_from, st_to;  // initial and final segments of integration for a given thread
  long double st_a;  // a left limit of integration
  long double st_step;  // the length of each segment of integration
  long double (*st_func) (long double);  // the function whose integral is calculated
  int st_method;  // number of method, which is used for integral's calculation
} Args;

// a function, called on a thread (used in methods 0-4)
void integral(void* args) {
  Args* arg = reinterpret_cast<Args*> (args);
  long double sum = 0.l;
  switch (arg->st_method) {
    case 0:  // left rectangle method
      for (int i = arg->st_from; i < arg->st_to; i++) {
        long double x = arg->st_a + i * arg->st_step;
        sum += arg->st_func(x);
      }
      break;
    case 1:  // right rectangle method
      for (int i = arg->st_from; i < arg->st_to; i++) {
        long double x = arg->st_a + (i + 1) * arg->st_step;
        sum += arg->st_func(x);
      }
      break;
    case 2:  // middle rectangle method
      for (int i = arg->st_from; i < arg->st_to; i++) {
        long double x = arg->st_a + (static_cast<long double> (i) + 0.5l) * arg->st_step;
        sum += arg->st_func(x);
      }
      break;
    case 3:  // trapezoidal method
      for (int i = arg->st_from; i < arg->st_to; i++) {
        long double x_1 = arg->st_a + i * arg->st_step;
        long double x_2 = arg->st_a + (i + 1) * arg->st_step;
        sum += arg->st_func(x_1) + arg->st_func(x_2);
      }
      sum /= 2.l;
      break;
    case 4:  // parabola method (Simpson's method)
      for (int i = arg->st_from; i < arg->st_to; i++) {
        long double x_left = arg->st_a + i * arg->st_step;
        long double x_middle = arg->st_a + (static_cast<long double> (i) + 0.5l) * arg->st_step;
        long double x_right = arg->st_a + (i + 1) * arg->st_step;
        sum += arg->st_func(x_left) + 4.l * arg->st_func(x_middle) + arg->st_func(x_right);
      }
      sum /= 6.l;
      break;
    default:
      std::cerr << "Undefined method\n";
      return;
  }
  // the beginning of the access to the critical section, i.e. to a variable 'answer'
  mutex_0_4.lock();
  result += sum * arg->st_step;
  // the end of the access to the critical section, i.e. to a variable 'answer'
  mutex_0_4.unlock();
  return;
}

long double answer;  // a global variable, used in one-dimensional Monte Carlo method
int count_2dim;  // a global variable, used in two-dimensional Monte Carlo method
std::mutex mutex_1;  // it's used for synchronization of threads, working on the first task
std::mutex mutex_2;  // it's used for synchronization of threads, working on the second task

// arguments for a function, called on a thread, working on the first task
typedef struct {
  int st_from, st_to;  // initial and final segments of integration for a given thread
  long double st_a;  // a left limit of integration
  long double st_step;  // the length of each segment of integration
  long double (*st_func) (long double);  // the function whose integral is calculated
} Args_1;

// arguments for a function, called on a thread, working on the second task
typedef struct {
  int st_from, st_to;  // initial and final numbers of points, scattered by a given thread
  long double st_a, st_b;  // left and right limits of integration
  long double (*st_func) (long double);  // the function whose integral is calculated
} Args_2;

// the function whose integral is calculated
long double function_integral (long double x) {
  return 2.l * sqrtl(1.l - powl(x, 2.l));
}

// a function, called on a thread, working on the first task
void one_dim_integral(void* args) {
  Args_1* arg = reinterpret_cast<Args_1*> (args);
  long double sum = 0.l;
  std::random_device rd;
  std::default_random_engine eng(rd());
  std::uniform_real_distribution<> distr(0, 1);
  for (int i = arg->st_from; i < arg->st_to; i++) {
    // on each interval of integration, a thread chooses an arbitrary abscissa
    long double x_random = arg->st_a + i * arg->st_step + arg->st_step * distr(eng);
    sum += arg->st_func(x_random);
  }
  // the beginning of the access to the critical section, i.e. to a variable 'answer'
  mutex_1.lock();
  answer += sum * arg->st_step;
  // the end of the access to the critical section, i.e. to a variable 'answer'
  mutex_1.unlock();
  return;
}

// a function, called on a thread, working on the second task
void two_dim_integral(void* args) {
  Args_2* arg = reinterpret_cast<Args_2*> (args);
  std::random_device rd;
  std::default_random_engine eng(rd());
  std::uniform_real_distribution<> distr(0, 1);
  // the beginning of the access to the critical section, i.e. to a variable 'count_2dim'
  mutex_2.lock();
  for (int j = arg->st_from; j < arg->st_to; j++) {
    // a random abscissa, belonging to the segment [a; b]
    long double x = arg->st_a + (arg->st_b - arg->st_a) * distr(eng);
    // a random ordinate, belonging to the segment [0; 10] (the upper value is taken with a margin)
    long double y = 10.l * distr(eng);
    if (y <= arg->st_func(x))
      count_2dim++;
  }
  // the end of the access to the critical section, i.e. to a variable 'count_2dim'
  mutex_2.unlock();
  return;
}

// a function that is called if the user chooses one of the first five methods
int integral_methods_0_4(long double method) {
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

  std::cout << "Enter the number of segments of integration (not more than 2'000'000'000): ";
  int NumOfSegments;
  std::cin >> NumOfSegments;
  if (NumOfSegments < 1) {
    std::cerr << "The number of segments of integration must be positive.\n";
    return -1;
  }
  if (NumOfSegments > 2'000'000'000) {
    std::cerr << "The program can't cope with so many segments of integration.\n";
    return -1;
  }

  // start the timer
  std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
  result = 0.l;  // set an initial value of the integral
  std::vector<std::thread> threads(NumOfThreads);  // create a vector of threads
  std::vector<Args> VectorOfStructures(NumOfThreads);  // create a vector of structures
  int NumOfSegmentsPerThread = NumOfSegments / NumOfThreads;
  long double a = -1.l, b = 1.l;  // set limits of integration
  long double step = (b - a) / NumOfSegments;
  for (int j = 0; j < NumOfThreads; j++) {
    VectorOfStructures[j].st_from = j * NumOfSegmentsPerThread;
    VectorOfStructures[j].st_to = (j + 1) * NumOfSegmentsPerThread;
    VectorOfStructures[j].st_a = a;
    VectorOfStructures[j].st_step = step;
    VectorOfStructures[j].st_func = function_integral;
    VectorOfStructures[j].st_method = static_cast<int>(method);
    // creation of threads
    threads[j] = std::thread(integral, &VectorOfStructures[j]);
  }
  // waiting for all threads to finish
  for (int j = 0; j < NumOfThreads; j++) {
    threads[j].join();
  }
  // integrate over the remaining segments (if NumOfSegments isn't evenly divisible by NumOfThreads)
  if (NumOfSegments % NumOfThreads != 0) {
    Args LastThread;  // a structure instance, created especially for the last thread 
    LastThread.st_from = NumOfThreads * NumOfSegmentsPerThread;
    LastThread.st_to = NumOfSegments;
    LastThread.st_a = a;
    LastThread.st_step = step;
    LastThread.st_func = function_integral;
    LastThread.st_method = static_cast<int>(method);
    std::thread last_thread(integral, &LastThread);
    last_thread.join();
  }
  std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();  // stop the timer
  std::cout << "The result: " << std::setprecision(20) << result << '\n';
  std::cout << "It took " << (std::chrono::duration_cast<std::chrono::nanoseconds>(end - begin).count()) / 1'000'000'000.l << " seconds to calculate it.\n\n";
  return 0;
}

// a function that is called if the user chooses Monte Carlo methods (number 6)
int integrals_Monte_Carlo() {
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

  std::cout << "Enter the number of segments of integration (not more than 2'000'000'000): ";
  int NumOfSegments;
  std::cin >> NumOfSegments;
  if (NumOfSegments < 1) {
    std::cout << "The number of segments of integration must be positive.\n";
    return -1;
  }
  if (NumOfSegments > 2'000'000'000) {
    std::cout << "The program can't cope with so many segments of integration.\n";
    return -1;
  }

  std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();  // start the timer
  answer = 0;  // clear the variable before the experiment
  std::vector<std::thread> threads_1(NumOfThreads_1);  // create a vector of threads
  std::vector<Args_1> VectorOfStructures_1(NumOfThreads_1);  // create a vector of structures
  int NumOfSegmentsPerThread = NumOfSegments / NumOfThreads_1;
  long double a = -1.l, b = 1.l;  // set limits of integration
  long double step = (b - a) / NumOfSegments;
  for (int j = 0; j < NumOfThreads_1; j++) {
    VectorOfStructures_1[j].st_from = j * NumOfSegmentsPerThread;
    VectorOfStructures_1[j].st_to = (j + 1) * NumOfSegmentsPerThread;
    VectorOfStructures_1[j].st_a = a;
    VectorOfStructures_1[j].st_step = step;
    VectorOfStructures_1[j].st_func = function_integral;
    // creation of threads
    threads_1[j] = std::thread(one_dim_integral, &VectorOfStructures_1[j]);
  }
  // waiting for all threads to finish
  for (int j = 0; j < NumOfThreads_1; j++) {
    threads_1[j].join();
  }
  // integrate over the remaining segments (if NumOfSegments isn't evenly divisible by NumOfThreads_1)
  if (NumOfSegments % NumOfThreads_1 != 0) {
    Args_1 LastThread;  // a structure instance, created especially for the last thread
    LastThread.st_from = NumOfThreads_1 * NumOfSegmentsPerThread;
    LastThread.st_to = NumOfSegments;
    LastThread.st_a = a;
    LastThread.st_step = step;
    LastThread.st_func = function_integral;
    std::thread last_thread(one_dim_integral, &LastThread);
    last_thread.join();
  }
  std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();  // stop the timer
  std::cout << "The answer: " << std::setprecision(20) << answer << '\n';
  std::cout << "It took " << (std::chrono::duration_cast<std::chrono::nanoseconds>(end - begin).count()) / 1'000'000'000.l << " seconds to calculate it.\n\n";

  std::cout << "Two-dimensional Monte Carlo method\n";
  std::cout << "Enter the number of threads for the second task (not more than 30'000): ";
  int NumOfThreads_2;
  std::cin >> NumOfThreads_2;
  if (NumOfThreads_2 < 1) {
    std::cerr << "The number of threads must be positive\n";
    return -1;
  }
  if (NumOfThreads_2 > 30'000) {
    std::cout << "The program can't cope with so many threads.\n";
    return -1;
  }
  
  int NumOfPoints;
  std::cout << "Enter the number of points you want to scatter (not more than 15'000'000): ";
  std::cin >> NumOfPoints;
  if (NumOfPoints < 1) {
    std::cerr << "The number of points must be positive\n";
    return -1;
  }
  if (NumOfPoints > 15'000'000) {
    std::cerr << "The program can't cope with so many points.\n";
    return -1;
  }

  begin = std::chrono::steady_clock::now();  // start the timer
  count_2dim = 0;  // clear the variable before the experiment
  int NumOfPointsPerThread = NumOfPoints / NumOfThreads_2;
  std::vector<std::thread> threads_2(NumOfThreads_2);
  std::vector<Args_2> VectorOfStructures_2(NumOfThreads_2);
  for (int j = 0; j < NumOfThreads_2; j++) {
    VectorOfStructures_2[j].st_from = j * NumOfPointsPerThread;
    VectorOfStructures_2[j].st_to = (j + 1) * NumOfPointsPerThread;
    VectorOfStructures_2[j].st_a = a;
    VectorOfStructures_2[j].st_b = b;
    VectorOfStructures_2[j].st_func = function_integral;
    threads_2[j] = std::thread(two_dim_integral, &VectorOfStructures_2[j]);
  }

  for (int j = 0; j < NumOfThreads_2; j++) {
    threads_2[j].join();
  }

  // scatter remaining points (if NumOfPoints isn't evenly divisible by NumOfThreads_2)
  if (NumOfPoints % NumOfThreads_2 != 0) {
    Args_2 RemainingPoints;
    RemainingPoints.st_from = NumOfThreads_2 * NumOfPointsPerThread;
    RemainingPoints.st_to = NumOfPoints;
    RemainingPoints.st_a = a;
    RemainingPoints.st_b = b;
    RemainingPoints.st_func = function_integral;
    std::thread remaining_points(two_dim_integral, &RemainingPoints);
    remaining_points.join();
  }

  end = std::chrono::steady_clock::now();  // stop the timer
  std::cout << "The answer: " << std::setprecision(15) << (10.l * (b - a)) * (long double) count_2dim / (long double) NumOfPoints;
  std::cout << "\nIt took " << (std::chrono::duration_cast<std::chrono::nanoseconds>(end - begin).count()) / 1'000'000'000.l << " seconds to calculate it.\n";
  return 0;
}

// a function that is called if the user chooses Romberg method (number 5)
void Romberg() {
  std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();  // start the timer
  std::vector<std::vector<long double>> Romberg_integral(25, std::vector<long double> (25));  // the implementation of a square lower-triangular matrix
  Romberg_integral.front().front() = 0.5l * (function_integral(-1.l) + function_integral(1.l)) * (1.l - (-1.l));
  long double Romberg_length = 1.l - (-1.l);  // current length of the segment of integration
  for (unsigned long int step = 1; step < Romberg_integral.size(); step++) {
    Romberg_length /= 2.l;
    long double trapezoidal_integral = 0.l;
    long double stepEnd = powl(2.l, static_cast<long double> (step - 1));
    for (unsigned long int tzStep = 1; tzStep <= stepEnd; tzStep++) {
      const long double deltaX = (2.l * tzStep - 1.l) * Romberg_length;
      trapezoidal_integral += function_integral(-1.l + deltaX);
    }
    Romberg_integral[step].front() = 0.5l * Romberg_integral[step - 1].front() + trapezoidal_integral * Romberg_length;
    for (unsigned long int rbStep = 1; rbStep <= step; rbStep++) {
      const long double k = powl(4.l, rbStep);
      Romberg_integral[step][rbStep] = (k * Romberg_integral[step][rbStep - 1] - Romberg_integral[step - 1][rbStep - 1]) / (k - 1);
    }
  }
  std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();  // stop the timer
  std::cout << "The answer: " << std::setprecision(20) << Romberg_integral[24][24] << '\n';
  std::cout << "It took " << (std::chrono::duration_cast<std::chrono::nanoseconds>(end - begin).count()) / 1'000'000'000.l << " seconds to calculate it.\n\n";
}

// a main function that interacts with user and launches a chosen experiment
int integral_computation() {
  std::cout << "Enter the number of method that you would like to use for integral's calculation:\n";
  std::cout << "0 - left rectangle method;\n";
  std::cout << "1 - right rectangle method;\n";
  std::cout << "2 - middle rectangle method;\n";
  std::cout << "3 - trapezoidal method;\n";
  std::cout << "4 - parabola method (Simpson's method);\n";
  std::cout << "5 - Romberg method;\n";
  std::cout << "6 - one-dimensional and two-dimensional Monte-Carlo methods;\n";
  int method;
  std::cin >> method;
  if ((method == 0) || (method == 1) || (method == 2) || (method == 3) || (method == 4)) {
    if (integral_methods_0_4(method) == -1)
      return -1;
  }
  else if (method == 5)  // this function is completely demonstrative; thus, it can't return any errors
    Romberg();
  else if (method == 6) {
    if (integrals_Monte_Carlo() == -1)
      return -1;
  }
  else {
    std::cerr << "This method is undefined; use one of available methods.\n";
    return -1;
  }
  return 0;
}