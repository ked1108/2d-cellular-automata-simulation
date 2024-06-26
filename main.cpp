#include <iostream>
#include "cellular_automata.h"
#include "cell.h"
int main() {
//    int n = 3, rule = 82, iterations = 4;
    int n = 0, rule = 0, iterations = 0;
    std::cout << "Enter The Size of the Image:";
    std::cin >> n;
    std::cout << "Enter The Rule No.:";
    std::cin >> rule;
    std::cout << "Enter The Number of Iterations:";
    std::cin >> iterations;
    std::vector<cell> image;
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            std::string pixel = "B"+std::to_string(j)+std::to_string(i);
            image.emplace_back(true, pixel);
        }
    }
    cellular_automata ca(rule, n, image);
    ca.evolutions(iterations);
    return 0;
}
