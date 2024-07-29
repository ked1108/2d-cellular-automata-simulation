//
// Created by kedar on 7/27/24.
//
#include "node.h"
#include "cell.h"

#include <algorithm>
#include <bitset>
#include <set>
#include <utility>
#include <vector>


//TODO Create find_chain
//TODO Create find_max
//TODO Create find_k
//TODO Create c2rules


std::vector<std::pair<int, int>> set_neighbourhood(std::bitset<9> rule_l) {
    std::vector<int> temp;
    std::vector<std::pair<int, int>> neighbours;

    temp[0] = rule_l[6];
    temp[1] = rule_l[7];
    temp[2] = rule_l[8];
    temp[3] = rule_l[5];
    temp[4] = rule_l[0];
    temp[5] = rule_l[1];
    temp[6] = rule_l[4];
    temp[7] = rule_l[3];
    temp[8] = rule_l[2];

    for(int i = -1, ct = 0; i < 2; i++) {
        for(int j = -1; j < 2; j++, ct++) {
            if(temp[ct] == 1) {
                neighbours.push_back({j, i});
            }
        }
    }


    return neighbours;
}

bool check_in_bounds(int x, int y, Size size) {
    return x >= 0 and x < size.x and y >= 0 and y < size.y;
}

void apply_rule(std::vector<Node> image, int rule, Size size) {
    std::bitset<9> rule_l = rule;
    std::vector neighbours = set_neighbourhood(rule_l);
    for (int i = 0; i < size.y; i++) {
        for(int j = 0; j < size.x; j++) {
            Node* cell = &image[get_pos(j, i, size.x)];
            for(auto p: neighbours) {
                auto[x, y] = p;
                if(check_in_bounds(j+x, i+y, size)) {
                    if(cell->first == nullptr) {
                        cell->first = &image[get_pos(j+x, j+y, size.x)];
                    } else if (cell->second == nullptr){
                        cell->second = &image[get_pos(j+x, j+y, size.x)];
                    }
                }
            }
        }
    }
}


std::vector<std::vector<bool>> create_adjacency_matrix(const std::vector<Node>& image, const Size& size) {
    std::vector adjacency_matrix(size.y, std::vector(size.x, false));
    for(int i = 0; i < size.x*size.y; i++) {
        if(image[i].first != nullptr)
            adjacency_matrix[i][get_pos(image[i].first->x, image[i].first->y, size.x)] = true;
        if(image[i].second != nullptr) 
          adjacency_matrix[i][get_pos(image[i].second->x, image[i].second->y, size.x)] = true; 
    } 
    return adjacency_matrix;
}



std::vector<Node> find_incoming_nodes(int pos, const std::vector<Node>& image, const std::vector<std::vector<bool>>& adjacency_matrix) {
    std::vector<Node> nodes;
    for (int i = 0 ; i < image.size(); i++ ) {
        if(adjacency_matrix[i][pos]){
            nodes.push_back(image[i]);
        }
    }

    return nodes;
}


std::vector<Node> find_outgoing_nodes(int pos, const std::vector<Node>& image, const std::vector<std::vector<int>>& adjacency_matrix) {
    std::vector<Node> nodes;
    for (int i = 0 ; i < image.size(); i++ ) {
        if(adjacency_matrix[pos][i] == 1){
            nodes.push_back(image[i]);
        }
    }

    return nodes;
}

void find_chain(int pos,  const Size& size, std::vector<Node> chain, Node from, const std::vector<Node>& image, const std::vector<std::vector<int>>& adjacency_matrix) {
    chain.push_back(image[pos]);
    std::vector incoming = find_incoming_nodes(pos, image, adjacency_matrix);
    auto it = std::find(incoming.begin(), incoming.end(), from);
    incoming.erase(it);

    Node node = incoming[0];
    std::vector outgoing = find_outgoing_nodes(get_pos(node.x, node.y, size.x), image, adjacency_matrix);
    
}



