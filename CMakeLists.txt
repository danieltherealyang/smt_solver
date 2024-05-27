cmake_minimum_required(VERSION 3.0)
project(smt-solver)

# Set C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Gather all .cpp files under the src directory
file(GLOB_RECURSE SRC_FILES src/*.cpp)

# Add executable
add_executable(SMTsolver ${SRC_FILES})
