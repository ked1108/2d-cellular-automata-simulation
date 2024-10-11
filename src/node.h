//
// Created by kedar on 7/27/24.
//

#ifndef NODE_H
#define NODE_H

#include "cell.h"

#include <vector>

inline int get_pos(int x, int y, int size) {
    return  x+y*size;
}

void apply_rule(std::vector<std::vector<int>> image, int rule, Size size);
std::vector<int> find_incoming_nodes(int pos, const std::vector<std::vector<int>>& image);
std::vector<int> find_outgoing_nodes(int pos, const std::vector<std::vector<int>>& image);
void find_chain(int pos,  const Size& size, std::vector<std::string> chain, int from, const std::vector<std::vector<int>>& image);

#endif //NODE_H
