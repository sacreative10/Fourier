#include "DFT.h"
#include <SFML/Graphics.hpp>
#include <cmath>
#include <complex>
#include <fstream>
#include <iostream>
#include <math.h>
#include <random>
#include <stdio.h>
#include <string.h>
#include <utility>
#include <vector>
// forbid formatting of these two lines
// clang-format off
#define NANOSVG_IMPLEMENTATION
#include "nanosvg/nanosvg.h"
// clang-format on

std::vector<std::vector<double>> inputFromAFile(std::string fileInp) {
  std::vector<std::vector<double>> input;
  std::ifstream file(fileInp);
  std::string line;
  while (std::getline(file, line)) {
    std::istringstream iss(line);
    std::vector<double> point;
    std::string x, y;
    std::getline(iss, x, ',');
    std::getline(iss, y, ',');
    // we will have to convert these from top left to center
    point.push_back(std::stod(x) - 400);
    point.push_back(std::stod(y) - 300);
    input.push_back(point);
  }
  return input;
}

// read from svg file
//
std::vector<std::vector<double>> inputFromSVG(std::string fileInp) {
  // Read SVG
  NSVGimage *image;
  image = nsvgParseFromFile(fileInp.c_str(), "px", 96);

  // Convert to vector of vectors of doubles
  std::vector<std::vector<double>> input;
  NSVGshape *shape = image->shapes;
  // get width and height of the svg
  double width = image->width;
  double height = image->height;
  for (; shape != NULL; shape = shape->next) {
    for (NSVGpath *path = shape->paths; path != NULL; path = path->next) {
      for (int i = 0; i < path->npts - 1; i++) {
        std::vector<double> point;
        point.push_back(path->pts[i * 2] - width / 2);
        point.push_back(path->pts[i * 2 + 1] - height / 2);
        input.push_back(point);
      }
    }
  }

  // Delete SVG
  nsvgDelete(image);

  return input;
}

sf::Vector2f drawEpicycles(sf::RenderWindow &window, sf::Vector2f vertex,
                           double rotation, DFTResponse response, float dt,
                           std::vector<int> indicies) {
  // simply put, we draw the epicycles here, and return the tail end of the last
  // epicycle, now with indicies

  for (int i = 0; i < indicies.size(); i++) {
    double prevX = vertex.x;
    double prevY = vertex.y;
    double freq = response.frequencies[indicies[i]];
    double radius = response.amplitudes[indicies[i]];
    double phase = response.phases[indicies[i]];
    vertex.x += radius * std::cos(freq * dt + phase + rotation);
    vertex.y += radius * std::sin(freq * dt + phase + rotation);

    sf::CircleShape circle(radius);
    circle.setOrigin(radius, radius);
    circle.setPosition(prevX, prevY);
    circle.setFillColor(sf::Color::Transparent);
    circle.setOutlineColor(sf::Color::White);
    circle.setOutlineThickness(1);
    window.draw(circle);
    // draw a line from the center of the previous circle to the center of the
    // current circle
    sf::Vertex line[] = {sf::Vertex(sf::Vector2f(prevX, prevY)),
                         sf::Vertex(sf::Vector2f(vertex.x, vertex.y))};
    window.draw(line, 2, sf::Lines);
  }

  return vertex;
}

std::vector<int> sortByAmplitude(DFTResponse &dft) {
  std::vector<int> indices;
  for (int i = 0; i < dft.amplitudes.size(); i++) {
    indices.push_back(i);
  }
  for (int i = 0; i < dft.amplitudes.size(); i++) {
    for (int j = i + 1; j < dft.amplitudes.size(); j++) {
      if (dft.amplitudes[indices[i]] < dft.amplitudes[indices[j]]) {
        std::swap(indices[i], indices[j]);
      }
    }
  }
  return indices;
}

int main() {
  auto window = sf::RenderWindow{{800, 600}, "CMake SFML Project"};
  // make the window centered at tge middle of the screen
  window.setFramerateLimit(60);
  window.setView(sf::View(sf::FloatRect(0, 0, 800, 600)));

  // std::vector<std::vector<double>> input =
  //     inputFromSVG("../res/pi-symbol-icon.svg");

  // for (int i = 0; i < input.size(); i++) {
  //   std::cout << input[i][0] << " " << input[i][1] << std::endl;
  //   input[i][0] /= 5;
  //   input[i][1] /= 5;
  // }

  std::vector<std::vector<double>> input;
  for (int i = 0; i < 100; i++) {
    std::vector<double> point;
    point.push_back(sin(i) * 100);
    point.push_back(0);
    input.push_back(point);
  }

  auto dft = DFT(input);
  std::vector<int> sorted = sortByAmplitude(dft);

  std::vector<sf::Vector2f> path;
  float time = 0;
  while (window.isOpen()) {
    for (auto event = sf::Event{}; window.pollEvent(event);) {
      if (event.type == sf::Event::Closed) {
        window.close();
      }
    }

    window.clear();
    sf::Vector2f vertex =
        drawEpicycles(window, {800 / 2, 600 / 2}, 0, dft, time, sorted);
    path.push_back(vertex);

    sf::Vertex line[path.size()];
    for (int i = 0; i < path.size(); i++) {
      line[i] = sf::Vertex(path[i]);
      line[i].color = sf::Color(255, 255, 255, 255 * i / path.size());
      path[i] = path[i] * 0.999f;
      if (path[i].x < 0 && path[i].y < 0) {
        path.erase(path.begin(), path.begin() + i);
      }
    }
    window.draw(line, path.size(), sf::Lines);
    window.display();

    const float dt = M_2_PI / input.size() * (1 / 60.0f) * 2;
    time += dt;

    if (time > M_2_PI) {
      // path.clear();
      time = 0;
    }
  }
}
