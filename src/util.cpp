#include "util.h"
#include <cstdlib>
#include <iostream>
#include <string>
std::vector<std::string> split(std::string s, char delim) {
  std::vector<std::string> result;
  std::string current = "";
  for (char c : s) {
    if (c == delim) {
      if (current != "")
        result.push_back(current);
      current = "";
      continue;
    }
    current += c;
  }
  if (current != "")
    result.push_back(current);
  return result;
}

void error(std::string msg) {
  std::cerr << msg << std::endl;
  exit(EXIT_FAILURE);
}


