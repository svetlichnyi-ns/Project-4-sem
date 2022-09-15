This project is dedicated to demonstration of different ways to calculate the number pi. The presented methods, leading to its computation, are as follows:
1) Monte Carlo methods:  
1.1) Buffon's needle problem. It will be accompanied with a graphic interpretation, and the user may choose concrete values of parameters (needle's length and width, distance between adjacent lines in the field, number of thrown needles) and to trace the dependence of the calculation accuracy on the aforementioned parameters.  
1.2) Method of uniformly scattering a particular number of points over the square, which is circumscribed about a circle. It's also feasible to repeat the same experiment with other regular polygons.  
2) Atlantic City method for computing pi.  
3) Computation of pi via colliding blocks (including its graphic demonstration).  
4) An application of infinite series to calculation of pi:  
4.1) Leibniz's series;  
4.2) Wallis's series;  
4.3) Nilakantha's series;  
4.4) Viete's series;  
4.5) Ramanujan's series.  
5) Calculating of pi by numerical integration. Using different definite integrals as examples, I intend to demonstrate various techniques of parallelization of programs.  
6) Some narrowly known algorithms:  
6.1) A spigot algorithm for the digits of pi;  
6.2) Bailey–Borwein–Plouffe formula (computation of N'th digit without calculating previous ones);  
6.3) Pi approximation using the Mandelbrot set.  

The programming language, used in this project, is C++. For graphic demonstartion of some above-mentioned algorithms, the library SFML is going to be used.

As a result, I created a program that supports eight methods of computation of number PI (excluding some abovementioned planned ones and including some new ones). These are:  
1) Buffon's needle problem;  
2) Scenario with colliding blocks;  
3) Numerical integration;  
4) Series summation;  
5) Mandelbrot's set;  
6) A spigot algorithm;  
7) Monte-Carlo method;  
8) Viete's formula.  
  
Before running a project, you should install the SFML library. For example, on Linux you should write the following:  sudo apt-get install libsfml-dev  
Then clone this repository to create a local copy on your computer:  
git clone https://github.com/svetlichnyi-ns/Project-4-sem  
So as to run a project, enter the following to the console:  
make  
./project  
If you're willing to delete object files and an executable file, enter:  
make clean
