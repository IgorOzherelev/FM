#ifndef FM_PARSE_H
#define FM_PARSE_H

#include <iostream>
#include <string>
#include <vector>

inline std::vector<std::uint32_t> *parse_into_vector(const std::string& line);

inline std::vector<std::uint32_t> *parse_into_vector(const std::string& line) {
    std::stringstream stream_str(line);
    auto *vec = new std::vector<std::uint32_t>();

    std::uint32_t var;
    while (stream_str >> var) {
        vec->push_back(var);
    }

    return vec;
}

#endif //FM_PARSE_H
