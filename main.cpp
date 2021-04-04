#include <chrono>
#include <iostream>
#include <string>
#include <vector>

#include "HyperGraph.h"
#include "Partition.h"
#include "GainContainer.h"

void apply(GainContainer *container, Partition *partition, std::pair<std::uint32_t, int> best_move) {
    std::uint32_t best_move_vex_id = best_move.first;
    HyperGraph *graph = partition->get_graph();
    auto vex_to_nets = graph->vex_to_nets[best_move_vex_id];

    char cur_side = partition->vertices_part[best_move_vex_id];
    std::uint32_t *from_block_sizes = (cur_side == 0) ? partition->left_net_sizes : partition->right_net_sizes;
    std::uint32_t *to_block_sizes = (cur_side == 0) ? partition->right_net_sizes : partition->left_net_sizes;
    for (std::size_t net_idx = 0; net_idx < vex_to_nets.size(); net_idx++) {
        std::uint32_t cur_net_id = vex_to_nets[net_idx];

        if (to_block_sizes[cur_net_id] == 0) {
            // increment gain of all free cells on current net
            for (auto vex_id : graph->nets_to_vertices[cur_net_id]) {
                if (container->free_array[vex_id] == 0) {
                    container->update(vex_id, 1);
                }
            }
        } else if (to_block_sizes[cur_net_id] == 1) {
            // decrement gain of only T cell on current net, if it is free
            for (auto vex_id : graph->nets_to_vertices[cur_net_id]) {
                if (container->free_array[vex_id] == 0 && partition->vertices_part[vex_id] != cur_side) {
                    container->update(vex_id, -1);
                }
            }
        }

        // decrement F(n)
        // increment T(n)
        from_block_sizes[cur_net_id]--;
        to_block_sizes[cur_net_id]++;

        /*
         * if F(n) == 0 THEN decrement gains of all free cells on net
         * else if F(n) == 1 THEN increment gain of the only F cell on net, if it is free
         * */
        if (from_block_sizes[cur_net_id] == 0) {
            for (auto vex_id : graph->nets_to_vertices[cur_net_id]) {
                if (container->free_array[vex_id] == 0) {
                    container->update(vex_id, -1);
                }
            }
        } else if (from_block_sizes[cur_net_id] == 1) {
            for (auto vex_id : graph->nets_to_vertices[cur_net_id]) {
                if (container->free_array[vex_id] == 0 && partition->vertices_part[vex_id] == cur_side) {
                    container->update(vex_id, 1);
                }
            }
        }
    }

    // free spare memory in container
    // upd partition
    container->free(best_move, cur_side);
    partition->update(best_move_vex_id);
}

std::uint32_t FMPass(GainContainer *container, Partition *partition) {
    std::uint32_t solution_cost = partition->get_cost();
    std::uint32_t best_cost = solution_cost;

    std::vector<std::uint32_t> rollback_vex;
    while (!container->is_empty()) {
        auto best_move = container->feasible_move();
        rollback_vex.push_back(best_move.first);
        solution_cost -= best_move.second;
        if (solution_cost < best_cost) {
            best_cost = solution_cost;
            rollback_vex.erase(rollback_vex.begin(), rollback_vex.end());
        }
        apply(container, partition, best_move);
    }

    for (auto vex : rollback_vex) {
        partition->update(vex);
    }
    partition->rollback_sizes();

    return best_cost;
}

void FM(Partition *partition) {
    bool is_quality_improved = false;

    std::uint32_t step = 1;
    while (!is_quality_improved) {
        auto container = new GainContainer(partition);
        std::uint32_t best_cost = FMPass(container, partition);
        std::cout << "step: " << step++ << " best_cost: " << best_cost << " balance: " << partition->balance <<"\n";
        if (best_cost == partition->solution_cost)
            is_quality_improved = true;
        else
            partition->solution_cost = best_cost;
    }
}

int main(int args, char **argv) {
    auto start = std::chrono::high_resolution_clock::now();

    std::string filename = get_input_file_name(args, argv);
    HyperGraph graph(filename);
    Partition partition(&graph, 1);

    FM(&partition);
    partition.store("final-partition.txt");

    auto end = std::chrono::high_resolution_clock::now();
    std::cout << std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() << " mc" << std::endl;

    return 0;
}

