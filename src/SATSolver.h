#pragma once
#include "SAT.h"
#include <optional>
#include <unordered_map>
#include <vector>

using clause_vec = std::vector<std::unordered_map<SATLiteral<int>, SATPolarity>>;

class SATSolver {
private:
  std::unordered_map<SATLiteral<int>, bool> m_assignments;
  //m_assignments should be full after this
  bool DPLL(clause_vec phi); //true if SAT
  void unit_propagate(clause_vec& phi);
  void pure_literal_assign(clause_vec& phi);
  SATLiteral<int> choose_literal(const clause_vec& phi);
  clause_vec phi(const std::vector<SATClause<int>>& clauses);
public:
  void reset();
  std::optional<SATModel<int>> solve(const std::vector<SATClause<int>>& clauses);
};
