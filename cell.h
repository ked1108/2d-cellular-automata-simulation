//
// Created by kedar on 6/26/2024.
//

#ifndef DUPLICATIN_IN_CA_CELL_H
#define DUPLICATIN_IN_CA_CELL_H


#include <vector>
#include <string>

class cell {
    std::vector<std::string> pixel;
public:
    bool state;
    cell(bool state, std::string pixel);
    std::string to_string();
    cell operator^(cell& obj);
    cell(bool state);
};


#endif //DUPLICATIN_IN_CA_CELL_H