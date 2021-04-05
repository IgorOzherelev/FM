#include <chrono>
#include <iostream>
#include <string>
#include <vector>

#include "headers/HyperGraph.h"
#include "headers/Partition.h"
#include "headers/GainContainer.h"

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

std::uint32_t FMPass(GainContainer *container, Partition *partition, bool is_modified) {
    std::uint32_t solution_cost = partition->get_cost();
    std::uint32_t best_cost = solution_cost;

    std::vector<std::uint32_t> rollback_vex;
    std::pair<std::uint32_t, int> best_move;
    while (!container->is_empty()) {
        if (is_modified)
            best_move = container->feasible_move_modified();
        else
            best_move = container->feasible_move();
        rollback_vex.push_back(best_move.first);
        solution_cost -= best_move.second;
        if (solution_cost < best_cost) {
            best_cost = solution_cost;
            rollback_vex.erase(rollback_vex.begin(), rollback_vex.end());
        }
        apply(container, partition, best_move);
    }

    partition->rollback(rollback_vex);

    return best_cost;
}

void FM(Partition *partition, const std::string& logfile_name, bool is_modified) {
    bool is_quality_improved = false;
    std::ofstream logfile(logfile_name);

    auto start = std::chrono::high_resolution_clock::now();

    std::uint32_t step = 1;
    logfile << "[GRAPH]: " << "[NETS:] " << partition->get_graph()->nets_num << " [VERTICES:] " << partition->get_graph()->vex_num << std::endl;
    logfile << "[PARTITION PARAMETERS]: " << "[IMBALANCE]: " << partition->imbalance << std::endl;
    while (!is_quality_improved) {
        auto container = new GainContainer(partition);
        std::uint32_t best_cost = FMPass(container, partition, is_modified);
        logfile << "[STEP]: " << step++ << " [BEST COST]: " << best_cost << " [BALANCE]: " << partition->balance << std::endl;
        if (best_cost == partition->solution_cost)
            is_quality_improved = true;
        else
            partition->solution_cost = best_cost;
    }
    auto end = std::chrono::high_resolution_clock::now();
    logfile << "[EXECUTION TIME]: " << std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() << " mc" << std::endl;
    logfile << "[EXECUTION TIME]: " << std::chrono::duration_cast<std::chrono::seconds>(end - start).count() << " sec" << std::endl;

    logfile.close();
}

void run_benchmarks(const std::vector<std::string>& names, bool is_modified) {
    for (const auto& name : names) {
        std::cout << name << " test is started " << std::endl;
        std::string filename = get_input_file_name(name);
        auto *graph = new HyperGraph(filename);
        auto *partition = new Partition(graph, 1);

        std::string output_filename = name;
        std::size_t pos_hgr = output_filename.find(".hgr");
        output_filename = output_filename.substr(0, pos_hgr);
        std::string logfile_name = output_filename;

        output_filename.append(".part.2");
        logfile_name.append(".log");

        FM(partition, logfile_name, is_modified);
        partition->store(output_filename);
    }
}

int main(int args, char **argv) {
    std::string version;
    std::cout << "Enter modified or base:" << std::endl;
    std::cin >> version;

    bool is_modified;
    if (version == "modified")
        is_modified = true;
    else
        is_modified = false;
    std::cout << "(version == modified): " << is_modified << std::endl;

    if (args == 1) {
        std::cout << "Starting all ISPD98_ibmXX.hgr tests " << "XX - 01, 02,...., 18" << std::endl;

        std::vector<std::string> ispd98_names = {
                "ISPD98_ibm01.hgr", "ISPD98_ibm02.hgr", "ISPD98_ibm03.hgr", "ISPD98_ibm04.hgr",
                "ISPD98_ibm05.hgr", "ISPD98_ibm06.hgr", "ISPD98_ibm07.hgr", "ISPD98_ibm08.hgr",
                "ISPD98_ibm09.hgr", "ISPD98_ibm10.hgr", "ISPD98_ibm11.hgr", "ISPD98_ibm12.hgr",
                "ISPD98_ibm13.hgr", "ISPD98_ibm14.hgr", "ISPD98_ibm15.hgr", "ISPD98_ibm16.hgr",
                "ISPD98_ibm17.hgr", "ISPD98_ibm18.hgr"
        };

        run_benchmarks(ispd98_names, is_modified);

        std::cout << "Starting all dac2012_superblueXX " << "XX - 2, 3, 6, 7, 9, 11, 12, 14, 16, 19" << std::endl;
        std::vector<std::string> dac2012_names = {
                "dac2012_superblue2.hgr", "dac2012_superblue3.hgr", "dac2012_superblue6.hgr",
                "dac2012_superblue7.hgr", "dac2012_superblue9.hgr", "dac2012_superblue11.hgr",
                "dac2012_superblue12.hgr", "dac2012_superblue14.hgr", "dac2012_superblue16.hgr",
                "dac2012_superblue19.hgr"
        };

        run_benchmarks(dac2012_names, is_modified);
        std::cout << "All tests have been finished" << std::endl;
    }

    if (args == 2) {
        std::cout << "Run specific test " << argv[1] << std::endl;

        std::vector<std::string> name;
        name.emplace_back(argv[1]);
        run_benchmarks(name, is_modified);
    } else if (args > 2) {
        std::cout << "[PARAMETERS]: Wrong set of args, enter only the file name to run specific test" << std::endl;
        std::cout << "[HOW TO RUN]: Executable file should be in the same directory as the benchmarks" << std::endl;
        std::cout << "[RESULTS]: Results will be stored in the same directory as the benchmarks"<< std::endl;
        std::exit(-255);
    }

    return 0;
}

