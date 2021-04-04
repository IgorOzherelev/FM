#include <chrono>
#include <iostream>
#include <map>
#include <string>
#include <vector>
#include <set>

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
    std::int64_t balance = 0;
    std::size_t imbalance = 0;

    ~Partition() {
        std::free(left_net_sizes);
        std::free(right_net_sizes);
    }
    Partition(HyperGraph *graph, std::size_t imbalance) {
        this->graph = graph;
        this->left_net_sizes = (std::uint32_t*)std::calloc(graph->nets_num, sizeof(std::uint32_t));
        this->right_net_sizes = (std::uint32_t*)std::calloc(graph->nets_num, sizeof(std::uint32_t));
        this->vertices_part = (char*)std::calloc(graph->vex_num, sizeof(char));
        this->balance = graph->vex_num % 2;
        this->imbalance = imbalance;

        init_vertices();
        init_side_sizes();
    }

    void rollback_sizes() {
        std::free(left_net_sizes);
        std::free(right_net_sizes);

        left_net_sizes = (std::uint32_t*)std::calloc(graph->nets_num, sizeof(std::uint32_t));
        right_net_sizes = (std::uint32_t*)std::calloc(graph->nets_num, sizeof(std::uint32_t));
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

    HyperGraph *get_graph() {
        return this->graph;
    }

    void store(const std::string& filename) {
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
};


class GainContainer {
private:
    Partition *partition;

    void init() {
        for (std::uint32_t vex_id = 0; vex_id < partition->get_graph()->vex_num; vex_id++) {
            int vx_gain = compute_vex_gain(vex_id);

            if (partition->vertices_part[vex_id] == 0) {
                left[vx_gain].insert(vex_id);
            } else {
                right[vx_gain].insert(vex_id);
            }
            vex_gains[vex_id] = vx_gain;
        }
    }

    int compute_vex_gain(std::uint32_t vex_id) {
        int vx_gain = 0;
        char cur_vex_side = partition->vertices_part[vex_id];

        std::vector<std::uint32_t> tmp = partition->get_graph()->vex_to_nets[vex_id];
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
    std::map<int, std::set<std::uint32_t>> left;
    std::map<int, std::set<std::uint32_t>> right;

    // массив для блокировки вершин
    // 0 - вершина свободна, 1 - занята
    char *free_array;
    std::size_t lock_cnt = 0;
    // массив gain для каждой вершины
    int *vex_gains;

    ~GainContainer() {
        std::free(free_array);
        std::free(vex_gains);
    }
    GainContainer(Partition *partition) {
        this->partition = partition;
        this->free_array = (char*)std::calloc(partition->get_graph()->vex_num, sizeof(char));
        this->vex_gains = (int*)std::malloc(sizeof(int) * partition->get_graph()->vex_num);
        init();
    }

    bool is_all_locked() {
        return lock_cnt == partition->get_graph()->vex_num;
    }

    std::pair<std::uint32_t, int> feasible_move() {
        bool left_is_empty = left.empty();
        bool right_is_empty = right.empty();

        int max_gain_right, max_gain_left;
        std::map<int, std::set<std::uint32_t>>::iterator max_left_bucket, max_right_bucket;
        if (left_is_empty) {
            max_right_bucket = --right.end();
            max_gain_right = max_right_bucket->first;
        } else if (right_is_empty) {
            max_left_bucket = --left.end();
            max_gain_left = max_left_bucket->first;

        }

        if (!left_is_empty && !right_is_empty) {
            max_left_bucket = --left.end();
            max_right_bucket = --right.end();

            max_gain_right = max_right_bucket->first;
            max_gain_left = max_left_bucket->first;
        }

        if (left_is_empty && right_is_empty) {
            throw std::runtime_error("Left and Right containers are empty!");
        }

        std::uint32_t moved_vex_id;
        int moved_vex_gain;
        // move from right to left
        if (left_is_empty || (max_gain_left < max_gain_right && -partition->balance < partition->imbalance)
                || (partition->balance >= partition->imbalance)) {
            moved_vex_id = *((max_right_bucket->second).begin());
            max_right_bucket->second.erase(max_right_bucket->second.begin());

            if (max_right_bucket->second.empty())
                right.erase(max_gain_right);

            moved_vex_gain = max_gain_right;
        } else { // move from left to right
            moved_vex_id = *((max_left_bucket->second).begin());
            max_left_bucket->second.erase(max_left_bucket->second.begin());

            if (max_left_bucket->second.empty())
                left.erase(max_gain_left);

            moved_vex_gain = max_gain_left;
        }

        // lock vertex
        free_array[moved_vex_id] = 1;
        lock_cnt++;
        return {moved_vex_id, moved_vex_gain};
    }

    void update(std::uint32_t vex_id, int weight) {
        if (free_array[vex_id] == 1) {
            return;
        }

        int vex_gain = vex_gains[vex_id];
        vex_gains[vex_id] += weight;

        char cur_side = partition->vertices_part[vex_id];
        if (cur_side == 1) {
            right[vex_gain].erase(vex_id);
            if (right[vex_gain].empty())
                right.erase(vex_gain);

            right[vex_gains[vex_id]].insert(vex_id);
        } else {
            left[vex_gain].erase(vex_id);
            if (left[vex_gain].empty())
                left.erase(vex_gain);

            left[vex_gains[vex_id]].insert(vex_id);
        }
    }

    void erase(std::pair<std::uint32_t, int> best_move, char cur_side) {
        if (cur_side == 1) {
            right[best_move.second].erase(best_move.first);
            if (right[best_move.second].empty())
                right.erase(best_move.second);
        } else {
            left[best_move.second].erase(best_move.first);
            if (left[best_move.second].empty())
                left.erase(best_move.second);
        }
    }
};

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

    // erase spare memory in container
    // upd partition
    container->erase(best_move, cur_side);
    partition->update(best_move_vex_id);
}

std::uint32_t FMPass(GainContainer *container, Partition *partition) {
    std::uint32_t solution_cost = partition->get_cost();
    std::uint32_t best_cost = solution_cost;

    std::vector<std::uint32_t> rollback_vex;
    std::size_t step = 1;
    while (!container->is_all_locked()) {
        auto best_move = container->feasible_move();
        rollback_vex.push_back(best_move.first);
        solution_cost -= best_move.second;
        std::cout << "step: " << step++ << " best_cost: " << best_cost << " balance: " << partition->balance <<"\n";
        if (solution_cost < best_cost) {
            best_cost = solution_cost;
            rollback_vex.erase(rollback_vex.begin(), rollback_vex.end());
        }
        apply(container, partition, best_move);
    }

    partition->store("final-partition.txt");
    // rollback partition
    for (auto vex : rollback_vex) {
        partition->update(vex);
    }
    partition->rollback_sizes();

    return best_cost;
}

void FM(Partition *partition) {
    bool is_quality_improved = false;

    while (!is_quality_improved) {
        GainContainer container(partition);
        std::uint32_t best_cost = FMPass(&container, partition);
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
    partition.store("init-partition.txt");

    //FM(&partition);
    GainContainer container(&partition);
    std::cout << "best cost: " << FMPass(&container, &partition) << "\n";

    //partition.store("final-partition.txt");

    auto end = std::chrono::high_resolution_clock::now();
    std::cout << std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() << " mc" << std::endl;

    return 0;
}

