//
// Created by i9163 on 04.04.2021.
//

#ifndef FM_PARTITION_H
#define FM_PARTITION_H

#include "HyperGraph.h"

class Partition {
private:
    HyperGraph *graph;

    void init_vertices();

    void init_side_sizes();
public:
    char *vertices_part;
    std::uint32_t *left_net_sizes;
    std::uint32_t *right_net_sizes;

    std::size_t solution_cost = 0;
    std::int64_t balance = 0;
    std::size_t imbalance = 0;

    ~Partition();
    Partition(HyperGraph *graph, std::size_t imbalance);

    void rollback_sizes();

    std::uint32_t get_cost();

    void update(std::uint32_t vex_id);

    HyperGraph *get_graph();

    void store(const std::string& filename);
};


#endif //FM_PARTITION_H
