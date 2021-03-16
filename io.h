#ifndef FM_IO_H
#define FM_IO_H

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


inline std::string get_pwd();
inline std::string get_input_file_name(int args, char** argv);

inline std::string get_input_file_name(int args, char** argv) {
    std::string file_name = "/";
    if (args == 1 || args == 0 || args > 2) {
        //std::cout << "Wrong set of args, enter only the file name";
        //std::exit(FILE_NAME_ERROR);
        file_name = "/test.hgr";
        //file_name = "/ISPD98_ibm01.hgr";

    } else {
        file_name = file_name.append(argv[1]);
    }

    std::string pwd = get_pwd();
    file_name = pwd.append(file_name);

    return file_name;
}

inline std::string get_pwd() {
    char buff[FILENAME_MAX];
    PWD(buff, FILENAME_MAX);
    std::string current_dir(buff);

    return current_dir;
}

#endif //FM_IO_H
