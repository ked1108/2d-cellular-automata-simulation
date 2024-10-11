//
// Created by kedar on 6/26/2024.
//

#ifndef DUPLICATIN_IN_CA_CELL_H
#define DUPLICATIN_IN_CA_CELL_H


#include <map>
#include <vector>
#include <string>

typedef struct {
    int x;
    int y;
} Size;


class cell {
public:
    int base;
    std::map<std::string, int> pixel;
    int state;

    cell(int state, int base, std::string pixel);
    std::string to_string();
    cell operator^(cell& obj) const;
    cell(int state, int base);
};


#endif //DUPLICATIN_IN_CA_CELL_H
