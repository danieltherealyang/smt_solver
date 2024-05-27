#pragma once
#include "DL.h"
#include <set>
#include <string>
#include <vector>
std::vector<std::string> split(std::string s, char delim);
void error(std::string msg);
std::string op_to_string(OP op);

template<typename T>
void remove_indices(std::vector<T>& vec, const std::set<int>& indicesToRemove) {
  for (auto it = indicesToRemove.rbegin(); it != indicesToRemove.rend(); ++it) {
    vec.erase(vec.begin() + *it);
  }
}
