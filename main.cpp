#include <chrono>
#include <iostream>
#include <map>
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
    Partition(HyperGraph *graph, std::size_t balance) {
        this->graph = graph;
        this->left_net_sizes = (std::uint32_t*)std::calloc(graph->nets_num, sizeof(std::uint32_t));
        this->right_net_sizes = (std::uint32_t*)std::calloc(graph->nets_num, sizeof(std::uint32_t));
        this->vertices_part = (char*)std::calloc(graph->vex_num, sizeof(char));
        this->balance = graph->vex_num % 2;

        init_vertices();
        init_side_sizes();
    }

    std::uint32_t get_cost() {
        for (std::size_t i = 0; i < graph->nets_to_vertices.size(); i++) {
            char cur_side = vertices_part[graph->nets_to_vertices[i][0]];
            for (std::size_t j = 0; j < graph->nets_to_vertices[i].size(); j++) {
                if(cur_side != vertices_part[graph->nets_to_vertices[i][j]]) {
                    solution_cost++;
                    break;
                }
            }
        }

        return solution_cost;
    }

    void update(std::uint32_t vex_id) {
        if (vertices_part[vex_id] == 1) {
            vertices_part[vex_id] = 0;
            balance--;
        } else {
            vertices_part[vex_id] = 1;
            balance++;
        }
    }

    HyperGraph *get_graph(){
        return this->graph;
    }
};


class GainContainer {
private:
    Partition *partition;

    void init() {
        for (std::uint32_t vex_id = 0; vex_id < partition->get_graph()->vex_num; vex_id++) {
            int vx_gain = compute_vex_gain(vex_id);

            if (partition->vertices_part[vex_id] == 0) {
                left[vx_gain].push_back(vex_id);
            } else {
                right[vx_gain].push_back(vex_id);
            }
        }
    }

    int compute_vex_gain(std::uint32_t vex_id) {
        int vx_gain = 0;
        char cur_vex_side = partition->vertices_part[vex_id];

        std::vector<uint32_t> tmp = partition->get_graph()->vex_to_nets[vex_id];
        for(std::size_t j = 0; j < tmp.size(); j++) {
            if (cur_vex_side == 0) {
                if (partition->left_net_sizes[tmp[j]] == 1)
                    vx_gain++;
                if (partition->right_net_sizes[tmp[j]] == 0)
                    vx_gain--;
            } else {
                if (partition->right_net_sizes[tmp[j]] == 1)
                    vx_gain++;
                if(partition->left_net_sizes[tmp[j]] == 0)
                    vx_gain--;
            }
        }

        return vx_gain;
    }
public:
    std::map<int, std::vector<std::uint32_t>> left;
    std::map<int, std::vector<std::uint32_t>> right;


    ~GainContainer() = default;
    GainContainer(Partition *partition) {
        this->partition = partition;
        init();
    }

};

int main(int args, char **argv) {
    auto start = std::chrono::high_resolution_clock::now();

    std::string filename = get_input_file_name(args, argv);
    HyperGraph graph(filename);
    Partition partition(&graph, 0);
    GainContainer container(&partition);

    auto end = std::chrono::high_resolution_clock::now();
    std::cout << std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() << " mc" << std::endl;

    return 0;
}

