//
// Created by aaron on 2021-03-24.
//

#include <array>
#include <fstream>
#include <iostream>

int main() {
    // 1. read array from file
    std::ifstream infile("res.txt");

    std::array<std::array<int, 2>, 3> resources{};

    int line = 0, a, b;
    while (infile >> a >> b) {
        resources[line][0] = a;
        resources[line][1] = b;
        line++;
    }
}
