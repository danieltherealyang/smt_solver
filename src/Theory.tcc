#pragma once
#include "Theory.h"
#include <iostream>
#include <utility>

template<typename T>
void TheoryAssignment<T>::assign(std::string name, T value) {
  assignments[name] = value;
};

template<typename T>
void TheoryAssignment<T>::print() {
  std::cout << "Theory Values: " << std::endl;
  for (const std::pair<std::string, T>& p : assignments) {
    std::cout << p.first << " = " << p.second << std::endl;
  }
};
