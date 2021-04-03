#include <chrono>
#include <iostream>
#include <string>

#include "utils.h"
#include "HyperGraph.h"

class Partition {
private:
    HyperGraph *graph;

    void init_vertices() {
        // размер левой части
        std::size_t len_left = (graph->vex_num) / 2;

        //0 - в левой, 1 - в правой части
        for (std::size_t i = len_left; i < graph->vex_num; i++) {
            vertices_part[i] = 1;
        }
    }

    void init_side_sizes() {
        for(std::size_t net_id = 0; net_id < graph->nets_num; net_id++) {
            std::vector<std::uint32_t> vx_from_net = graph->nets_to_vertices[net_id];
            for (auto vex_id : vx_from_net) {
                if (vertices_part[vex_id] == 1) {
                    right_net_sizes[net_id]++;
                } else {
                    left_net_sizes[net_id]++;
                }
            }
        }
    }

public:
    char *vertices_part;
    std::uint32_t *left_net_sizes;
    std::uint32_t *right_net_sizes;

    std::size_t solution_cost = 0;
    std::size_t balance = 0;


    ~Partition() = default;
    Partition(HyperGraph *graph) {
        this->graph = graph;
        this->left_net_sizes = (std::uint32_t*)std::calloc(graph->nets_num, sizeof(std::uint32_t));
        this->right_net_sizes = (std::uint32_t*)std::calloc(graph->nets_num, sizeof(std::uint32_t));
        this->vertices_part = (char*)std::calloc(graph->vex_num, sizeof(char));

        init_vertices();
        init_side_sizes();
    }
};

int main(int args, char **argv) {
    auto start = std::chrono::high_resolution_clock::now();

    std::string filename = get_input_file_name(args, argv);
    HyperGraph graph(filename);
    Partition partition(&graph);

    auto end = std::chrono::high_resolution_clock::now();
    std::cout << std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() << " mc" << std::endl;

    return 0;
}

