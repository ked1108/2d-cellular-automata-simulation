//
// Created by kedar on 6/25/2024.
//

#include <iostream>
#include <utility>
#include <fstream>
#include "cellular_automata.h"
#include "cell.h"

cellular_automata::cellular_automata(int rule, int size) {
    this->rule = rule;
    set_neighbours();
    this->size = size;
    grid.assign(size*size, cell(true));
}

cellular_automata::cellular_automata(int rule, int size, std::vector<cell> image) {
    this->rule = rule;
    set_neighbours();
    this->size = size;
    grid = std::move(image);
    // std::cout << "INITIAL STATE" << std::endl;
    // print();
    export_image("initial.csv");
}

void cellular_automata::evolutions(int n) {
    for (int i = 0; i < n; ++i) {
        std::cout << std::to_string(i+1) << " Iteration:" << std::endl;
        std::string filename = std::to_string(this->rule.to_ulong())+"_iteration_"+std::to_string(i+1)+".csv";
        step(filename);
    }
}

int cellular_automata::get_size() const {
    return this->size;
}


void cellular_automata::step() {
    increase_size();
    std::vector<cell> temp = grid;

    for (int i = 0; i < size; ++i) {
        for (int j = 0; j < size; ++j) {
            int pos = get_pos(j, i);
            cell state(false);
            for (int y = -1, ct = 0; y < 2; y++) {
                for (int x = -1; x < 2; x++, ct++) {
                    if(neighbours[ct] && is_inbounds(j+x, i+y)){
                        int neighbour_pos = get_pos(j+x, i+y);
                        state = state ^ grid[neighbour_pos];
                    }
                }
            }
            temp[pos] = state;
        }
    }

    grid = std::move(temp);
}

void cellular_automata::step(const std::string& filename) {
    increase_size();
    std::vector<cell> temp = grid;

    for (int i = 0; i < size; ++i) {
        for (int j = 0; j < size; ++j) {
            int pos = get_pos(j, i);
            cell state(false);
            for (int y = -1, ct = 0; y < 2; y++) {
                for (int x = -1; x < 2; x++, ct++) {
                    if(neighbours[ct] && is_inbounds(j+x, i+y)){
                        int neighbour_pos = get_pos(j+x, i+y);
                        state = state ^ grid[neighbour_pos];
                    }
                }
            }
            temp[pos] = state;
        }
    }

    grid = std::move(temp);
    export_image(filename);
}

std::vector<cell> cellular_automata::get_grid() {
    return this->grid;
}


void cellular_automata::print() {
    for (int i = 0; i < size; ++i) {
        for (int j = 0; j < size; ++j) {
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


    for (int i = 0; i < size; ++i) {
        for (int j = 0; j < size; ++j) {
            if (grid[get_pos(j, i)].state){
                outFile << grid[get_pos(j, i)].to_string();
            }
            else
                outFile << " ";
            if(j < size -1) outFile << ", ";
        }
        outFile << "\n";
    }
}

void cellular_automata::set_neighbours() {
    neighbours[0] = this->rule[6];
    neighbours[1] = this->rule[7];
    neighbours[2] = this->rule[8];
    neighbours[3] = this->rule[5];
    neighbours[4] = this->rule[0];
    neighbours[5] = this->rule[1];
    neighbours[6] = this->rule[4];
    neighbours[7] = this->rule[3];
    neighbours[8] = this->rule[2];
}

bool cellular_automata::is_inbounds(int x, int y) const {
    return !(x < 0 || x >= size || y < 0 || y >= size);
}

void cellular_automata::increase_size() {
    std::vector<cell> temp = grid;
    int offset = 0;
    for (int i = 0; i < size; ++i) {
        int beg_pos = get_pos(0, i);
        temp.insert(temp.begin()+beg_pos+offset++, false);
        int end_pos = get_pos(size, i);
        temp.insert(temp.begin()+end_pos+offset++, false);
    }
    size += 2;
    std::vector<cell> temp2(size, cell(false));
    temp.insert(temp.begin(), temp2.begin(), temp2.end());
    temp.insert(temp.end(), temp2.begin(), temp2.end());
    grid = temp;
}

int cellular_automata::get_pos(int x, int y) const {
    return x+size*y;
}