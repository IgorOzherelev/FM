//
// Created by i9163 on 03.04.2021.
//

#ifndef FM_PARTITION_H
#define FM_PARTITION_H

#include "HyperGraph.h"

class Partition {
private:
    HyperGraph graph;
public:
    char *vertices_part;
    auto *left_sizes = nullptr;
    auto *right_sizes = (std::uint32_t*)std::calloc(*vex_num, sizeof(std::uint32_t));

    ~Partition() = default;
    Partition(HyperGraph graph) {
        this->graph = graph;
        this->left_sizes = (std::uint32_t*)std::calloc(*, sizeof(std::uint32_t));
        init();
    }

    void init();
};

#endif //FM_PARTITION_H
