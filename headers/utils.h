#ifndef FM_UTILS_H
#define FM_UTILS_H

#ifdef WINDOWS
#include <direct.h>
#define PWD _getcwd
#else
#include <unistd.h>
#define PWD getcwd
#endif

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <cstdlib>

std::string get_pwd();
std::string get_input_file_name(const std::string& filename);

#endif //FM_UTILS_H
