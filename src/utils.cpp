#include "../headers/utils.h"

std::string get_input_file_name(const std::string& filename) {
    std::string file_name = "/";
    file_name = file_name.append(filename);

    std::string pwd = get_pwd();
    file_name = pwd.append(file_name);

    return file_name;
}

std::string get_pwd() {
    char buff[FILENAME_MAX];
    PWD(buff, FILENAME_MAX);
    std::string current_dir(buff);

    return current_dir;
}

