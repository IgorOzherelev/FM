cmake_minimum_required(VERSION 3.17)
project(FM)

set(CMAKE_CXX_STANDARD 14)

add_executable(FM main.cpp headers/parse.h headers/utils.h headers/HyperGraph.h src/HyperGraph.cpp src/Partition.cpp headers/Partition.h src/GainContainer.cpp headers/GainContainer.h src/parse.cpp src/utils.cpp)
