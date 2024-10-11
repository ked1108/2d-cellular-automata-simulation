//
// Created by kedar on 6/25/2024.
//

#ifndef DUPLICATIN_IN_CA_CELLULAR_AUTOMATA_H
#define DUPLICATIN_IN_CA_CELLULAR_AUTOMATA_H

#include "cell.h"
#include <array>
#include <cstddef>
#include <deque>
#include <stack>
#include <vector>

class cellular_automata {
    Size size;
    int base;
    long long int r;
    std::array<int, 9> rule{};
    std::vector<std::pair<short, short>> neighbours;
    /*std::array<int, 9> neighbours;*/
    std::stack<std::vector<cell>> undo;

public:
    std::vector<cell> grid;

    cellular_automata(int rule, int x, int y, int base);
    cellular_automata(int rule, int x, int y, int base, const std::vector<cell>& image);
    void evolutions(int n);
    void step();
    void step(const std::string& filename);
    std::vector<cell> get_grid() const;
    Size get_size() const;
    int get_pos(int x, int y) const;
    void export_image(const std::string& filename);
    void undo_step();

private:
    void set_rule(int n);
    bool is_inbounds(int x, int y) const;
    void increase_size();
    void set_neighbours();
    void print();

};


#endif //DUPLICATIN_IN_CA_CELLULAR_AUTOMATA_H
