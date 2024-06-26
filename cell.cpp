//
// Created by kedar on 6/26/2024.
//

#include "cell.h"

#include <iostream>
#include <utility>
#include <algorithm>
#include <sstream>
#include <iterator>

cell::cell(bool state) {
    this->state = state;
    pixel = {};
}

cell::cell(bool state, std::string pixel) {
    this->state = state;
    this->pixel = {std::move(pixel)};
}

cell cell::operator^(cell &obj) {
    cell result(false);
    result.pixel = std::vector<std::string>(obj.pixel.size()+this->pixel.size(), "");
    std::sort(this->pixel.begin(), this->pixel.end());
    std::sort(obj.pixel.begin(), obj.pixel.end());

    std::set_symmetric_difference(this->pixel.begin(), this->pixel.end(),
                                       obj.pixel.begin(), obj.pixel.end(),
                                       result.pixel.begin());

    result.pixel.erase(std::remove(result.pixel.begin(), result.pixel.end(), ""), result.pixel.end());

    result.state = this->state ^ obj.state;

    return result;
}

std::string cell::to_string() {
    if(this->pixel.empty()) return "";
    std::string output;
    for(int i = 0; i < this->pixel.size()-1; i++) {
        output += this->pixel[i] + "^";
    }
    output += this->pixel.back();
    return output;
}