#include "../headers/GainContainer.h"

void GainContainer::init() {
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

int GainContainer::compute_vex_gain(std::uint32_t vex_id) {
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

GainContainer::~GainContainer() {
    std::free(free_array);
    std::free(vex_gains);
}

GainContainer::GainContainer(Partition *partition) {
    this->partition = partition;
    this->free_array = (char*)std::calloc(partition->get_graph()->vex_num, sizeof(char));
    this->vex_gains = (int*)std::malloc(sizeof(int) * partition->get_graph()->vex_num);
    init();
}

bool GainContainer::is_empty() const {
    return left.empty() || right.empty();
}

bool GainContainer::is_all_locked() {
    return lock_cnt == partition->get_graph()->vex_num;
}

std::pair<std::uint32_t, int> GainContainer::feasible_move() {
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

std::pair<std::uint32_t, int> GainContainer::feasible_move_modified() {
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
        moved_vex_id = *(--(max_right_bucket->second).end());
        max_right_bucket->second.erase(--max_right_bucket->second.end());

        if (max_right_bucket->second.empty())
            right.erase(max_gain_right);

        moved_vex_gain = max_gain_right;
    } else { // move from left to right
        moved_vex_id = *(--(max_left_bucket->second).end());
        max_left_bucket->second.erase(--max_left_bucket->second.end());

        if (max_left_bucket->second.empty())
            left.erase(max_gain_left);

        moved_vex_gain = max_gain_left;
    }

    // lock vertex
    free_array[moved_vex_id] = 1;
    lock_cnt++;
    return {moved_vex_id, moved_vex_gain};
}

void GainContainer::update(std::uint32_t vex_id, int weight) {
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

void GainContainer::free(std::pair<std::uint32_t, int> best_move, char cur_side) {
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
