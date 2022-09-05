#ifndef BUFFON_H
#define BUFFON_H
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <SFML/Window.hpp>
#include <thread>
#include <mutex>

extern int count;  // the number of intersections of needles and lines
extern sf::VertexArray needles;  // an array of vertices, forming needles
extern std::mutex mutex;  // the presence of a critical section requires the synchronization of threads

// the arguments of a function, called on a thread, are "wrapped" in a structure
typedef struct ArgumentsForSpreader {
  int st_from;  // starting needle number
  int st_to;  // ending needle number
  float st_width;  // the interval between neighbouring lines
  float st_length;  // the length of each needle
  int st_NumOfLines;  // the total number of lines
} Args;

void spreader(void* args);
int Buffon_needle();

#endif