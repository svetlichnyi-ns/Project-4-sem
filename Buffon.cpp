#define _USE_MATH_DEFINES
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <SFML/Window.hpp>
#include <random>
#include <thread>
#include <mutex>
#include <iostream>
#include <iomanip>
#include <chrono>
#include <cmath>
#include <vector>
#include "Buffon.h"

int count;  // the number of intersections of needles and lines
sf::VertexArray needles(sf::Lines, 0);  // an array of vertices, forming needles
std::mutex mutex;  // the presence of a critical section requires the synchronization of threads

// the arguments of a function, called on a thread, are "wrapped" in a structure
typedef struct ArgumentsForSpreader {
  int st_from;  // starting needle number
  int st_to;  // ending needle number
  float st_width;  // the interval between neighbouring lines
  float st_length;  // the length of each needle
  int st_NumOfLines;  // the total number of lines
} Args;

void spreader(void* args) {  // a function, called on a thread
  Args* arg = reinterpret_cast<Args*> (args);
  std::random_device rd;
  std::default_random_engine eng(rd());
  std::uniform_real_distribution<> distr(0, 1);
  for (int j = arg->st_from; j < arg->st_to; j++) {
    // randomly choose an angle for each thrown needle
    float angle = 2.f * M_PIf32 * (float) distr(eng);
    // randomly choose an abscissa for the "left end" of each thrown needle
    float x = 1920.f * (float) distr(eng);
    // randomly choose an ordinate for the "left end" of each thrown needle
    float y = 1080.f * (float) distr(eng);
    needles[2*j].position = sf::Vector2f(x, y);
    needles[2*j].color = sf::Color::Black;
    // the position of the "right end" of a needle is determined by the position of its "left end", the needle's length and its angle
    needles[2*j + 1].position = sf::Vector2f(x + arg->st_length * std::cos(angle), y + arg->st_length * std::sin(angle));
    needles[2*j + 1].color = sf::Color::Black;
  }
  // the beginning of the access to the critical section, i.e. to a variable 'count'
  mutex.lock();
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
  mutex.unlock();
  return;
}

int Buffon_needle() {
  float width;
  std::cout << "Enter the interval between neighbouring lines (don't choose it too small: around 100 will be appropriate): ";
  std::cin >> width;
  if (width <= 0) {
    std::cerr << "The interval between neighbouring lines must be positive.\n";
    return -1;
  }

  int NumOfNeedles;
  std::cout << "Enter the number of needles you want to scatter (no more than 25'000'000): ";
  std::cin >> NumOfNeedles;
  if (NumOfNeedles <= 0) {
    std::cerr << "The number of needles must be positive.\n";
    return -1;
  }
  if (NumOfNeedles > 25'000'000) {
    std::cerr << "The program can't cope with so many needles.\n";
    return -1;
  }

  float length;
  std::cout << "Enter the length of each needle (choose it commensurate with the interval between adjacent lines): ";
  std::cin >> length;
  if (length <= 0) {
    std::cerr << "The length of each needle must be positive.\n";
    return -1;
  }

  int NumOfThreads;
  std::cout << "Enter the preferable number of threads (no more than 25'000): ";
  std::cin >> NumOfThreads;
  if (NumOfThreads <= 0) {
    std::cerr << "The number of threads must be positive.\n";
    return -1;
  }
  if (NumOfThreads > 25'000) {
    std::cerr << "The program does not support so many threads.\n";
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
  
  // new size of the array of vertices, forming needles
  needles.resize(2*NumOfNeedles);
  int NumOfNeedlesPerThread = NumOfNeedles / NumOfThreads;  // the number of needles that each individual thread must scatter
  std::vector <std::thread> threads(NumOfThreads);  // create a vector of threads
  std::vector <Args> VectorOfStructures(NumOfThreads);  // create a vector of structures
  // fill in the fields of all structures
  for (int j = 0; j < NumOfThreads; j++) {
    // needles are subsequently and uniformly distributed among all threads
    VectorOfStructures[j].st_from = j * NumOfNeedlesPerThread;
    VectorOfStructures[j].st_to = (j + 1) * NumOfNeedlesPerThread;
    VectorOfStructures[j].st_length = length;
    VectorOfStructures[j].st_width = width;
    VectorOfStructures[j].st_NumOfLines = NumOfLines;
    // creation of threads
    threads[j] = std::thread(spreader, &VectorOfStructures[j]);
  }
  // waiting for all threads to finish
  for (int j = 0; j < NumOfThreads; j++) {
    threads[j].join();
  }

  // scatter the remaining needles (if NumOfNeedles isn't evenly divisible by NumOfThreads)
  if (NumOfNeedles % NumOfThreads != 0) {
    Args RemainingNeedles;
    RemainingNeedles.st_from = NumOfThreads * NumOfNeedlesPerThread;
    RemainingNeedles.st_to = NumOfNeedles;
    RemainingNeedles.st_length = length;
    RemainingNeedles.st_width = width;
    RemainingNeedles.st_NumOfLines = NumOfLines;
    std::thread remaining_needles(spreader, &RemainingNeedles);
    remaining_needles.join();
  }
  std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();  // stop the timer
  // calculate number PI, based on the probability for each needle to cross any line and on current geometrical parameters
  long double pi = 2.f * length / width / (float) count * (float) NumOfNeedles;
  std::cout << "Number PI is: " << std::setprecision(15) << pi << ";\n";
  std::cout << "The calculation error is: " << fabsl(pi - M_PIl) << ";\n";
  std::cout << "It took " << (std::chrono::duration_cast<std::chrono::nanoseconds>(end - begin).count()) / 1'000'000'000.l << " seconds to calculate it.\n\n";

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
  return 0;
}