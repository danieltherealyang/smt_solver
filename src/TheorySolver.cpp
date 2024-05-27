#include "TheorySolver.h"
#include "Theory.h"
#include "util.h"
#include <climits>
#include <optional>
#include <string>
#include <string_view>
#include <sys/types.h>
#include <unordered_set>
#include <vector>

bool TheorySolver::neg_cycle_found() const {
  std::vector<int> path_lengths(num_vars, 0);
  std::cout << "Find negative cycles: " << std::endl;
  for (int i : path_lengths)
    std::cout << i << ',';
  std::cout << std::endl;
  for (int i = 0; i < num_vars-1; i++) {
    path_lengths = relax_edges(path_lengths);
    for (int i : path_lengths)
      std::cout << i << ',';
    std::cout << std::endl;
  }
  std::vector<int> final_paths = relax_edges(path_lengths);
  for (int i : final_paths)
    std::cout << i << ',';
  std::cout << std::endl;
  std::cout << ((path_lengths != final_paths) ? "Neg cycle found" : "No neg cycle") << std::endl;
  return path_lengths != final_paths;
}


std::vector<int> TheorySolver::relax_edges(const std::vector<int>& path_lengths) const {
  //assumes that path_lengths don't overflow
  std::vector<int> result = path_lengths;
  for (int i = 0; i < num_vars; i++) {
    for (const std::pair<int,int>& p : in_edges[i]) {
      result[i] = std::min(result[i], path_lengths[p.first] + p.second);
    }
  }
  return result;
}

void TheorySolver::initialize(const std::vector<Constraint>& constraints) {
  //maps varnames to ints
  //creates graph
  std::unordered_set<std::string> vars;
  for (const Constraint& c : constraints) {
    vars.insert(c.t1.v1.name);
    vars.insert(c.t1.v2.name);
  }
  num_vars = vars.size();
  for (const std::string& s : vars) {
    int id = name_to_id.size();
    name_to_id[s] = id;
    id_to_name[id] = s;
  }
  out_edges = std::vector<std::vector<std::pair<int,int>>>(num_vars);
  in_edges = std::vector<std::vector<std::pair<int,int>>>(num_vars);
  for (const Constraint& c : constraints) {
    if (c.op != GE && c.op != LE)
      error("Constraints passed to TheorySolver must be either <= or >= formulas");
    int src = name_to_id[c.t1.v1.name];
    int dest = name_to_id[c.t1.v2.name];
    int weight = c.c2;
    if (c.op == GE) {
      std::swap(src, dest);
      weight = -weight;
    }
    out_edges[src].push_back({dest, weight});
    in_edges[dest].push_back({src, weight});
  }
  std::cout << "OUT EDGES" << std::endl;
  for (int i = 0; i < num_vars; i++) {
    std::cout << id_to_name[i] << ": ";
    for (const auto& p : out_edges[i])
      std::cout << p.first << ',' << p.second << " ";
    std::cout << std::endl;
  }
  std::cout << "IN EDGES" << std::endl;
  for (int i = 0; i < num_vars; i++) {
    std::cout << id_to_name[i] << ": ";
    for (const auto& p : in_edges[i])
      std::cout << p.first << ',' << p.second << " ";
    std::cout << std::endl;
  }
}

void random_assign_if_no_root(std::vector<int>& assignments) {
  for (int i = 0; i < assignments.size(); i++) {
    if (assignments[i] != INT_MAX)
      return;
  }
  assignments[0] = 0;
}

TheoryAssignment<int> TheorySolver::get_assignment() {
  //assume no neg cycles
  std::vector<int> assignments(num_vars, INT_MAX);
  if (name_to_id.find("z0") != name_to_id.end())
    assignments[name_to_id["z0"]] = 0;
  for (int i = 0; i < num_vars; i++) {
    if (out_edges[i].size() == 0)
      assignments[i] = 0;
  }
  random_assign_if_no_root(assignments);

  std::cout << "Initial Assignment: " << std::endl;

  for (int i = 0; i < num_vars; i++)
    std::cout << id_to_name[i] << ',';
  std::cout << std::endl;

  for (int i : assignments)
    std::cout << i << ',';
  std::cout << std::endl;
  
  bool bellman_continue = true;
  while (bellman_continue) {
    std::vector<int> next = assignments;
    for (int i = 0; i < num_vars; i++) {
      if (assignments[i] == INT_MAX)
        continue;
      for (const std::pair<int,int>& p : in_edges[i]) {
        next[p.first] = std::min(next[p.first], assignments[i] + p.second);
      }
    }
    bellman_continue = assignments != next;
    assignments = next;
    for (int i : assignments)
      std::cout << i << ',';
    std::cout << std::endl;
  }

  TheoryAssignment<int> result;
  int z0 = (name_to_id.find("z0") != name_to_id.end()) ? assignments[name_to_id["z0"]] : 0;
  for (int i = 0; i < num_vars; i++) {
    result.assign(id_to_name[i], assignments[i] - z0);
  }
  return result;
}

void TheorySolver::reset() {
  name_to_id.clear();
  id_to_name.clear();
  num_vars = 0;
  out_edges.clear();
  in_edges.clear();
}

std::optional<TheoryAssignment<int>> TheorySolver::solve(std::vector<Constraint> constraints) {
  std::cout << "Theory Solver" << std::endl;
  for (const auto& i : constraints) {
    i.print();
    std::cout << std::endl;
  }

  reset();
  initialize(constraints);
  if (neg_cycle_found())
    return std::nullopt;
  return get_assignment();
}
