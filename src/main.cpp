#include "SMTSolver.h"
#include <iostream>
#include <fstream>
#include <string>

int main(int argc, char* argv[]) {
  // Check if the correct number of arguments is provided
  if (argc != 2) {
    std::cerr << "Usage: " << argv[0] << " <filename>" << std::endl;
    return 1;
  }

  // Get the file name from the command line argument
  std::string filename = argv[1];

  SMTSolver solver(filename);
  solver.print_constraints();
  solver.solve();
  return 0;
}

