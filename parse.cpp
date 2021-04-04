#include "parse.h"

std::vector<std::uint32_t> *parse_into_vec(const std::string& line) {
    std::stringstream stream_str(line);
    auto *vec = new std::vector<std::uint32_t>();

    std::uint32_t var;
    while (stream_str >> var) {
        vec->push_back(var);
    }

    return vec;
}