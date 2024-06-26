//
// Created by kedar on 6/25/2024.
//

#ifndef DUPLICATIN_IN_CA_CELLULAR_AUTOMATA_H
#define DUPLICATIN_IN_CA_CELLULAR_AUTOMATA_H

#include "cell.h"
#include <bitset>
#include <vector>

class cellular_automata {
    int size;
    std::bitset<9> rule;
    std::bitset<9> neighbours;
    std::vector<cell> grid;
public:
    cellular_automata(int rule, int size);
    cellular_automata(int rule, int size ,std::vector<cell> image);
    void evolutions(int n);
    void step(const std::string& filename);

private:
    bool is_inbounds(int x, int y) const;
    void increase_size();
    int get_pos(int x, int y) const;
    void set_neighbours();
    void print();
    void export_image(const std::string& filename);
};


#endif //DUPLICATIN_IN_CA_CELLULAR_AUTOMATA_H
