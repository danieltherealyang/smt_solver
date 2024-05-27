#pragma once
#include "DL.h"
#include "SAT.h"
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>
class SMTSolver {
private:
  std::vector<std::vector<Constraint>> t_clauses;
  std::unordered_map<Constraint, int> c_to_id;
  std::unordered_map<int, Constraint> id_to_c;
  void parse_clause(std::string s);
  void preprocess_equality_recurse(const std::vector<Constraint> original, std::vector<Constraint>& st, int index);
  void preprocess_disequality_recurse(const std::vector<Constraint> original, std::vector<Constraint>& st, int index);
  void preprocess_equality();
  void preprocess_disequality();
  void preprocess_greaterthan();
  void preprocess_lessthan();
  void preprocess_clauses();
  std::vector<SATClause<int>> map_clauses();
public:
  SMTSolver(std::string filename);
  std::vector<Constraint> extract_constraints(SATModel<int> model);
  void print_constraints();
  void solve();
};

