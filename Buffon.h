#ifndef BUFFON_H
#define BUFFON_H
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <SFML/Window.hpp>
#include <pthread.h>

extern int count;  // the number of intersections of needles and lines
extern sf::VertexArray needles;  // an array of vertices, forming needles
extern pthread_mutex_t mutex;  // the presence of a critical section requires the synchronization of threads

// the arguments of a function, called on a thread, are "wrapped" in a structure
typedef struct ArgumentsForSpreader {
  int st_from;  // starting needle number
  int st_to;  // ending needle number
  float st_width;  // the interval between neighbouring lines
  float st_length;  // the length of each needle
  int st_NumOfLines;  // the total number of lines
} Args;

// this function is called on each thread
void* spreader(void* args);
// this function is responsible for interaction with user, creation of threads, summarizing and conclusion of results
int Buffon_needle();

#endif