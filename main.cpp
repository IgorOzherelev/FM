#include <set>
#include <map>
#include <chrono>

#include "io.h"
#include "parse.h"

void load(const std::string& filename, std::set<std::uint32_t> *vx_names,
          std::size_t *vex_num, std::size_t *nets_num,
          std::map<std::size_t, std::set<std::uint32_t>> *net_to_vertices);
void init_side_sizes(std::set<std::uint32_t> *vx_names,
                     std::uint32_t *left, std::uint32_t *right,
                     const char *partition, std::map<std::size_t, std::set<std::uint32_t>> *net_to_vertices, const std::size_t *net_num);
inline char *init_partition(const std::size_t *vex_num, char *partition);

int main(int args, char **argv) {
    auto *vex_num = new std::size_t, *nets_num = new std::size_t;

    //stores vertexes names; index in set is equal to index in partition
    auto start = std::chrono::high_resolution_clock::now();
    auto *vx_names = new std::set<std::uint32_t>();

    std::map<std::size_t, std::set<std::uint32_t>> net_to_vertices;

    std::string filename = get_input_file_name(args, argv);
    load(filename, vx_names, vex_num, nets_num, &net_to_vertices);

    // stores nets sizes for each side
    auto *left_sizes = (std::uint32_t*)std::calloc(*vex_num, sizeof(std::uint32_t));
    auto *right_sizes = (std::uint32_t*)std::calloc(*vex_num, sizeof(std::uint32_t));
    //stores partition for vertexes
    char *partition = nullptr;
    partition = init_partition(vex_num, partition);

    init_side_sizes(vx_names, left_sizes, right_sizes, partition, &net_to_vertices, nets_num);
    auto end = std::chrono::high_resolution_clock::now();
    std::cout << std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() << " mc" << std::endl;

//    for (int i = 0; i < *nets_num; i++) {
//        std::cout << left_sizes[i] << " ";
//    }
//    std::cout << '\n';
//    for (int i = 0; i < *nets_num; i++) {
//        std::cout << right_sizes[i] << " ";
//    }
//    std::cout << '\n';
//    for (int i = 0; i < *vex_num; i++) {
//        std::cout << (int)partition[i] << " ";
//    }

    return 0;
}

inline char *init_partition(const std::size_t *vex_num, char *partition) {
    //0 - в левой, 1 - в правой части
    partition = (char*)std::calloc(*vex_num, sizeof(char));

    // размер левой части
    std::size_t len_left = (*vex_num) / 2;

    for (std::size_t i = len_left; i < *vex_num; i++) {
        partition[i] = 1;
    }

    return partition;
}

void load(const std::string& filename,
          std::set<std::uint32_t> *vx_names, std::size_t *vex_num,
          std::size_t *nets_num, std::map<std::size_t, std::set<std::uint32_t>> *net_to_vertices) {
    std::ifstream file(filename);
    std::string line;
    std::set<std::uint32_t> *parsed_line;

    std::size_t net_id = 0;
    if (file.is_open()) {
        bool first_line = true;

        while(std::getline(file, line)) {
            parsed_line = parse_into_set(line);
            if (first_line) {
                *nets_num = *(parsed_line->begin());
                auto set_it = parsed_line->begin();
                std::advance(set_it, 1);
                *vex_num = *set_it;

                first_line = false;
            } else {
                net_to_vertices->insert(std::pair<std::size_t, std::set<std::uint32_t>>(net_id, *parsed_line));
                for (auto it = parsed_line->begin(); it != parsed_line->end(); it++) {
                    vx_names->insert(*it);
                }
                net_id++;
            }
        }
        file.close();
    } else {
        throw std::runtime_error("Error during opening the file");
    }
}

void init_side_sizes(std::set<std::uint32_t> *vx_names, std::uint32_t *left,
                     std::uint32_t *right, const char *partition,
                     std::map<std::size_t, std::set<std::uint32_t>> *net_to_vertices, const std::size_t *net_num) {
    std::set<std::uint32_t> set_per_net;
    for (std::size_t i = 0; i < *net_num; i++) {
        set_per_net = net_to_vertices->find(i)->second;
        for (auto it = set_per_net.begin(); it != set_per_net.end(); it++) {
            auto name_it = vx_names->find(*it);
            std::size_t vx_id = std::distance(vx_names->begin(), name_it);
            if (partition[vx_id] == 1) {
                right[i]++;
            } else {
                left[i]++;
            }
        }
    }
}

