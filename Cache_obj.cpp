#include "Cache_obj.h"
#include <cmath>
#include <iostream>
#include <fstream>

using namespace std;

Cache::Cache(int size, int assoc, int block_size, bool on) {
    if (on) {
        num_sets = size / (block_size * assoc);
        num_offset_bits = static_cast<int>(std::log(block_size) / std::log(2));
        num_index_bits = static_cast<int>(std::log(num_sets) / std::log(2));
        //Resize Cache to Proper Set Size
        cache.resize(num_sets);

        for (int i = 0; i < num_sets; i++) {
            cache[i].resize(assoc);
            for (int j = 0; j < assoc; j++) {
                cache[i][j].tag = 0;
                cache[i][j].index = 0;
                cache[i][j].LRU_Counter = 0;
                cache[i][j].data = 0;
                cache[i][j].dirty = false;
                cache[i][j].valid = false;
            }
        }
        //Generate Values for masks 
        mask_index = 0;
        mask_offset = 0;
        for (int i = 0; i < num_index_bits; i++) {
            mask_index = (mask_index << 1) | 1;
        }
        for (int i = 0; i < num_offset_bits; i++) {
            mask_offset = (mask_offset << 1) | 1;
        }
        lru_counter_max_val = assoc - 1;
        tag_placement_indicator = 0;
        tags_in_Set = assoc;
        //indicator for evicted address
        address_evicted_flag = false;
    }
}

bool Cache::read(int address, bool d_flag) {
    //Awaken The Sleeper (Reset The Kwitsatz Haderach)
    tag_placement_indicator = 0;
    tag_in = address >> (num_index_bits + num_offset_bits);
    index_in = (address >> num_offset_bits) & mask_index;
    offset_in = address & mask_offset;

    //Find if its there act accordingly
    for (; tag_placement_indicator <= lru_counter_max_val; tag_placement_indicator++) {
        if (cache[index_in][tag_placement_indicator].tag == tag_in) {
            if (d_flag) {
                cache[index_in][tag_placement_indicator].dirty = true;
            }
            return false;
        }
    }
    //Data is not in This Cache, Read is a miss.
    return true;
}

int Cache::write(int address, bool d_flag) {
    //Awaken The Sleeper (Reset The Kwitsatz Haderach)
    int wtag = address >> (num_index_bits + num_offset_bits);
    int windex = (address >> num_offset_bits) & mask_index;
    int woffset = address & mask_offset;
    tag_placement_indicator = 0;

    //Check for invalid bits if so dump it there index in order
    for (; tag_placement_indicator <= lru_counter_max_val; tag_placement_indicator++) {
        if (!cache[windex][tag_placement_indicator].valid) {
            cache[windex][tag_placement_indicator].tag = wtag;
            cache[windex][tag_placement_indicator].valid = true;
            cache[windex][tag_placement_indicator].data = woffset;
            cache[windex][tag_placement_indicator].index = tag_placement_indicator;
            //If the Cache is operating a Write Command Label the new bit Dirty
            if (d_flag) {
                cache[windex][tag_placement_indicator].dirty = true;
            }
            //No eviction so return address 0;
            return 0;
        }
    }

    //Get the Evict Location
    determine_victim(windex);
    //No Invalid Bits, Evict the last one First undo the math to seperate the bits
    int evicted_address = (
        (cache[windex][evic_loc].tag << (num_index_bits + num_offset_bits))
        | (cache[windex][evic_loc].index << num_offset_bits)
        | cache[windex][evic_loc].data
        );

    //Replace data at evicted spot
    cache[windex][evic_loc].tag = wtag;
    cache[windex][evic_loc].valid = true;
    if (d_flag) {
        cache[windex][evic_loc].dirty = true;   
    }
    else {
        cache[windex][evic_loc].dirty = false;   

    }
    cache[windex][evic_loc].data = woffset;
    //indicate and eviction
    address_evicted_flag = true;
    return(evicted_address);
}

void Cache::determine_victim(int windex) {
    evic_loc = 0; bool prior_dirty = false;
    for (int i = 0; i < tags_in_Set; i++) {
        if (cache[windex][evic_loc].LRU_Counter <= cache[windex][i].LRU_Counter) {
            if (!prior_dirty) {
                evic_loc = i;
                prior_dirty = cache[windex][i].dirty;
            }
            else if (prior_dirty & cache[windex][i].dirty) {
                evic_loc = i;
                prior_dirty = true;
            }
        }
    }
    return;
}

void Cache::update_stored() {
    //The Value is already there and just needs updated
    cache[index_in][tag_placement_indicator].valid = true;
    cache[index_in][tag_placement_indicator].dirty = true;
    cache[index_in][tag_placement_indicator].data = offset_in;
    cache[index_in][tag_placement_indicator].index = tag_placement_indicator;
    cache[index_in][tag_placement_indicator].tag = tag_in;
    return;
}

void Cache::LRU_Update() {
    if (tags_in_Set == 1) {
        return;
    }
    //An address has been evicted 
    else if (address_evicted_flag) {
        for (int i = 0; i < tags_in_Set; i++ ) {
            if (i == evic_loc) {
                cache[index_in][i].LRU_Counter = 0;
            }
            else {
                ++cache[index_in][i].LRU_Counter;
            }
        }
        //reset flag and return
        address_evicted_flag = false;
        return;
    }
    //There are >2 Tags per set and no evictions, a address was read or updated.
    else {
        for (int i = 0; i < tags_in_Set; i++) {
            if (i == tag_placement_indicator) {
                cache[index_in][i].LRU_Counter = 0;
            }  
            else if(cache[index_in][i].LRU_Counter == 0){
                ++cache[index_in][i].LRU_Counter;
            }
            else if (cache[index_in][i].LRU_Counter < lru_counter_max_val) {
                ++cache[index_in][i].LRU_Counter;
            }
        }
        return;
    }
}

void Cache::spit_out_data(std::ofstream& outputfile) {
    for (int i = 0; i < num_sets; i++) {
        outputfile << "set\t" << std::dec << i << ':' << '\t';
        for (int q = 0; q <= lru_counter_max_val; q++) {
            outputfile << std::hex << cache[i][q].tag << ' ';
            if (cache[i][q].dirty) {
                outputfile << ' ' << 'D' << ' ';
            }
            else {
                outputfile << "   ";
            }
        }
        outputfile << '\n';
    }
    return;
}