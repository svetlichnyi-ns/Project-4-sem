#include <iostream>
#include <iomanip>
#include <pthread.h>
#include <cmath>
#include <assert.h>
#include <chrono>
#include <omp.h>
#include "final.h"

#define PI_25_DIGITS 3.141592653589793238462643

int count;  // the number of intersections of needles and lines
sf::VertexArray needles;  // an array of vertices, forming needles
pthread_mutex_t mutex;  // the presence of a critical section requires the synchronization of threads

// this function draws a button with an inscription on it
void SetButton (sf::RenderWindow& window, sf::Vector2f button_position, std::string button_content, sf::Color text_color) {
  sf::RectangleShape button;
  button.setSize(sf::Vector2f(800.f, 150.f));
  button.setFillColor(sf::Color::White);
  button.setOrigin(sf::Vector2f(button.getSize().x / 2.f, button.getSize().y / 2.f));
  button.setPosition(button_position);
  
  sf::Font font;
  font.loadFromFile ("fonts/Times_New_Roman.ttf");

  sf::Text text;
  text.setFont (font);
  text.setCharacterSize (80);
  text.setFillColor (text_color);
  text.setString ((sf::String) button_content);
  // so as to centralize the text on the button
  sf::FloatRect textRect = text.getLocalBounds();
  text.setOrigin(textRect.left + textRect.width / 2.f, textRect.top + textRect.height / 2.f);
  text.setPosition (button_position);

  window.draw(button);
  window.draw(text);
  return;
}

// this function completely implements Monte Carlo algorithm (file "scattering.cpp")
int Monte_Carlo() {
  sf::RenderWindow window(sf::VideoMode(1920, 1080), "Scattering of points");
  window.setFramerateLimit(30);  // this frame rate provides both optimal perception and rather high computational performance

  sf::RectangleShape square(sf::Vector2f(800.f, 800.f));  // creating a square
  square.setOrigin(sf::Vector2f(square.getSize().x / 2, square.getSize().y / 2));
  square.setPosition(sf::Vector2f(static_cast<float>(window.getSize().x / 2), static_cast<float>(window.getSize().y / 2)));
  square.setOutlineThickness(10.f);
  square.setOutlineColor(sf::Color::Yellow);
  window.draw(square);

  sf::RectangleShape patch(sf::Vector2f(static_cast<float>(window.getSize().x), 80.f));  // creating a black patch at the top of the screen: without it, previous inscriptions would overlap each other
  patch.setPosition(sf::Vector2f(0.f, 0.f));
  patch.setFillColor(sf::Color::Black);

  sf::CircleShape circle;  // creating a circle
  circle.setOrigin(sf::Vector2f(square.getSize().x / 2, square.getSize().y / 2));
  circle.setPosition(sf::Vector2f(static_cast<float>(window.getSize().x / 2), static_cast<float>(window.getSize().y / 2)));
  circle.setOutlineThickness(2.f);
  circle.setRadius(square.getSize().x / 2 - circle.getOutlineThickness());
  circle.setOutlineColor(sf::Color::Black);
  circle.setPointCount(10000000);  // number of triangles, forming a circle, which is close to an "ideal" one
  window.draw(circle);

  int iterations = 0;  // the total number of points, depicted on the screen

  sf::Font font;
  font.loadFromFile("fonts/a_Futurica_ExtraBold.ttf");

  sf::Text text;
  text.setFont(font);
  text.setCharacterSize(50);
  text.setFillColor(sf::Color::White);
  text.setPosition(sf::Vector2f(350.f, 15.f));

  int blue_points = 0;  // number of points, depicted inside the circle
  
  while (window.isOpen()) {
    sf::Event event;
    while (window.pollEvent(event)) {
      if (event.type == sf::Event::Closed)
        window.close();
    }

    sf::CircleShape point(2.f, 100000);  // we actually draw not points, but tiny circles, so that they are seen for us
    point.setOrigin(point.getRadius(), point.getRadius());
    point.setPosition(sf::Vector2f((float) (window.getSize().x / 2) - square.getSize().x / 2 + square.getSize().x * static_cast<float>(rand()) / static_cast<float>(RAND_MAX),
                                   (float) (window.getSize().y / 2) - square.getSize().y / 2 + square.getSize().y * static_cast<float>(rand()) / static_cast<float>(RAND_MAX)));
    if (pow(square.getPosition().x - point.getPosition().x, 2.f) + pow(square.getPosition().y - point.getPosition().y, 2.f) <= pow(circle.getRadius() + circle.getOutlineThickness(), 2.f)) {
      point.setFillColor(sf::Color::Blue);  // if the point occurs inside the circle or on its border, it becomes blue
      blue_points++;
    }
    else
      point.setFillColor(sf::Color::Red); // if the point occurs outside the circle, it becomes red

    iterations++;
    std::ostringstream num_of_iterations;
    num_of_iterations << iterations;
    std::ostringstream pi;
    pi << 4 * (long double) blue_points / iterations; // number pi is calculated via the probability for the point not to occur outside the circle
    text.setString ("Number of iterations: " + num_of_iterations.str() + "; number pi = " + pi.str());
    window.draw(point);
    window.draw(text);
    window.display();
    window.draw(patch);
  }
  return 0;
}

// this function calculates blocks' velocities after their absolutely elastic collision
sf::Vector2f elastic_collision (sf::Vector2f initial_velocities, sf::Vector2i masses) {
  sf::Vector2f final_velocities;
  // these formulas are the result of solving a system of equations, consisting of the laws of conservation of energy and momentum
  final_velocities.x = (2 * (float) masses.y * initial_velocities.y + (float) (masses.x - masses.y) * initial_velocities.x) / (float) (masses.x + masses.y);
  final_velocities.y = (2 * (float) masses.x * initial_velocities.x + (float) (masses.y - masses.x) * initial_velocities.y) / (float) (masses.x + masses.y);
  return final_velocities;
}

// this function completely implements scenario with colliding blocks (file "blocks.cpp")
int blocks() {
  std::cout << "Before entering masses, consider that the second block's mass should exceed the first block's mass by 1, 100, 10000 or 1000000 times.\n";
  
  int mass_1;
  std::cout << "Enter the mass of the first block: ";
  std::cin >> mass_1;
  assert(mass_1 > 0 && "The mass must be strictly positive");
  
  int mass_2;
  std::cout << "Enter the mass of the second block: ";
  std::cin >> mass_2;
  assert(mass_2 > 0 && "The mass must be strictly positive");

  float speed_1 = 0.f, speed_2;  // at the beginning, the left block is motionless, whereas the right block moves towards the left one

  sf::RenderWindow window (sf::VideoMode(1920, 1080), "Colliding blocks!");
  window.setFramerateLimit (30);

  sf::RectangleShape ground;
  ground.setSize (sf::Vector2f((float) window.getSize().x, 0.5f * (float) window.getSize().y));
  // set the position of the upper left corner of 'ground'
  ground.setPosition (sf::Vector2f(0.f, (float) window.getSize().y - ground.getSize().y));
  sf::Texture ground_texture;
  ground_texture.loadFromFile ("pictures/ground.jpg");
  ground.setTexture (&ground_texture);
  
  sf::RectangleShape sky;
  sky.setSize (sf::Vector2f((float) window.getSize().x, (float) window.getSize().y - ground.getSize().y));
  // set the position of the upper left corner of 'sky'
  sky.setPosition (sf::Vector2f(0.f, 0.f));
  sf::Texture sky_texture;
  sky_texture.loadFromFile ("pictures/sky.jpg");
  sky.setTexture (&sky_texture);
  
  sf::RectangleShape wall;
  wall.setSize (sf::Vector2f(0.3f * (float) window.getSize().x, (float) window.getSize().y - ground.getSize().y));
  // set the position of the upper left corner of 'wall'
  wall.setPosition (sf::Vector2f(0.f, 0.f));
  sf::Texture wall_texture;
  wall_texture.loadFromFile ("pictures/brick.jpg");
  wall.setTexture (&wall_texture);

  sf::RectangleShape block_1;
  block_1.setSize (sf::Vector2f(150.f, 150.f));
  block_1.setFillColor (sf::Color::Red);
  // now the first block's origin is located in its center
  block_1.setOrigin (sf::Vector2f(block_1.getSize().x / 2.f, block_1.getSize().y / 2.f));
  
  sf::RectangleShape block_2;
  block_2.setSize (sf::Vector2f(250.f, 250.f));
  block_2.setFillColor (sf::Color::Yellow);
  // now the second block's origin is located in its center
  block_2.setOrigin (sf::Vector2f(block_2.getSize().x / 2.f, block_2.getSize().y / 2.f));

  // we adjust the experiment's parameters so that the demonstration is the best in each concrete case
  switch (mass_2 / mass_1) {
    case 1:
      speed_2 = -200.f;
      block_1.setPosition (sf::Vector2f(800.f, wall.getSize().y - block_1.getSize().y / 2.f));
      block_2.setPosition (sf::Vector2f(1200.f, wall.getSize().y - block_2.getSize().y / 2.f));
      break;
    case 100:
      speed_2 = -20.f;
      block_1.setPosition (sf::Vector2f(700.f, wall.getSize().y - block_1.getSize().y / 2.f));
      block_2.setPosition (sf::Vector2f(1000.f, wall.getSize().y - block_2.getSize().y / 2.f));
      break;
    case 10000:
      speed_2 = -2.f;
      block_1.setPosition (sf::Vector2f(700.f, wall.getSize().y - block_1.getSize().y / 2.f));
      block_2.setPosition (sf::Vector2f(950.f, wall.getSize().y - block_2.getSize().y / 2.f));
      break;
    case 1000000:
      speed_2 = -0.1f;
      block_1.setPosition (sf::Vector2f(650.f, wall.getSize().y - block_1.getSize().y / 2.f));
      block_2.setPosition (sf::Vector2f(860.f, wall.getSize().y - block_2.getSize().y / 2.f));
      break;
    default:
      std::cerr << "Please, choose one of possible ratios between the masses\n";
      return -1;
  }

  sf::Font font;
  font.loadFromFile ("fonts/Times_New_Roman.ttf");

  sf::Text text;
  text.setFont (font);
  text.setCharacterSize (100);
  text.setFillColor (sf::Color::Black);
  text.setPosition (sf::Vector2f(700.f, 50.f));

  std::ostringstream initial_inscription;
  initial_inscription << 0;
  text.setString ("Number of collisions: " + initial_inscription.str());

  sf::Clock clock;  // start the timer
  int collisions = 0;  // total number of collisions
  while (window.isOpen()) {
    sf::Event event;
    while (window.pollEvent(event)) {
      if (event.type == sf::Event::Closed)
        window.close();
    }
    window.clear();
    window.draw (ground);
    window.draw (sky);
    window.draw (wall);
    window.draw (text);
    window.draw (block_1);
    window.draw (block_2);
    window.display();
    // at each iteration, we move the blocks, based on their current velocities
    block_1.move (sf::Vector2f(speed_1 * clock.getElapsedTime().asSeconds(), 0.f));
    block_2.move (sf::Vector2f(speed_2 * clock.restart().asSeconds(), 0.f));
    // checking the collision condition of two blocks
    if (fabs(block_1.getPosition().x - block_2.getPosition().x) <= (block_1.getSize().x + block_2.getSize().x) / 2.f) {
      collisions++;  // increment variable 'collisions'
      float prev_speed_1 = speed_1;  // memorize the previous speed of the first block in a temporary variable
      // recalculate blocks' velocities
      speed_1 = elastic_collision (sf::Vector2f(speed_1, speed_2), sf::Vector2i(mass_1, mass_2)).x;
      speed_2 = elastic_collision (sf::Vector2f(prev_speed_1, speed_2), sf::Vector2i(mass_1, mass_2)).y;
      do {  // here we move blocks with recalculated velocities, until the collision criterion is no longer met
        window.clear();
        window.draw(ground);
        window.draw(sky);
        window.draw(wall);
        std::ostringstream num_of_collisions;
        num_of_collisions << collisions;
        text.setString ("Number of collisions: " + num_of_collisions.str());
        window.draw (text);
        window.draw (block_1);
        window.draw (block_2);
        block_1.move (sf::Vector2f(speed_1 * clock.getElapsedTime().asSeconds(), 0.f));
        block_2.move (sf::Vector2f(speed_2 * clock.restart().asSeconds(), 0.f));
        window.display();
      } while (fabs(block_1.getPosition().x - block_2.getPosition().x) <= (block_1.getSize().x + block_2.getSize().x) / 2.f);
    }
    // checking the collision condition of the first block with the wall
    if (block_1.getPosition().x - block_1.getSize().x / 2.f <= wall.getSize().x) {
      collisions++;  // increment variable 'collisions'
      speed_1 = -speed_1;  // recalculate the speed of the first block
      do {  // here we move both blocks, until the collision condition is no longer met
        window.clear();
        window.draw(ground);
        window.draw(sky);
        window.draw(wall);
        std::ostringstream num_of_collisions;
        num_of_collisions << collisions;
        text.setString ("Number of collisions: " + num_of_collisions.str());
        window.draw(text);
        window.draw(block_1);
        window.draw(block_2);
        block_1.move (sf::Vector2f(speed_1 * clock.getElapsedTime().asSeconds(), 0.f));
        block_2.move (sf::Vector2f(speed_2 * clock.restart().asSeconds(), 0.f));
        window.display();
      } while (block_1.getPosition().x - block_1.getSize().x / 2.f <= wall.getSize().x);
    }
  }
  std::cout << "Collisions: " << collisions << std::endl;
  std::cout << "Number PI in its " << (int) log10(mass_2 / mass_1) / 2 + 1 << "th approximation is equal to: ";
  std::cout << std::setprecision((int) log10(mass_2 / mass_1) / 2 + 1) << (float) collisions / sqrtf((float) (mass_2 / mass_1)) << std::endl << std::endl;
  return 0;
}

// this function draws Mandelbrot's set (a part of the file "Mandelbrot_set.cpp")
void Mandelbrot_set (sf::VertexArray& vertex_array, int precision, long double zoom, sf::RenderWindow& window) {
  for (int i = 0; i < (int) window.getSize().y; i++) {
    for (int j = 0; j < (int) window.getSize().x; j++) {
      complex_number c;
      // drawing Mandelbrot's set, we move point (0; 0) to the center of the window
      c.real = (long double) (j - (int) window.getSize().x / 2) / zoom;
      c.imaginary = (long double) (i - (int) window.getSize().y / 2) / zoom;
      complex_number z = c;
      int k;
      for (k = 1; k <= precision; k++) {  // the recurrent sequence: z_(n+1) = (z_n)^2 + c
        complex_number tmp;
        tmp.real = z.real * z.real - z.imaginary * z.imaginary + c.real;
        tmp.imaginary = 2 * z.real * z.imaginary + c.imaginary;
        z = tmp;
        if (z.real * z.real + z.imaginary * z.imaginary > 4)
          break;
      }
      // if after a certain number of iterations (called "precision") a sequence number does not exceed 2 (a «bail-out»), it belongs to the set and the corresponding point stays black 
      if (k <= precision) {  // otherwise, it becomes red, green or blue (depending on the iteration number, at which the criterion was fulfilled)
        vertex_array [i * window.getSize().x + j].position = sf::Vector2f((float) j, (float) i);
        if (k <= precision / 4)
          // shades of red (the closer to the boundary of the set, the brighter)
          vertex_array [i * window.getSize().x + j].color = sf::Color ((sf::Uint8) (k * 255 / (precision / 4)), 0, 0);
        else if (k <= precision / 2)
          // shades of green (likewise)
          vertex_array [i * window.getSize().x + j].color = sf::Color (0, (sf::Uint8) (k * 255 / (precision / 2)), 0);
        else if (k <= precision)
          // shades of blue (likewise)
          vertex_array [i * window.getSize().x + j].color = sf::Color (0, 0, (sf::Uint8) (k * 255 / precision));
      }
    }
  }
  return;
}

// this function uses a recursive formula that is necessary for PI's calculation (a part of the file "Mandelbrot_set.cpp")
void Mandelbrot_iterations(long double eps) {
  long double constanta = 0.25l + eps;
  long double z = constanta;
  int NumOfIterations;  // the number of iterations in a recurrent sequence z_(n+1) = (z_n)^2 + c, happened before its member exceeded 2
  sf::Clock clock;  // start the timer
  sf::Time elapsed;
  for (NumOfIterations = 1; z < 2.l; NumOfIterations++) {
    long double tmp = z;
    z = powl(tmp, 2.l) + constanta;
  }
  elapsed = clock.restart();  // stop the timer
  int NumOfExperiment = (int) (-log10l(eps) / 2.l);  // knowing epsilon, we can restore an experiment number
  std::cout << "Epsilon = " << std::setprecision(1 + 2 * NumOfExperiment) << eps << "; number of iterations: " << NumOfIterations << ";\n";
  std::cout << "Number PI in its " << NumOfExperiment << "th approximation: ";
  // experiment №NumOfExperiment enables to precisely gain first NumOfExperiment digits of PI
  std::cout << std::setprecision(NumOfExperiment) << (NumOfIterations / 10) / powl(10.l, NumOfExperiment - 1) << ";\n";
  std::cout << "Elapsed time: " << elapsed.asMicroseconds() / 1'000'000.l << " seconds.\n\n";
  return;
}

// this function is a main one in a file "Mandelbrot_set.cpp"
int Mandelbrot() {
  sf::RenderWindow window (sf::VideoMode(1920, 1080), "Mandelbrot's set");
  sf::VertexArray pointmap (sf::Points, window.getSize().x * window.getSize().y);
  
  int precision = 250;  // the maximum number of iterations for a given construction:
  // if after it "infinity criterion" has not been reached yet, we conclude that this point belongs to Mandelbrot's set; otherwise - it doesn't belong to the set
  long double zoom = 500.l;  // it is applied so as to illustrate this set on the screen in a full scale

  Mandelbrot_set (pointmap, precision, zoom, window);

  window.draw(pointmap);
  window.display();
  while (window.isOpen()) {
    sf::Event event;
    while (window.pollEvent(event)) {
      if (event.type == sf::Event::Closed)
        window.close();
    }
  }
  // organize seven experiments at different values of epsilon
  for (int j = 1; j < 8; j++) {
    long double eps = powl(100.l, -j);
    Mandelbrot_iterations(eps);
  }
  return 0;
}

// this function is called on each thread in a file "needles_multithreading.cpp"
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

// this function is a main one in a file "needles_multithreading.cpp"
int Buffon_needle() {
  float width;
  std::cout << "Enter the interval between neighbouring lines: ";
  std::cin >> width;
  assert(width > 0 && "The interval between neighbouring lines must be positive");

  int NumOfNeedles;
  std::cout << "Enter the number of needles you want to scatter: ";
  std::cin >> NumOfNeedles;
  assert(NumOfNeedles > 0 && "The number of needles must be positive");

  float length;
  std::cout << "Enter the length of each needle: ";
  std::cin >> length;
  assert(length > 0 && "The length of each needle must be positive");

  int NumOfThreads;
  std::cout << "Enter the preferable number of threads: ";
  std::cin >> NumOfThreads;
  assert(NumOfThreads > 0 && "The number of threads must be positive");

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
  needles.setPrimitiveType(sf::Lines);
  needles.resize(2*NumOfNeedles);
  int NumOfNeedlesPerThread = NumOfNeedles / NumOfThreads;  // the number of needles that each individual thread must scatter
  // create an array of threads' identificators
  pthread_t* threads = (pthread_t*) malloc (NumOfThreads * sizeof(pthread_t));
  if (threads == NULL) {
    std::cerr << "Failed to allocate memory for an array of threads' identificators via malloc()\n";
    return -1;
  }
  // create an array of structures
  Args* ArrayOfStructures = (Args*) malloc (NumOfThreads * sizeof(Args));
  if (ArrayOfStructures == NULL) {
    std::cerr << "Failed to allocate memory for an array of structures via malloc()\n";
    free(threads);
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
      free(threads);
      free(ArrayOfStructures);
      return -1;
    }
    // checking whether an error occurred within a function
    if (ArrayOfStructures[j].st_error == true) {
      free(threads);
      free(ArrayOfStructures);
      return -1;
    }
  }
  // waiting for all threads to finish
  for (int j = 0; j < NumOfThreads; j++) {
    if (pthread_join(threads[j], NULL) != 0) {
      std::cerr << "Failed to join a thread!\n";
      free(threads);
      free(ArrayOfStructures);
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
      free(threads);
      free(ArrayOfStructures);
      return -1;
    }
    if (RemainingNeedles.st_error == true) {
      free(threads);
      free(ArrayOfStructures);
      return -1;
    }
    if (pthread_join(remaining_needles, NULL) != 0) {
      std::cerr << "Failed to join a thread!\n";
      free(threads);
      free(ArrayOfStructures);
      return -1;
    }
  }
  std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();  // stop the timer
  // calculate number pi, based on the probability for each needle to cross any line and on current geometrical parameters
  long double pi = 2.f * length / width / (float) count * (float) NumOfNeedles;
  std::cout << "Number PI is: " << std::setprecision(15) << pi << ";\n";
  std::cout << "The calculation error is: " << fabsl(pi - PI_25_DIGITS) << ";\n";
  std::cout << "It took " << (std::chrono::duration_cast<std::chrono::nanoseconds>(end - begin).count()) / 1'000'000'000.l << " seconds to calculate it.\n\n";

  // the destroyment of the mutex
  if (pthread_mutex_destroy(&mutex) != 0) {
    std::cerr << "Failed to destroy a mutex!\n";
    free(threads);
    free(ArrayOfStructures);
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
  free(threads);
  free(ArrayOfStructures);
  return 0;
}

// this function autonomously implements Viete's formula (file "Viete.cpp")
int Viete() {
  uint64_t N;  // the number of factors that make up the product
  std::cout << "Enter the number of factors in Viete's formula: ";
  std::cin >> N;
  long double* factors = (long double*) malloc (N * sizeof(long double));  // a dynamic array of factors
  if (factors == NULL) {
    std::cerr << "Failed to allocate memory via malloc()\n";
    return -1;
  }
  std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();  // start the timer
  factors[0] = 2.l / sqrtl(2.l);  // initialize the zeroth factor
  long double pi = 2.l * factors[0];
  for (uint64_t i = 1; i < N; i++) {
    factors[i] = 2.l / sqrtl(2.l + 2.l / factors[i - 1]);  // nested radicals
    pi *= factors[i];
  }
  std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();  // stop the timer
  std::cout << "Number PI, obtained as a product of " << N << " factors, is equal to: " << std::setprecision(15) << pi << ";\n";
  std::cout << "The calculation error is: " << fabsl(pi - PI_25_DIGITS) << ";\n";
  std::cout << "The calculation took " << (std::chrono::duration_cast<std::chrono::nanoseconds>(end - begin).count()) / 1'000'000'000.l << " seconds.\n\n";
  free(factors);  // free allocated memory
  return 0;
}

// a function, used in the first integral expression (file "integral.cpp")
long double function_1 (long double x) {
  return 2.l * sqrtl(1.l - powl(x, 2.l));
}

// a function, used in the second integral expression (file "integral.cpp")
long double function_2 (long double x) {
  return 4.l / (1.l + powl(x, 2.l));
}

// a function, used in the third integral expression (file "integral.cpp")
long double function_3 (long double x) {
  return 6.l / sqrtl(1.l - powl(x, 2.l));
}

// this function is responsible for computational work only (the part of the file "integral.cpp")
long double Integral (uint8_t method, long double a, long double b, unsigned long N, long double (*func) (long double)) {
  const long double length = (b - a) / N;  // the length of each segment of integration
  long double integral = 0.l;  // an initial value of the integral
  switch (method) {
    case 0:  // left rectangle method
      for (long unsigned int step = 0; step < N; step++) {
        const long double x_left = a + step * length;
        integral += func(x_left);
      }
      integral *= length;
      break;
    case 1:  // right rectangle method
      for (long unsigned int step = 0; step < N; step++) {
        const long double x_right = a + (step + 1) * length;
        integral += func(x_right);
      }
      integral *= length;
      break;
    case 2:  // middle rectangle method
      for (long unsigned int step = 0; step < N; step++) {
        const long double x_middle = a + length * (static_cast<long double> (step) + 0.5l);
        integral += func(x_middle);
      }
      integral *= length;
      break;
    case 3:  // trapezoidal method
      for (long unsigned int step = 0; step < N; step++) {
        const long double x_1 = a + step * length;
        const long double x_2 = a + (step + 1) * length;
        integral += func(x_1) + func(x_2);
      }
      integral /= 2.l;
      integral *= length;
      break;
    case 4:  // parabola method (Simpson's method)
      for (long unsigned int step = 0; step < N; step++) {
        const long double x_left = a + step * length;
        const long double x_middle = a + (static_cast<long double> (step) + 0.5l) * length;
        const long double x_right = a + (step + 1) * length;
        integral += func(x_left) + 4.l * func(x_middle) + func(x_right);
      }
      integral /= 6.l;
      integral *= length;
      break;
    case 5:  // Romberg's method
    {
      std::vector<std::vector<long double>> Romberg_integral(20, std::vector<long double> (20));  // the implementation of a square lower-triangular matrix
      Romberg_integral.front().front() = Integral(3, a, b, 1, func);  // trapezoidal integral (method 3)
      long double Romberg_length = b - a;  // current length of the segment of integration
      for (unsigned long int step = 1; step < Romberg_integral.size(); step++) {
        Romberg_length /= 2.l;
        long double trapezoidal_integral = 0.l;
        long double stepEnd = powl(2.l, static_cast<long double> (step - 1));
        for (unsigned long int tzStep = 1; tzStep <= stepEnd; tzStep++) {
          const long double deltaX = (2.l * tzStep - 1.l) * Romberg_length;
          trapezoidal_integral += func(a + deltaX);
        }
        Romberg_integral[step].front() = 0.5l * Romberg_integral[step - 1].front() + trapezoidal_integral * Romberg_length;
        for (unsigned long int rbStep = 1; rbStep <= step; rbStep++) {
          const long double k = powl(4.l, rbStep);
          Romberg_integral[step][rbStep] = (k * Romberg_integral[step][rbStep - 1] - Romberg_integral[step - 1][rbStep - 1]) / (k - 1);
        }
      }
      return Romberg_integral[19][19];  // the result contains in the lower right corner of the lower-triangular matrix
    }
    case 6:  // one-dimensional Monte Carlo method
      for (long unsigned int step = 0; step < N; step++) {
        // choose an arbitrary abscissa on each interval of integration
        const long double x_random = a + (long double) step * length + length * static_cast <long double> (rand()) / static_cast <long double> (RAND_MAX);
        integral += func(x_random);
      }
      integral *= length;
      break;
    case 7:  // two-dimensional Monte Carlo method (estimation of areas)
    {
      unsigned long int count_local = 0;  // counter of the number of points, which are located in the area under the graph of the function
      Point* Points = (Point*) malloc (N * sizeof(Point));  // a dynamic array of instances of class Point
      if (Points == NULL) {
        std::cout << "Failed to allocate memory via malloc()\n";
        return -1;
      }
      for (unsigned long int i = 0; i < N; i++) {
        // a random abscissa, belonging to the segment [a; b]
        Points[i].set_X(a + (b - a) * static_cast <long double> (rand()) / static_cast <long double> (RAND_MAX));
        // a random ordinate, belonging to the segment [0; 10] (the upper value is taken with a margin)
        Points[i].set_Y(10.l * static_cast <long double> (rand()) / static_cast <long double> (RAND_MAX));
        if (Points[i].get_Y() <= func(Points[i].get_X()))  // if a point occurs under the function's graph, we increment count
          count_local++;
      }
      integral = (10.l * (b - a)) * (long double) count_local / (long double) N;  // integral is calculated, based on the probability for each point to occur under the function's graph
      free(Points);
      break;
    }
    default:
      std::cerr << "Undefined method\n";
      return -1;
  }
  return integral;
}

// this function informs a user about the results of integral's calculation (the part of the file "integral.cpp")
int usage_of_method (uint8_t method, long double a, long double b, unsigned long N, long double (*func) (long double)) {
  switch (method) {
    case 0:
      std::cout << "Wait for the results of usage of left rectangle method...\n";
      break;
    case 1:
      std::cout << "Wait for the results of usage of right rectangle method...\n";
      break;
    case 2:
      std::cout << "Wait for the results of usage of middle rectangle method...\n";
      break;
    case 3:
      std::cout << "Wait for the results of usage of trapezoidal method...\n";
      break;
    case 4:
      std::cout << "Wait for the results of usage of parabola method (Simpson's method)...\n";
      break;
    case 5:
      std::cout << "Wait for the results of usage of Romberg's method...\n";
      break;
    case 6:
      std::cout << "Wait for the results of usage of one-dimensional Monte Carlo method...\n";
      break;
    case 7:
      std::cout << "Wait for the results of usage of two-dimensional Monte Carlo method (estimation of areas)...\n";
      break;
    default:
      std::cerr << "Undefined method\n";
      return -1;
  }
  std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();  // start the timer
  // in the context of methods 0-4 and 6: N is the number of segments of integration; in the 7th method, it's the number of scattered points
  if (Integral(method, a, b, N, func) == -1)
    return -1;
  long double pi = Integral(method, a, b, N, func);
  std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();  // stop the timer
  std::cout << "Number PI is: " << std::setprecision(15) << pi;
  std::cout << ";\nThe calculation error is: " << fabsl(pi - PI_25_DIGITS);
  std::cout << ";\nIt took " << (std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count()) / 1000000.l;
  std::cout << " seconds to calculate PI.\n\n";
  return 0;
}

// this function runs calculation of integrals (a main function in the file "integral.cpp")
int integral_computation() {
  std::cout << "Enter the number of parts, into which you want to split the segment of integration: ";
  unsigned long NumOfSegments;
  std::cin >> NumOfSegments;
  assert(NumOfSegments >= 1 && "The number of segments of integration must be positive");

  std::vector<uint8_t> methods = {};  // all currently available methods
  for (uint8_t i = 0; i < 8; i++)
    methods.push_back(i);
  std::cout << "The first integral expression" << std::endl;
  for (uint8_t i = 0; i < methods.size(); i++) {
    if (usage_of_method(methods.at(i), -1.l, 1.l, NumOfSegments, function_1) == -1)
      return -1;
  }
  std::cout << "The second integral expression" << std::endl;
  for (uint8_t i = 0; i < methods.size(); i++) {
    if (usage_of_method(methods.at(i), 0.l, 1.l, NumOfSegments, function_2) == -1)
      return -1;
  }
  std::cout << "The third integral expression" << std::endl;
  for (uint8_t i = 0; i < methods.size(); i++) {
    if (usage_of_method(methods.at(i), 0.l, 0.5l, NumOfSegments, function_3) == -1)
      return -1;
  }
  return 0;
}

// this function autonomously implements a spigot algorithm (a main function in the file "spigot.cpp")
int spigot() {
  std::cout << "Enter the number of digits in number PI that you want to calculate (two or more): ";
  int N;
  std::cin >> N;
  assert(N >= 2 && "This number should be not less than two (not less than one digit after the decimal point)");
  int size = 10 * N / 3;  // the number of columns in a table (it depends on N)
  int* digits_of_pi = (int*) malloc (N * sizeof(int));  // allocation of memory for the array of digits of PI
  if (digits_of_pi == NULL) {
    std::cerr << "Failed to allocate memory via malloc() for an array <<digits_of_pi>>\n";
    return -1;
  }
  int** table = (int**) malloc ((4 * N) * sizeof(int*));  // allocation of memory for a two-dimensional array (table)
  if (table == NULL) {
    std::cerr << "Failed to allocate memory via malloc() for an array <<table>>\n";
    return -1;
  }
  for (int i = 0; i < 4 * N; i++) {
    table[i] = (int*) malloc (size * sizeof(int));
    if (table[i] == NULL) {
      std::cerr << "Failed to allocate memory via malloc() for the " << i << "th one-dimensional array: <<table[" << i << "]\n";
      return -1;
    }
  }
  std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();  // start the timer
  for (int i = 0; i < size; i++)  // initialization of the zeroth row by twenties
    table[0][i] = 20;
  for (int step = 0; step < N; step++) {
    // fill the last column of the table
    table[step * 4 + 1][size - 1] = 0;  // transfer
    table[step * 4 + 2][size - 1] = table[step * 4][size - 1] + table[step * 4 + 1][size - 1];  // sum
    table[step * 4 + 3][size - 1] = table[step * 4 + 2][size - 1] % (2 * (size - 1) + 1);  // remainder
    // fill the skeleton of the table
    for (int j = size - 2; j >= 1; j--) {
      table[step * 4 + 1][j] = (table[step * 4 + 2][j + 1] / (2 * (j + 1) + 1)) * (j + 1);  // transfer
      table[step * 4 + 2][j] = table[step * 4][j] + table[step * 4 + 1][j];  // sum
      table[step * 4 + 3][j] = table[step * 4 + 2][j] % (2 * j + 1);  // remainder
    }
    // fill the zeroth column of the table
    table[step * 4 + 1][0] = table[step * 4 + 2][1] / 3;
    table[step * 4 + 2][0] = table[step * 4 + 0][0] + table[step * 4 + 1][0];
    table[step * 4 + 3][0] = table[step * 4 + 2][0] % 10;
    // find a digit of PI
    digits_of_pi[step] = table[step * 4 + 2][0] / 10;
    // initialization for the following step
    if (step == N - 1)
      break;
    else {
      for (int i = 0; i < size; i++)
        table[(step + 1) * 4][i] = 10 * table[step * 4 + 3][i];
    }
  }
  // avoid overflow of individual digits
  for (int k = N - 1; k > 0; k--) {
    if (digits_of_pi[k] > 9) {
      digits_of_pi[k] -= 10;
      digits_of_pi[k - 1]++;
    }
  }
  // print the number PI
  std::cout << "Number PI is equal to: " << digits_of_pi[0] << ".";
  for (int i = 1; i < N; i++)
    std::cout << digits_of_pi[i];
  std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();  // stop the timer
  std::setprecision(10);
  std::cout << "; it took " << (std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count()) / 1000000.l;
  std::cout << " seconds to calculate PI.\n\n";
  // memory release
  free(digits_of_pi);
  for (int i = 0; i < size; i++)
    free(table[i]);
  free(table);
  return 0;
}

// this function forms a computing framework of the file "series.cpp"
int calculation_of_pi_via_series (int way, int N) {
  if (!((way == 0) || (way == 1) || (way == 2) || (way == 3) || (way == 4))) {
    std::cerr << "Undefined method\n";
    return -1;
  }
  long double pi = 0.l;  // initial value for addition
  long double pi_Wallis = 1.l;  // initial value for multiplication
  long double start = omp_get_wtime();  // the beginning of the algorithm
  #pragma omp parallel reduction(+:pi)
  {
    #pragma omp parallel for
    for (int i = 0; i < N; i++) {
      switch (way) {   // there're several series, approximating number pi
        case 0:  // Gregory-Leibniz series
          pi += powl(-1.l, i) / (2.l * i + 1.l);
          break;
        case 1:  // Madhava series
          pi += powl(-1.l, i) / (2.l * i + 1.l) / powl(3.l, i);
          break;
        case 2:  // Nilakantha series
          pi += powl(-1.l, i) / (2.l * i + 2.l) / (2.l * i + 3.l) / (2.l * i + 4.l);
          break;
        case 3:  // Euler series
          pi += 1.l / powl(i + 1.l, 2.l);
          break;
        case 4:  // Wallis' formula
          pi_Wallis *= powl((2.l * i + 2.l), 2.l) / (2.l * i + 1.l) / (2.l * i + 3.l);
          break;
        default:
          break;
      }
    }
  }
  std::cout << "The value of number PI for the first ";
  std::cout << std::setprecision(15);
  switch (way) {
    case 0:
      std::cout << N << " terms of Gregory-Leibniz series: " << 4.l * pi << ";\n";
      std::cout << "The calculation error is: " << fabsl(4.l * pi - PI_25_DIGITS) << ";\n";
      break;
    case 1:
      std::cout << N << " terms of Madhava series: " << sqrtl(12.l) * pi << ";\n";
      std::cout << "The calculation error is: " << fabsl(sqrtl(12.l) * pi - PI_25_DIGITS) << ";\n";
      break;
    case 2:
      std::cout << N + 1 << " terms of Nilakantha series: " << 4.l * pi + 3.l << ";\n";
      std::cout << "The calculation error is: " << fabsl((4.l * pi + 3.l) - PI_25_DIGITS) << ";\n";
      break;
    case 3:
      std::cout << N << " terms of Euler series: " << sqrtl(6.l * pi) << ";\n";
      std::cout << "The calculation error is: " << fabsl(sqrtl(6.l * pi) - PI_25_DIGITS) << ";\n";
      break;
    case 4:
      std::cout << 2*N + 1 << " factors in Wallis' formula: " << 2.l * pi_Wallis << ";\n";
      std::cout << "The calculation error is: " << fabsl(2.l * pi_Wallis - PI_25_DIGITS) << ";\n";
      break;
    default:
      break;
  }
  long double end = omp_get_wtime();  // the end of the algorithm
  std::cout << "It took " << std::setprecision(6) << end - start << " seconds to calculate PI.\n";
  return 0;
}

// this function runs all experiments on PI's calculation via series (a main function in the file "series.cpp")
int series() {
  std::vector<int> ways = {};  // vector of ways of calculation of pi
  for (int i = 0; i < 5; i++)
    ways.push_back(i);
  for (int j = 0; j < (int) ways.size(); j++) {
    for (int i = 10; i < 100'000'000; i *= 10) {
      if (calculation_of_pi_via_series(ways.at(j), i) == -1)
        return -1;
    }
    std::cout << '\n';
  }
  return 0;
}

// an auxiliary function that displays a notification (about the necessity to look at the console) on the screen
void notification(sf::RenderWindow& window) {
  sf::RenderWindow window_notification(sf::VideoMode(1920, 1080), "Notification");
  window_notification.clear(sf::Color::White);

  sf::Font font;
  font.loadFromFile ("fonts/Times_New_Roman.ttf");

  sf::Text text;
  text.setFont (font);
  text.setCharacterSize (120);
  text.setFillColor (sf::Color::Red);
  text.setString ("Look at the console, please");
  sf::FloatRect textRect = text.getLocalBounds();
  text.setOrigin(textRect.left + textRect.width / 2.f, textRect.top + textRect.height / 2.f);
  text.setPosition(sf::Vector2f((float) window.getSize().x / 2.f, (float) window.getSize().y / 2.f));
  window_notification.draw(text);
  window_notification.display();
  while (window_notification.isOpen()) {
    sf::Event event;
    while (window_notification.pollEvent(event)) {
      if (event.type == sf::Event::Closed)
        window_notification.close();
    }
  }
  return;
}

// an auxiliary function that displays reference information on the screen
void reference_information(const std::string &filename, sf::Vector2f size) {
  sf::RenderWindow window_info(sf::VideoMode((unsigned int) size.x, (unsigned int) size.y), "Reference information");
  
  sf::RectangleShape info;
  info.setSize(size);
  sf::Texture info_texture;
  info_texture.loadFromFile(filename);
  info.setTexture(&info_texture);
  
  window_info.draw(info);
  window_info.display();
  // so as to continue using a program, a user should close a window with reference information first
  while (window_info.isOpen()) {
    sf::Event event;
    while (window_info.pollEvent(event)) {
      if (event.type == sf::Event::Closed)
        window_info.close();
    }
  }
  return;
}