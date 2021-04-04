#ifndef FM_HYPERGRAPH_H
#define FM_HYPERGRAPH_H

#include "parse.h"
#include "utils.h"

class HyperGraph {
public:
    std::size_t vex_num = 0;
    std::size_t nets_num = 0;

    std::vector<std::vector<std::uint32_t>> nets_to_vertices;
    std::vector<std::vector<std::uint32_t>> vex_to_nets;

    ~HyperGraph() = default;
    explicit HyperGraph(const std::string& filename);
};

#endif //FM_HYPERGRAPH_H
