cmake_minimum_required(VERSION 3.17)
project(FM)

set(CMAKE_CXX_STANDARD 14)

add_executable(FM main.cpp parse.h utils.h HyperGraph.h HyperGraph.cpp Partition.cpp Partition.h GainContainer.cpp GainContainer.h parse.cpp utils.cpp)
