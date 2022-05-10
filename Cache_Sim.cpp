//**************************************************************************//
// Project  ECE 483 Cache Simulator                                         //
// Author:  Ryan Bride                                                      //
// File:    Cache_Sim.cpp                                                   //
// Note:    Num Address bits = 32                                           //
//          address -> [tag][index][offset]                                 //
//          LRU is only 0 or 1 as assoc is 1 or 2 in test cases therefore   //
//          for ease of dev I do not dynamically control the LRU I just     //
//          Swap the first and second place if its the item is not there    //
//         - Ryan Bride                                                     //
//**************************************************************************//
#include "Cache_obj.h"
#include <iostream>
#include <fstream>
#include <string>

using namespace std;
static bool L2_flag = false;
void cache_sim();

int main() {
    while (true) {
        int mode = 0;
        cout << "Enter 1 to begin Cache Simulator or 2 to Exit \n>: ";
        cin >> mode; cout << '\n';
        switch (mode) {
        case 1:
            cache_sim();

            break;
        case 2:
            return 0;
        default:
            cout << "\nInvalid Entry";
            break;
        }
    }
    return 0;
}

void cache_sim() {
    //Define L1 & L2 caches
    string InFileName, OutFileName, address_str;
    char w_r = '\0'; int blocksize = 0;  int L1_size = 0;
    int L1_assoc = 0; int L2_size = 0; int L2_assoc = 0;

    //Parse File Input
    cout << "File Name: ";  cin >> InFileName;
    cout << "\nOutput File Name: "; cin >> OutFileName;
    ofstream outfile(OutFileName);
    ifstream infile(InFileName);
    if (!infile.is_open() || !outfile.is_open()) {
        cerr << "Input file and/or output file could not be opened." << endl;
        exit(1);
    }

    cout << "Enter Block Size:\n";
    cin >> blocksize;
    cout << "\nEnter L1 Cache Size:\n";
    cin >> L1_size;
    cout << "\nEnter L1 Associativity:\n";
    cin >> L1_assoc;
    cout << "\nEnter L2 Cache Size:\n";
    cin >> L2_size;
    cout << "\nEnter L2 Associativity:\n";
    cin >> L2_assoc;
    if (L2_size != 0) {
        L2_flag = true;
    }
    else {
        L2_flag = false;

    }

    //Initilize needed Cache and Read/Write Counters
    int L1_read_count = 0; int L1_read_miss_count = 0;
    int L1_write_count = 0; int L1_write_miss_count = 0;
    int L2_read_count = 0; int L2_read_miss_count = 0;
    int L2_write_count = 0; int L2_write_miss_count = 0;
    Cache L1_Cache(L1_size, L1_assoc, blocksize, true);
    Cache L2_Cache(L2_size, L2_assoc, blocksize, L2_flag);
    int debug = 0;

    while (infile >> w_r >> address_str) {
        bool missed_flag = false;
        int evicted_address = 0; //if there is nothing there its 0
        int thrown_out = 0; //the value that would get pushed up to l3 but l3 aint real so dont care
        int address = stoi(address_str, nullptr, 16);

        //compare linearly the tags at the set associated with the index
        if (w_r == 'r') {
            L1_read_count++;
            missed_flag = L1_Cache.read(address);
            // Read Miss at L1 Chace (L1 is the only Cache)
            if ((missed_flag != 0) && (L2_flag == false)) {
                L1_read_miss_count++;
                thrown_out = L1_Cache.write(address);
                L1_Cache.LRU_Update();
            }
            // Read Miss at L1 Cache (L2 Cache Exist)
            else if ((missed_flag != 0) && (L2_flag == true)) {
                L1_read_miss_count++;
                evicted_address = L1_Cache.write(address);

                if (evicted_address != 0) {
                    L2_read_count++;
                    missed_flag = L2_Cache.read(evicted_address);
                    if (missed_flag) {
                        //L2_read_miss_count++;
                        thrown_out = L2_Cache.write(evicted_address);
                        L2_Cache.LRU_Update();
                    }
                    //Evicted Address hits at L2
                    else {
                        L2_Cache.LRU_Update();
                    }

                }
                //Increment and read for the current address at L2 after parsing and eviction
                L2_read_count++;
                missed_flag = L2_Cache.read(address);
                //Read Miss at L2
                if (missed_flag) {
                    L2_read_miss_count++;
                    thrown_out = L2_Cache.write(address);
                    L2_Cache.LRU_Update();
                }
                //Else its a Hit update LRU 
                else {
                    L2_Cache.LRU_Update();
                }
                //Either way Update L1 LRU counter
                L1_Cache.LRU_Update();
            }
            //Hit Just Update LRU
            else {
                L1_Cache.LRU_Update();
            }
        }

        else if (w_r == 'w') {
            L1_write_count++;
            missed_flag = L1_Cache.read(address, true);
            if ((missed_flag != 0) && (L2_flag == false)) {
                L1_write_miss_count++;
                thrown_out = L1_Cache.write(address, true);
                L1_Cache.LRU_Update();
            }

            else if ((missed_flag != 0) && (L2_flag == false)) {
                L1_write_miss_count++;
                evicted_address = L1_Cache.write(address, true);

                if (evicted_address != 0) {
                    missed_flag = L2_Cache.read(evicted_address, false);
                    if (missed_flag) {
                        L2_write_miss_count++;
                        thrown_out = L2_Cache.write(evicted_address, false);
                        L2_Cache.LRU_Update();
                    }
                    else {
                        L2_Cache.LRU_Update();
                    }
                }
                L2_write_count++;
                missed_flag = L2_Cache.read(address, true);
                if (missed_flag) {
                    L2_write_miss_count++;
                    thrown_out = L2_Cache.write(address, false);
                    L2_Cache.LRU_Update();
                }
                else {
                    L2_Cache.update_stored();
                    L2_Cache.LRU_Update();
                }
                L1_Cache.LRU_Update();
            }

            else {
                //write data to L1 if hit and set dirty, as it may have changed
                L1_Cache.update_stored();
                L1_Cache.LRU_Update();
            }
        }

        else {
            cerr << "Write better tests, *insert Politically correct insult*." << endl;
            exit(1);
        }

    }
    double miss_rate = (static_cast<double>(L1_read_miss_count + L1_write_miss_count)
        / static_cast<double>(L1_read_count + L1_write_count));
    cout << miss_rate;
    //Print Stuff Out
    outfile << "===== Simulator configuration =====\n";
    outfile << "BLOCKSIZE:  " << std::dec << blocksize << '\n' << "L1_SIZE: " << L1_size << '\n';
    outfile << "L1_ASSOC:   " << L1_assoc << '\n' << "L1_PSEUDO-ASSOC-EN:  0\n";
    outfile << "L2_SIZE: " << L2_size << '\n' << "L2_ASSOC:   " << L2_assoc << '\n';
    outfile << "L2_PSEUDO-ASSOC-EN:  0\n" << "trace_file: " << InFileName << '\n';
    outfile << "===== L1 contents =====" << '\n';
    L1_Cache.spit_out_data(outfile);
    if (L2_flag) {
        outfile << '\n' << "===== L2 contents =====" << '\n';

        L2_Cache.spit_out_data(outfile);
    }
    outfile << "===== Simulation results (raw) =====\n";
    outfile << "a. number of L1 reads: " << std::dec << L1_read_count << '\n';
    outfile << "b. number of L1 read misses: " << L1_read_miss_count << '\n';
    outfile << "c. number of L1 writes: " << L1_write_count << '\n';
    outfile << "d. number of L1 write misses: " << L1_write_miss_count << '\n';
    outfile << "e. L1 miss rate: " << miss_rate << '\n';

    infile.close();
    outfile.close();
    return;
}
