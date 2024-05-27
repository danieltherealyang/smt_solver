#include "SMTSolver.h"
#include "SATSolver.h"
#include "TheorySolver.h"
#include "Theory.h"
#include "SAT.h"
#include "DL.h"
#include "util.h"
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <iterator>
#include <ostream>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

OP get_op(std::string_view s) {
  if (s == "<")
    return LT;
  if (s == "<=")
    return LE;
  if (s == ">")
    return GT;
  if (s == ">=")
    return GE;
  if (s == "=")
    return EQ;
  if (s == "!=")
    return NEQ;
  std::string msg = "Incorrect operator: " + std::string(s) + ", must be <, <=, >, >=, =, !=";
  error(msg);
  return EQ;
}

OP neg_op(OP op) {
  switch(op) {
    case LT:
      return GE;
    case LE:
      return GT;
    case GT:
      return LE;
    case GE:
      return GT;
    case EQ:
      return NEQ;
    case NEQ:
      return EQ;
  }
  error("neg_op should never reach here");
  return LT;
}

Constraint parse_constraint(const std::vector<std::string>& tokens, int& current) {
  bool negated = tokens[current] == "!";
  current += negated;
  int skip = 5;
  if (current > tokens.size() - skip) {
    error("Incorrect Input Format");
  }
  OP op = get_op(tokens[current+3]);
  Constraint ret_val(
    Term(
      Variable(tokens[current]),
      Variable(tokens[current+2])
      ),
    (negated) ? neg_op(op) : op,
    stoi(tokens[current+4])
    );
  current += skip;
  return ret_val;
}

void SMTSolver::parse_clause(std::string clause) {
  if (clause == "")
    return;
  t_clauses.push_back(std::vector<Constraint>());
  std::vector<std::string> tokens = split(clause, ' ');
  int current = 0;
  while (current < tokens.size()) {
    if (tokens[current] == "||")
      current++;
    t_clauses.rbegin()->push_back(parse_constraint(tokens, current));
  }
}

std::vector<SATClause<int>> SMTSolver::map_clauses() {
  std::vector<SATClause<int>> result;
  for (const std::vector<Constraint>& clause : t_clauses) {
    result.push_back(SATClause<int>());
    for (const Constraint& constraint : clause) {
      if (c_to_id.find(constraint) == c_to_id.end()) {
        c_to_id[constraint] = c_to_id.size();
        id_to_c[id_to_c.size()] = constraint;
      }
      result.rbegin()->add_literal(SATLiteral<int>(c_to_id[constraint]), POS);
    }
  }
  return result;
}

void SMTSolver::preprocess_equality_recurse(const std::vector<Constraint> original, std::vector<Constraint>& st, int index) {
  //index not processed yet
  //when reach end, add to t_clauses
  if (index == original.size()) {
    t_clauses.push_back(st);
    return;
  }
  if (original[index].op != EQ) {
    st.push_back(original[index]);
    preprocess_equality_recurse(original, st, index+1);
    st.pop_back();
    return;
  }
  //Handle Equality
  const Constraint& current = original[index];
  //x-y <= c && x-y >= c
  st.push_back({current.t1, LE, current.c2});
  preprocess_equality_recurse(original, st, index+1);
  st.pop_back();

  st.push_back({current.t1, GE, current.c2});
  preprocess_equality_recurse(original, st, index+1);
  st.pop_back();
}

bool has_op_constraint(const std::vector<Constraint>& clause, OP op) {
  for (const Constraint& i : clause) {
    if (i.op == op)
      return true;
  }
  return false;
}

void SMTSolver::preprocess_equality() {
  int n = t_clauses.size();
  std::set<int> indices_to_remove;
  for (int i = 0; i < n; i++) {
    if (!has_op_constraint(t_clauses[i], EQ))
      continue;
    indices_to_remove.insert(i);
    std::vector<Constraint> st;
    preprocess_equality_recurse(t_clauses[i], st, 0);
  }
  remove_indices(t_clauses, indices_to_remove);
}

void SMTSolver::preprocess_disequality_recurse(const std::vector<Constraint> original, std::vector<Constraint>& st, int index) {
  //index not processed yet
  //when reach end, add to t_clauses
  if (index == original.size()) {
    t_clauses.push_back(st);
    return;
  }
  if (original[index].op != NEQ) {
    st.push_back(original[index]);
    preprocess_disequality_recurse(original, st, index+1);
    st.pop_back();
    return;
  }
  //Handle Equality
  const Constraint& current = original[index];
  //(x-y < c || x-y > c) && (x-y <= c || x-y >= c)
  st.push_back({current.t1, LT, current.c2});
  st.push_back({current.t1, GT, current.c2});
  preprocess_disequality_recurse(original, st, index+1);
  st.pop_back();
  st.pop_back();

  if (current.c2 != 0) {
    st.push_back({current.t1, LE, current.c2});
    st.push_back({current.t1, GE, current.c2});
    preprocess_disequality_recurse(original, st, index+1);
    st.pop_back();
    st.pop_back();
  }
}

void SMTSolver::preprocess_disequality() {
  int n = t_clauses.size();
  std::set<int> indices_to_remove;
  for (int i = 0; i < n; i++) {
    if (!has_op_constraint(t_clauses[i], NEQ))
      continue;
    indices_to_remove.insert(i);
    std::vector<Constraint> st;
    preprocess_disequality_recurse(t_clauses[i], st, 0);
  }
  remove_indices(t_clauses, indices_to_remove);
}

void SMTSolver::preprocess_greaterthan() {
  for (int i = 0; i < t_clauses.size(); i++) {
    for (int j = 0; j < t_clauses[i].size(); j++) {
      if (t_clauses[i][j].op != GT)
        continue;
      t_clauses[i][j].op = GE;
      t_clauses[i][j].c2++;
    }
  }
}

void SMTSolver::preprocess_lessthan() {  
  for (int i = 0; i < t_clauses.size(); i++) {
    for (int j = 0; j < t_clauses[i].size(); j++) {
      if (t_clauses[i][j].op != LT)
        continue;
      t_clauses[i][j].op = LE;
      t_clauses[i][j].c2--;
    }
  }
}

void SMTSolver::preprocess_clauses() {
  //all constraints should be >= or <=
  preprocess_equality();
  preprocess_disequality();
  preprocess_greaterthan();
  preprocess_lessthan();
}

SMTSolver::SMTSolver(std::string filename) {
// Open the file
  std::ifstream file(filename);
  if (!file) {

    error("Error: Could not open file "  + filename);
  }

  // Read and print the file content
  std::string line;
  while (std::getline(file, line)) {
    parse_clause(line);
  }

  // Close the file
  file.close();

  preprocess_clauses();
}

std::vector<Constraint> SMTSolver::extract_constraints(SATModel<int> model) {
  std::vector<Constraint> result;
  for (const std::pair<SATLiteral<int>, bool>& p : model) {
    Constraint current = id_to_c[p.first.id];
    current.op = (p.second) ? current.op : neg_op(current.op);
    if (current.op == GT) {
      current.t1 = {current.t1.v2, current.t1.v1};
      current.op = LT;
      current.c2 = -current.c2;
    }
    if (current.op == LT) {
      current.op = LE;
      current.c2--;
    }
    result.push_back(current);
  }
  return result;
}

void SMTSolver::print_constraints() {
  std::cout << "Constraints:" << std::endl;
  for (const std::vector<Constraint>& clause : t_clauses) {
    for (auto it = clause.begin(); it != clause.end(); it++) {
      it->print();
      if (std::next(it) != clause.end())
        std::cout << " || ";
    }
    std::cout << std::endl;
  }
}

void SMTSolver::solve() {
  //create mapping from current clauses to ints
  //convert mapping to clauses
  std::vector<SATClause<int>> sat_clauses = map_clauses();
  print_constraints();
  for (const auto& p : id_to_c) {
    p.second.print();
    std::cout << "; " << p.first << std::endl;
  }
  SATSolver sat_solver;
  TheorySolver theory_solver;
  std::optional<SATModel<int>> sat_model = sat_solver.solve(sat_clauses);
  std::cout << "SATMODEL: " << std::endl;
  if (sat_model.has_value())
    for (const auto& p : sat_model.value()) {
      id_to_c[p.first.id].print();
      std::cout << ((p.second) ? "TRUE" : "FALSE") << std::endl;
    }
  while (sat_model.has_value()) {
    std::optional<TheoryAssignment<int>> concretization = theory_solver.solve(extract_constraints(sat_model.value()));
    if (concretization.has_value()) {
      concretization.value().print();
      return;
    }
    sat_clauses.push_back(sat_model.value().conflict_clause());
    
    //remove
    // std::cout << "SAT_Clauses" << std::endl;
    // for (const SATClause<int>& c : sat_clauses)
    //   c.print();
    // std::cout << "SAT solving" << std::endl;
    //endremove

    sat_model = sat_solver.solve(sat_clauses);
    
    //remove
    std::cout << "Second round" << std::endl;
    if (sat_model.has_value())
      for (const auto& p : sat_model.value()) {
        id_to_c[p.first.id].print();
        std::cout << ((p.second) ? "TRUE" : "FALSE") << std::endl;
      }
    // break;
    //endremove
  }
  std::cout << "Unsatisfiable" << std::endl;
}
