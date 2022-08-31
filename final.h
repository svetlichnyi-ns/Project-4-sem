#ifndef FINAL_H
#define FINAL_H
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <SFML/Window.hpp>
#include <pthread.h>

extern int count;  // the number of intersections of needles and lines
extern sf::VertexArray needles;  // an array of vertices, forming needles
extern pthread_mutex_t mutex;  // the presence of a critical section requires the synchronization of threads

// the arguments of a function, called on a thread, are "wrapped" in a structure (especially for Buffon's needle problem)
typedef struct ArgumentsForSpreader {
  int st_from;  // starting needle number
  int st_to;  // ending needle number
  float st_width;  // the interval between neighbouring lines
  float st_length;  // the length of each needle
  int st_NumOfLines;  // the total number of lines
  bool st_error;  // an indicator of an error inside a function, called on a thread
} Args;

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

class complex_number {  // it is used in a recurrent formula (especially for Mandelbrot's set)
  public:
    long double real, imaginary;
};

void SetButton (sf::RenderWindow& window, sf::Vector2f button_position, std::string button_content, sf::Color text_color);
int Monte_Carlo();
sf::Vector2f elastic_collision (sf::Vector2f initial_velocities, sf::Vector2i masses);
int blocks();
void Mandelbrot_set (sf::VertexArray& vertex_array, int precision, long double zoom, sf::RenderWindow& window);
void Mandelbrot_iterations(long double eps);
int Mandelbrot();
void* spreader(void* args);
int Buffon_needle();
int Viete();
long double function_1 (long double x);
long double function_2 (long double x);
long double function_3 (long double x);
long double Integral (uint8_t method, long double a, long double b, unsigned long N, long double (*func) (long double));
int usage_of_method (uint8_t method, long double a, long double b, unsigned long N, long double (*func) (long double));
int integral_computation();
int spigot();
int calculation_of_pi_via_series (int way, int N);
int series();
void notification(sf::RenderWindow& window);
void reference_information(const std::string &filename, sf::Vector2f size);

#endif