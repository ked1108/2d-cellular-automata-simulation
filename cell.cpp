//
// Created by kedar on 6/26/2024.
//

#include "cell.h"

#include <iostream>
#include <utility>
#include <algorithm>
#include <sstream>
#include <iterator>
#include <numeric>

bool is_leading_utf8_byte(char c) {
    auto const first_bit_set = (c & 0x80) != 0;
    auto const second_bit_set = (c & 0X40) != 0;
    return !first_bit_set || second_bit_set;
}

void pop_utf8(std::string& x) {
    while (!is_leading_utf8_byte(x.back()))
        x.pop_back();
    x.pop_back();
}

cell::cell(bool state) {
    this->state = state;
    pixel = {};
}

cell::cell(bool state, std::string pixel) {
    this->state = state;
    this->pixel = {{std::move(pixel), 1}};
}

cell cell::operator^(cell &obj) const {
    cell result(false);
    result.pixel = this->pixel;
    result.pixel = std::accumulate(obj.pixel.begin(), obj.pixel.end(), result.pixel, [](std::map<std::string, uint> m, std::pair<const std::string, uint> &p) {
        return (m[p.first] += p.second, m);
    });

    result.state = this->state ^ obj.state;

    return result;
}

std::string cell::to_string() {
    if(this->pixel.empty()) return "";
    std::string output;
    for(auto const& [key, val] : this->pixel) {
        output += std::to_string(val) + key + "⊕";
    }
    pop_utf8(output);
    return output;
}