#ifndef FM_GAINCONTAINER_H
#define FM_GAINCONTAINER_H

#include "Partition.h"
#include <map>
#include <set>
#include <list>

class GainContainer {
private:
    Partition *partition;

    void init();

    int compute_vex_gain(std::uint32_t vex_id);
public:
    std::map<int, std::list<std::uint32_t>> left;
    std::map<int, std::list<std::uint32_t>> right;
    std::list<std::uint32_t>::iterator *vex_iters;

    // массив для блокировки вершин
    // 0 - вершина свободна, 1 - занята
    char *free_array;
    std::size_t lock_cnt = 0;
    // массив gain для каждой вершины
    int *vex_gains;

    ~GainContainer();
    explicit GainContainer(Partition *partition);

    bool is_all_locked();

    std::pair<std::uint32_t, int> feasible_move();
    std::pair<std::uint32_t, int> feasible_move_modified();

    void update(std::uint32_t vex_id, int weight);

};


#endif //FM_GAINCONTAINER_H
