#include <set>
#include <chrono>

#include "io.h"
#include "parse.h"

void load(const std::string& filename, std::set<std::uint32_t> *vx_names,
          std::size_t *vex_num, std::size_t *nets_num);
void init_side_sizes(const std::string& filename, std::set<std::uint32_t> *vx_names,
                     std::uint32_t *left, std::uint32_t *right,
                     const char *partition);
inline char *init_partition(const std::size_t *vex_num, char *partition);

int main(int args, char **argv) {
    auto *vex_num = new std::size_t, *nets_num = new std::size_t;

    //stores vertexes names; index in set is equal to index in partition
    auto start = std::chrono::high_resolution_clock::now();
    auto *vx_names = new std::set<std::uint32_t>();

    std::string filename = get_input_file_name(args, argv);
    load(filename, vx_names, vex_num, nets_num);

    // stores nets sizes for each side
    auto *left_sizes = (std::uint32_t*)std::calloc(*vex_num, sizeof(std::uint32_t));
    auto *right_sizes = (std::uint32_t*)std::calloc(*vex_num, sizeof(std::uint32_t));
    //stores partition for vertexes
    char *partition = nullptr;
    partition = init_partition(vex_num, partition);

    init_side_sizes(filename, vx_names, left_sizes, right_sizes, partition);
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
          std::set<std::uint32_t> *vx_names, std::size_t *vex_num, std::size_t *nets_num) {
    std::ifstream file(filename);
    std::string line;
    std::vector<std::uint32_t> *parsed_line;

    if (file.is_open()) {
        bool first_line = true;

        while(std::getline(file, line)) {
            parsed_line = parse_into_vector(line);
            if (first_line) {
                *nets_num = parsed_line->at(0);
                *vex_num = parsed_line->at(1);

                first_line = false;
            } else {
                for (auto it = parsed_line->begin(); it != parsed_line->end(); it++) {
                    vx_names->insert(*it);
                }
            }
        }
        file.close();
    } else {
        throw std::runtime_error("Error during opening the file");
    }
}

void init_side_sizes(const std::string& filename, std::set<std::uint32_t> *vx_names, std::uint32_t *left,
                     std::uint32_t *right, const char *partition) {
    std::ifstream file(filename);
    std::string line;
    std::vector<std::uint32_t> *parsed_line;

    std::getline(file, line);
    std::size_t net_id = 0;
    while(std::getline(file, line)) {
        parsed_line = parse_into_vector(line);
        for (auto it = parsed_line->begin(); it != parsed_line->end(); it++) {
            auto set_it = vx_names->find(*it);
            std::size_t vx_id = std::distance(vx_names->begin(), set_it);
            if (partition[vx_id] == 1) {
                right[net_id]++;
            } else {
                left[net_id]++;
            }
        }
        net_id++;
    }
    file.close();
}

