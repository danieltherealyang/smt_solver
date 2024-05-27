#pragma once
#include "Theory.h"
#include "DL.h"
#include <optional>
#include <string>
class TheorySolver {
private:
  std::unordered_map<std::string, int> name_to_id;
  std::unordered_map<int, std::string> id_to_name;
  int num_vars = 0;

  //{dest, weight}
  std::vector<std::vector<std::pair<int,int>>> out_edges; //forward edges
  std::vector<std::vector<std::pair<int,int>>> in_edges; //backward edges
  bool neg_cycle_found() const; //relax edges n times or when no changes made
  std::vector<int> relax_edges(const std::vector<int>& path_lengths) const;
  void initialize(const std::vector<Constraint>& constraints);
  TheoryAssignment<int> get_assignment();
public:
  //nullopt means Unsatisfiable
  void reset();
  std::optional<TheoryAssignment<int>> solve(std::vector<Constraint> constraints);
};
