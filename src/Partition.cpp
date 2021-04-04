//
// Created by i9163 on 04.04.2021.
//

#include "../headers/Partition.h"

void Partition::init_vertices() {
    // размер левой части
    std::size_t len_left = (graph->vex_num) / 2;

    //0 - в левой, 1 - в правой части
    for (std::size_t i = len_left; i < graph->vex_num; i++) {
        vertices_part[i] = 1;
    }
}

void Partition::init_side_sizes() {
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

Partition::~Partition() {
    std::free(left_net_sizes);
    std::free(right_net_sizes);
}

Partition::Partition(HyperGraph *graph, std::size_t imbalance) {
    this->graph = graph;
    this->left_net_sizes = (std::uint32_t*)std::calloc(graph->nets_num, sizeof(std::uint32_t));
    this->right_net_sizes = (std::uint32_t*)std::calloc(graph->nets_num, sizeof(std::uint32_t));
    this->vertices_part = (char*)std::calloc(graph->vex_num, sizeof(char));
    this->balance = graph->vex_num % 2;
    this->imbalance = imbalance;

    init_vertices();
    init_side_sizes();
}

void Partition::rollback_sizes() {
    std::free(left_net_sizes);
    std::free(right_net_sizes);

    left_net_sizes = (std::uint32_t*)std::calloc(graph->nets_num, sizeof(std::uint32_t));
    right_net_sizes = (std::uint32_t*)std::calloc(graph->nets_num, sizeof(std::uint32_t));
    init_side_sizes();
}

std::uint32_t Partition::get_cost() {
    solution_cost = 0;
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

void Partition::update(std::uint32_t vex_id) {
    if (vertices_part[vex_id] == 1) {
        vertices_part[vex_id] = 0;
        balance--;
    } else {
        vertices_part[vex_id] = 1;
        balance++;
    }
}

void Partition::store(const std::string &filename) {
    std::ofstream out_file;
    out_file.open(filename);
    for (int i = 0; i < graph->vex_num; i++) {
        if (vertices_part[i] == 1)
            out_file << 1 << "\n";
        else
            out_file << 0 << "\n";
    }
    out_file.close();
}

HyperGraph *Partition::get_graph() {
    return this->graph;
}
