#pragma once

#include <string>
#include <unordered_map>
template<typename T>
class TheoryAssignment {
  std::unordered_map<std::string, T> assignments;
public:
  void assign(std::string name, T value);
  void print();
};

#include "Theory.tcc"
