#include "SATSolver.h"
#include "SAT.h"
#include "util.h"
#include <optional>
#include <iostream>
#include <set>
#include <unordered_map>
#include <unordered_set>

void SATSolver::reset() {
  m_assignments.clear();
}

clause_vec SATSolver::phi(const std::vector<SATClause<int>>& clauses) {
  clause_vec result;
  for (const SATClause<int>& p : clauses) {
    result.push_back(p.get_map());
  }
  return result;
}

bool isempty(const clause_vec& phi) {
  return phi.empty();
}

bool hasempty(const clause_vec& phi) {
  for (auto i : phi) {
    if (i.empty())
      return true;
  }
  return false;
}

void mutatetrue(clause_vec& phi, SATLiteral<int> l) {
  std::set<int> indices;
  do {
    indices.clear();
    for (int i = 0; i < phi.size(); i++) {
      if (phi[i].find(l) != phi[i].end() && phi[i][l] == POS)
        indices.insert(i);
      else
        phi[i].erase(l);
    }
    remove_indices(phi, indices);
  } while (!indices.empty());
}

clause_vec propagatetrue(clause_vec phi, SATLiteral<int> l) {
  mutatetrue(phi, l);
  return phi;
}

void mutatefalse(clause_vec& phi, SATLiteral<int> l) {
  std::set<int> indices;
  do {
    indices.clear();
    for (int i = 0; i < phi.size(); i++) {
      if (phi[i].find(l) != phi[i].end() && phi[i][l] == NEG)
        indices.insert(i);
      else
        phi[i].erase(l);
    }
    remove_indices(phi, indices);
  } while (!indices.empty());
}

clause_vec propagatefalse(clause_vec phi, SATLiteral<int> l) {
  mutatefalse(phi, l);
  return phi;
}

std::optional<std::pair<SATLiteral<int>, SATPolarity>> find_unit_clause(const clause_vec& phi) {
  for (const std::unordered_map<SATLiteral<int>, SATPolarity>& clause : phi) {
    if (clause.size() == 1)
      return *clause.begin();
  }
  return std::nullopt;
}

void SATSolver::unit_propagate(clause_vec& phi) {
  std::optional<std::pair<SATLiteral<int>, SATPolarity>> unit_literal = find_unit_clause(phi);
  while (unit_literal.has_value()) {
    if (unit_literal.value().second == POS) {
      m_assignments[unit_literal.value().first] = true;
      mutatetrue(phi, unit_literal.value().first);
    }
    else {
      m_assignments[unit_literal.value().first] = false;
      mutatefalse(phi, unit_literal.value().first);
    }
    unit_literal = find_unit_clause(phi);
  }
}

void SATSolver::pure_literal_assign(clause_vec& phi) {
  std::unordered_map<SATLiteral<int>, SATPolarity> pures;
  std::unordered_set<SATLiteral<int>> impure;
  //find pures
  for (const std::unordered_map<SATLiteral<int>, SATPolarity>& clause : phi) {
    for (const std::pair<SATLiteral<int>, SATPolarity>& p : clause) {
      if (impure.contains(p.first))
        continue;
      if (pures.find(p.first) == pures.end()) {
        pures[p.first] = p.second;
        continue;
      }
      if (pures[p.first] != p.second) {
        pures.erase(p.first);
        impure.insert(p.first);
      }
    }
  }
  //assign pures
  for (const std::pair<SATLiteral<int>, SATPolarity>& p : pures) {
    m_assignments[p.first] = p.second == POS;
  }
  //remove clauses with pures
  std::set<int> clauses_to_remove;
  for (int i = 0; i < phi.size(); i++) {
    for (const std::pair<SATLiteral<int>, SATPolarity>& p : pures) {
      if (phi[i].find(p.first) != phi[i].end()) {
        clauses_to_remove.insert(i);
        break;
      }
    }
  }
  remove_indices(phi, clauses_to_remove);
}

SATLiteral<int> SATSolver::choose_literal(const clause_vec& phi) {
  if (phi.empty())
    error("phi not supposed to be empty when choosing literal");
  return phi[0].begin()->first;
}

bool SATSolver::DPLL(clause_vec phi) {
  unit_propagate(phi);
  pure_literal_assign(phi);
  
  if (isempty(phi))
    return true;
  if (hasempty(phi))
    return false;
  SATLiteral<int> l = choose_literal(phi);
  m_assignments[l] = true;
  if (DPLL(propagatetrue(phi, l)))
    return true;
  m_assignments[l] = false;
  return DPLL(propagatefalse(phi, l));
}

std::optional<SATModel<int>> SATSolver::solve(const std::vector<SATClause<int>>& clauses) {
  reset();
  bool satisfiable = DPLL(phi(clauses)); //m_assignments filled;
  if (!satisfiable)
    return std::nullopt;
  SATModel<int> result;
  for (const std::pair<SATLiteral<int>,bool>& p : m_assignments) {
    result.assign(p.first, p.second);
  }
  // for (const auto i : result)
    // std::cout << "ID: " << i.first.id << " " << ((i.second) ? "TRUE" : "FALSE") << std::endl;
  return result;
}
