#define _USE_MATH_DEFINES
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <SFML/Window.hpp>
#include <pthread.h>
#include <iostream>
#include <iomanip>
#include <chrono>
#include <cmath>
#include <new>
#include "Buffon.h"

int count;  // the number of intersections of needles and lines
sf::VertexArray needles(sf::Lines, 0);  // an array of vertices, forming needles
pthread_mutex_t mutex;  // the presence of a critical section requires the synchronization of threads

void* spreader(void* args) {  // a function, called on a thread
  Args* arg = reinterpret_cast<Args*> (args);
  for (int j = arg->st_from; j < arg->st_to; j++) {
    // randomly choose an angle for each thrown needle
    float angle = 2.f * 3.14159265358979f * static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
    // randomly choose an abscissa for the "left end" of each thrown needle
    float x = 1920.f * static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
    // randomly choose an ordinate for the "left end" of each thrown needle
    float y = 1080.f * static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
    needles[2*j].position = sf::Vector2f(x, y);
    needles[2*j].color = sf::Color::Black;
    // the position of the "right end" of a needle is determined by the position of its "left end", the needle's length and its angle
    needles[2*j + 1].position = sf::Vector2f(x + arg->st_length * std::cos(angle), y + arg->st_length * std::sin(angle));
    needles[2*j + 1].color = sf::Color::Black;
  }
  // the beginning of the access to the critical section, i.e. to a variable 'count'
  if (pthread_mutex_lock(&mutex) != 0) {
    std::cerr << "Failed to lock a mutex!\n";
    arg->st_error = true;  // indicator operation
    return NULL;
  }
  // checking whether the needle crossed the line
  for (int j = arg->st_from; j < arg->st_to; j++) {
    for (int i = 0; i < arg->st_NumOfLines; i++) {
      if ((((float) i * arg->st_width > needles[2*j].position.y) && ((float) i * arg->st_width < needles[2*j + 1].position.y)) ||
          (((float) i * arg->st_width < needles[2*j].position.y) && ((float) i * arg->st_width > needles[2*j + 1].position.y)))
      {
        needles[2*j].color = sf::Color::Red;
        needles[2*j + 1].color = sf::Color::Red;
        count++;
      }
    }
  }
  // the end of the access to the critical section, i.e. to a variable 'count'
  if (pthread_mutex_unlock(&mutex) != 0) {
    std::cerr << "Failed to unlock a mutex!\n";
    arg->st_error = true;  // indicator operation
    return NULL;
  }
  // if there were not any errors, indicator is false
  arg->st_error = false;
  return NULL;
}

int Buffon_needle() {
  float width;
  std::cout << "Enter the interval between neighbouring lines: ";
  std::cin >> width;
  if (width <= 0) {
    std::cerr << "The interval between neighbouring lines must be positive\n";
    return -1;
  }

  int NumOfNeedles;
  std::cout << "Enter the number of needles you want to scatter: ";
  std::cin >> NumOfNeedles;
  if (NumOfNeedles <= 0) {
    std::cerr << "The number of needles must be positive\n";
    return -1;
  }

  float length;
  std::cout << "Enter the length of each needle: ";
  std::cin >> length;
  if (length <= 0) {
    std::cerr << "The length of each needle must be positive\n";
    return -1;
  }

  int NumOfThreads;
  std::cout << "Enter the preferable number of threads: ";
  std::cin >> NumOfThreads;
  if (NumOfThreads <= 0) {
    std::cerr << "The number of threads must be positive\n";
    return -1;
  }

  std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();  // start the timer
  count = 0;  // clear the variable every time when a new experiment starts

  // calculate the number of lines, based on the distance between adjacent ones
  int NumOfLines = static_cast<int>(1080.f / width) + 1;

  sf::VertexArray lines(sf::Lines, 2*NumOfLines);  // create an array of vertices, forming lines
  for (int i = 0; i < NumOfLines; i++) {  // draw lines on a plane
    lines[2*i].position = sf::Vector2f(0.f, (float) i * width);
    lines[2*i].color = sf::Color::Blue;
    lines[2*i + 1].position = sf::Vector2f(1920.f, (float) i * width);
    lines[2*i + 1].color = sf::Color::Blue;
  }
  // the initialization of the mutex
  if (pthread_mutex_init(&mutex, NULL) != 0) {
    std::cerr << "Failed to initialize a mutex!\n";
    return -1;
  }
  // new size of the array of vertices, forming needles
  needles.resize(2*NumOfNeedles);
  int NumOfNeedlesPerThread = NumOfNeedles / NumOfThreads;  // the number of needles that each individual thread must scatter
  // create an array of threads' identificators
  pthread_t* threads;
  try {
    threads = new pthread_t [NumOfThreads];
  }
  catch (const std::bad_alloc& e) {
    std::cerr << "Failed to allocate memory for an array of threads' identificators: " << e.what() << std::endl;
    return -1; 
  }
  // create an array of structures
  Args* ArrayOfStructures;
  try {
    ArrayOfStructures = new Args [NumOfThreads];
  }
  catch (const std::bad_alloc& e) {
    std::cerr << "Failed to allocate memory for an array of structures: " << e.what() << std::endl;
    delete [] threads;
    return -1;
  }
  // fill in the fields of all structures
  for (int j = 0; j < NumOfThreads; j++) {
    // needles are subsequently and uniformly distributed among all threads
    ArrayOfStructures[j].st_from = j * NumOfNeedlesPerThread;
    ArrayOfStructures[j].st_to = (j + 1) * NumOfNeedlesPerThread;
    ArrayOfStructures[j].st_length = length;
    ArrayOfStructures[j].st_width = width;
    ArrayOfStructures[j].st_NumOfLines = NumOfLines;
    // creation of threads
    if (pthread_create(&threads[j], NULL, spreader, &ArrayOfStructures[j]) != 0) {
      std::cerr << "Failed to create a thread!\n";
      delete [] threads;
      delete [] ArrayOfStructures;
      return -1;
    }
    // checking whether an error occurred within a function
    if (ArrayOfStructures[j].st_error == true) {
      delete [] threads;
      delete [] ArrayOfStructures;
      return -1;
    }
  }
  // waiting for all threads to finish
  for (int j = 0; j < NumOfThreads; j++) {
    if (pthread_join(threads[j], NULL) != 0) {
      std::cerr << "Failed to join a thread!\n";
      delete [] threads;
      delete [] ArrayOfStructures;
      return -1;
    }
  }
  
  // scatter the remaining needles (if NumOfNeedles isn't evenly divisible by NumOfThreads)
  if (NumOfNeedles % NumOfThreads != 0) {
    pthread_t remaining_needles;
    Args RemainingNeedles;
    RemainingNeedles.st_from = NumOfThreads * NumOfNeedlesPerThread;
    RemainingNeedles.st_to = NumOfNeedles;
    RemainingNeedles.st_length = length;
    RemainingNeedles.st_width = width;
    RemainingNeedles.st_NumOfLines = NumOfLines;
    if (pthread_create(&remaining_needles, NULL, spreader, &RemainingNeedles) != 0) {
      std::cerr << "Failed to create a thread!\n";
      delete [] threads;
      delete [] ArrayOfStructures;
      return -1;
    }
    if (RemainingNeedles.st_error == true) {
      delete [] threads;
      delete [] ArrayOfStructures;
      return -1;
    }
    if (pthread_join(remaining_needles, NULL) != 0) {
      std::cerr << "Failed to join a thread!\n";
      delete [] threads;
      delete [] ArrayOfStructures;
      return -1;
    }
  }
  std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();  // stop the timer
  // calculate number pi, based on the probability for each needle to cross any line and on current geometrical parameters
  long double pi = 2.f * length / width / (float) count * (float) NumOfNeedles;
  std::cout << "Number PI is: " << std::setprecision(15) << pi << ";\n";
  std::cout << "The calculation error is: " << fabsl(pi - M_PIl) << ";\n";
  std::cout << "It took " << (std::chrono::duration_cast<std::chrono::nanoseconds>(end - begin).count()) / 1'000'000'000.l << " seconds to calculate it.\n\n";

  // the destroyment of the mutex
  if (pthread_mutex_destroy(&mutex) != 0) {
    std::cerr << "Failed to destroy a mutex!\n";
    delete [] threads;
    delete [] ArrayOfStructures;
    return -1;
  }

  sf::RenderWindow window(sf::VideoMode(1920, 1080), "Buffon's needle");
  window.clear(sf::Color::White);
  window.draw(lines);
  window.draw(needles);
  window.display();
  while (window.isOpen()) {
    sf::Event event;
    while (window.pollEvent(event)) {
      if (event.type == sf::Event::Closed)
        window.close();
    }
  }
  delete [] threads;
  delete [] ArrayOfStructures;
  return 0;
}