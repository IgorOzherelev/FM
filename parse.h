#ifndef FM_PARSE_H
#define FM_PARSE_H

#include <iostream>
#include <string>
#include <set>

inline std::set<std::uint32_t> *parse_into_set(const std::string& line);

inline std::set<std::uint32_t> *parse_into_set(const std::string& line) {
    std::stringstream stream_str(line);
    auto *set = new std::set<std::uint32_t>();

    std::uint32_t var;
    while (stream_str >> var) {
        set->insert(var);
    }

    return set;
}

#endif //FM_PARSE_H
