//
// Created by kedar on 7/27/24.
//
#include "node.h"

#include <bitset>

std::vector<int> set_neighbourhood(std::bitset<9> rule_l) {
    std::vector<int> neighbours;
    neighbours[0] = rule_l[6];
    neighbours[1] = rule_l[7];
    neighbours[2] = rule_l[8];
    neighbours[3] = rule_l[5];
    neighbours[4] = rule_l[0];
    neighbours[5] = rule_l[1];
    neighbours[6] = rule_l[4];
    neighbours[7] = rule_l[3];
    neighbours[8] = rule_l[2];
    return neighbours;
}

bool check_in_bounds(int x, int y, Size size) {
    return x >= 0 and x < size.x and y >= 0 and y < size.y;
}

void apply_rule(std::vector<Node> image, int rule, Size size) {
    std::bitset<9> rule_l = rule;
    std::vector<int> neighbours = set_neighbourhood(rule_l);
    for (int i = 0; i < size.y; i++) {
        for(int j = 0; j < size.x; j++) {
            Node cell = image[get_pos(j, i, size.x)];
            for(int y = -1; y <= 1; y++) {
                for(int x = -1; x <= 1; x++) {
                    if(check_in_bounds(j+x, i+y, size) and neighbours[ct] == 1)
                }
            }
        }
    }
}


std::vector<std::vector<bool> > create_adjacency_matrix(const std::vector<Node>& image, const Size& size) {
    std::vector adjacency_matrix(size.y, std::vector(size.x, false));

    for(int i = 0; i < size.x*size.y; i++) {
        if(image[i].first != nullptr) adjacency_matrix[i][get_pos(image[i].first->x, image[i].first->y, size.x)] = true;
        if(image[i].second != nullptr) adjacency_matrix[i][get_pos(image[i].second->x, image[i].second->y, size.x)] = true;
    }

    return adjacency_matrix;
}

