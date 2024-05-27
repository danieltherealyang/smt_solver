#pragma once
#include <functional>
#include <iostream>
#include <string>

enum OP {
  LT, //<
  LE, //<=
  GT, //>
  GE, //>=
  EQ, //=
  NEQ //!=
};

std::string op_to_string(OP op);

struct Variable {
  std::string name;
  bool operator==(const Variable& other) const;
};

struct Term { //v1 - v2
  Variable v1;
  Variable v2;
  bool operator==(const Term& other) const;
};

struct Constraint { //term <= c2
  Term t1;
  OP op;
  int c2;
  bool operator==(const Constraint& other) const;
  void print() const {
    std::cout << t1.v1.name + " - " + t1.v2.name + " " << op_to_string(op) + " " << c2;
  };
};

namespace std {
template <>
struct hash<Variable> {
  std::size_t operator()(const Variable& v) {
    return hash<std::string>()(v.name);
  }
};
template <>
struct hash<Term> {
  std::size_t operator()(const Term& t) const {
    std::size_t h1 = std::hash<Variable>()(t.v1);
    std::size_t h2 = std::hash<Variable>()(t.v2);
    return h1 ^ (h1 << 1);
  }
};

template <>
struct hash<Constraint> {
  std::size_t operator()(const Constraint& s) const {
    // Combine the hash values of individual members
    std::size_t h1 = std::hash<Term>()(s.t1);
    std::size_t h2 = std::hash<int>()(s.op);
    std::size_t h3 = std::hash<int>()(s.c2);
    return h1 ^ (h2 << 1) ^ (h3 << 2); // or use boost::hash_combine
  }
};
}

