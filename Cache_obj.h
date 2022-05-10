#ifndef CACHE_OBJ_H
#define CACHE_OBJ_H
#include <vector>
#include <algorithm>
#include <fstream>

using Cache_Line = struct Cache_Line_t {
    int tag;
    int index;
    int LRU_Counter;
    int data;
    bool dirty;
    bool valid;
};

class Cache {
public:
    Cache(int size, int assoc, int block_size, bool on);
    bool read(int address, bool w_flag = false);
    int write(int address, bool w_flag = false); //int to pass up address
    void update_stored();
    void determine_victim(int windex);
    void LRU_Update();
    void spit_out_data(std::ofstream&);

private:
    std::vector<std::vector<Cache_Line>> cache;
    int tags_in_Set;
    int num_sets;
    int num_offset_bits;
    int num_index_bits;
    int mask_index;
    int mask_offset;
    int tag_in;
    int index_in;
    int offset_in;
    int lru_counter_max_val;
    int tag_placement_indicator;
    int evic_loc;
    bool address_evicted_flag;
};

#endif