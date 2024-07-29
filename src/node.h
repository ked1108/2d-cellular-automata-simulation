//
// Created by kedar on 7/27/24.
//

#ifndef NODE_H
#define NODE_H

#include "cell.h"

#include <vector>


typedef struct Node{
    cell* self;
    Node* first;
    Node* second;
    int x;
    int y;
} Node;

inline int get_pos(int x, int y, int size) {
    return  x+y*size;
}

std::vector<std::vector<bool>> create_adjacency_matrix(std::vector<Node> image, Size size);
void apply_rule(std::vector<Node> image, int rule, Size size);
std::vector<std::vector<bool>> create_adjacency_matrix(std::vector<Node> image, Size size);
std::vector<Node> find_incoming_nodes(int pos, const std::vector<Node>& image, const std::vector<std::vector<int>>& adjacency_matrix);
std::vector<Node> find_outgoing_nodes(int pos, const std::vector<Node>& image, const std::vector<std::vector<int>>& adjacency_matrix);
void find_chain(int pos,  const Size& size, std::vector<Node> chain, Node from, const std::vector<Node>& image, const std::vector<std::vector<int>>& adjacency_matrix);

#endif //NODE_H
