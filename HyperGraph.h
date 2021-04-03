#ifndef FM_HYPERGRAPH_H
#define FM_HYPERGRAPH_H

#include "parse.h"

class HyperGraph {
public:
    std::size_t vex_num = 0;
    std::size_t nets_num = 0;

    std::vector<std::vector<std::uint32_t>> nets_to_vertices;
    std::vector<std::vector<std::uint32_t>> vex_to_nets;

    ~HyperGraph() = default;
    HyperGraph(const std::string& filename) {
        std::ifstream file(filename);
        std::string line;
        std::uint32_t vex_id = 0;

        std::size_t net_id = 0;
        if (file.is_open()) {
            bool is_first = true;

            while(std::getline(file, line)) {
                if (is_first) {
                    std::vector<std::uint32_t> *first_line = parse_into_vec(line);
                    nets_num = first_line->at(0);
                    vex_num = first_line->at(1);

                    vex_to_nets = std::vector<std::vector<std::uint32_t>>(vex_num);
                    nets_to_vertices = std::vector<std::vector<std::uint32_t>>(nets_num);

                    is_first = false;
                } else {
                    std::stringstream stream_str(line);
                    while(stream_str >> vex_id) {
                        nets_to_vertices[net_id].push_back(vex_id - 1);
                        vex_to_nets[vex_id - 1].push_back(net_id);
                    }
                    net_id++;
                }
            }
            file.close();
        } else {
            throw std::runtime_error("Error during opening the file");
        }
    }
};

#endif //FM_HYPERGRAPH_H
