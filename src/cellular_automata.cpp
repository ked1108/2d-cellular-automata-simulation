//
// Created by kedar on 6/25/2024.
//

#include <iostream>
#include <utility>
#include <fstream>
#include <omp.h>
#include "cellular_automata.h"
#include "cell.h"

cellular_automata::cellular_automata(int rule, int x, int y, int base) {
    this->base = base;
    this->r = rule;
    set_rule(rule);
    set_neighbours();
    this->size.x = x;
    this->size.y = y;
    grid.assign(x*y, cell(this->base, this->base));
    undo.push(grid);
}

cellular_automata::cellular_automata(int rule, int x, int y, int base, const std::vector<cell>& image) {
    this->base = base;
    this->r = rule;
    set_rule(rule);
    set_neighbours();
    this->size.x = x;
    this->size.y = y;
    grid = std::move(image);
    undo.push(grid);
    // std::cout << "INITIAL STATE" << std::endl;
    // print();
}

void cellular_automata::evolutions(int n) {
    for (int i = 0; i < n; ++i) {
        std::cout << std::to_string(i+1) << " Iteration:" << std::endl;
        std::string filename = std::to_string(r)+"_iteration_"+std::to_string(i+1)+".csv";
        step(filename);
    }
}

Size cellular_automata::get_size() const {
    return this->size;
}


void cellular_automata::step() {
    increase_size();
    std::vector<cell> temp = grid;

	#pragma omp parallel collapse(2)
    for (int i = 0; i < size.y; ++i) {
        for (int j = 0; j < size.x; ++j) {
            int pos = get_pos(j, i);
            cell state(0, base);
            for (auto p: neighbours) {
                auto[x, y] = p;
                if(is_inbounds(j+x, i+y)){
                    int neighbour_pos = get_pos(j+x, i+y);
                    state = state ^ grid[neighbour_pos];
                }
            }
            temp[pos] = state;
        }
    }

    grid = std::move(temp);
    undo.push(grid);
}

void cellular_automata::step(const std::string& filename) {
    increase_size();
    std::vector<cell> temp = grid;

    for (int i = 0; i < size.y; ++i) {
        for (int j = 0; j < size.x; ++j) {
            int pos = get_pos(j, i);
            cell state(0, base);
            for (auto p: neighbours) {
                auto[x, y] = p;
                if(is_inbounds(j+x, i+y)){
                    int neighbour_pos = get_pos(j+x, i+y);
                    state = state ^ grid[neighbour_pos];
                }
            }
            temp[pos] = state;
        }
    }

    grid = std::move(temp);
    export_image(filename);
    undo.push(grid);
}

std::vector<cell> cellular_automata::get_grid() const {
    return this->grid;
}

void cellular_automata::undo_step() {
    if(undo.size() > 1) {
        grid.clear();
        undo.pop();
        grid = undo.top();
        size.x -= 2;
        size.y -= 2;
    }
}


void cellular_automata::print() {
    for (int i = 0; i < size.y; ++i) {
        for (int j = 0; j < size.x; ++j) {
            if (grid[get_pos(j, i)].state)    std::cout << "██" << " ";
            else                              std::cout << "__" << " ";
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;
}

void cellular_automata::export_image(const std::string& filename) {
    std::ofstream outFile(filename);
    if (!outFile.is_open()) {
        std::cerr << "Could not open the file " << filename << " for writing." << std::endl;
        return;
    }

    for (int i = 0; i < size.y; ++i) {
        for (int j = 0; j < size.x; ++j) {

//            outFile << ((grid[get_pos(j, i)].state > 0) ? grid[get_pos(j, i)].to_string() : " " );
            outFile << grid[get_pos(j, i)].to_string();
            if(j < size.x -1) outFile << ", ";
        }
        outFile << "\n";
    }
}

void cellular_automata::set_rule(int n) {
    int k = n, i = 0;
    while(k!=0) {
        rule[i++] = (k % base + base) % base;
        k = k / base;
    }
}


void cellular_automata::set_neighbours() {
    int temp[9];
    temp[0] = this->rule[6];
    temp[1] = this->rule[7];
    temp[2] = this->rule[8];
    temp[3] = this->rule[5];
    temp[4] = this->rule[0];
    temp[5] = this->rule[1];
    temp[6] = this->rule[4];
    temp[7] = this->rule[3];
    temp[8] = this->rule[2];

    for(int i = -1, ct = 0; i < 2; i++) {
        for(int j = -1; j < 2; j++, ct++) {
            if(temp[ct] == 1) {
                neighbours.push_back({j, i});
            }
        }
    }
}

bool cellular_automata::is_inbounds(int x, int y) const {
    return !(x < 0 || x >= size.x || y < 0 || y >= size.y);
}

//TODO: Refactor this garbage ass code you wrote
void cellular_automata::increase_size() {
    std::vector<cell> temp = grid;
    int offset = 0;
    for (int i = 0; i < size.y; ++i) {
        const int beg_pos = get_pos(0, i);
        temp.insert(temp.begin()+beg_pos+offset++, cell(0, this->base));
        const int end_pos = get_pos(size.x, i);
        temp.insert(temp.begin()+end_pos+offset++, cell(0, this->base));
    }
    size.x += 2;
    size.y += 2;
    std::vector<cell> temp2(size.x, cell(0, base));
    temp.insert(temp.begin(), temp2.begin(), temp2.end());
    temp.insert(temp.end(), temp2.begin(), temp2.end());
    grid = temp;
}

int cellular_automata::get_pos(int x, int y) const {
    return x+size.x*y;
}
