//
// Created by paulm on 5/14/25.
//

#include <iostream>
#include <fstream>

int main(int argc, char *argv[]) {
    // Print the command line arguments
    std::cout << "Command line arguments:" << std::endl;
    for (int i = 0; i < argc; ++i) {
        std::cout << argv[i] << std::endl;
    }

    const auto* TestClass = R"(
#pragma once
struct TestClass {
    int a;
    float b;
    double c;
};
)";

    auto pathToNewFile = argv[2];
    // write TestClass to the file
    auto file = std::ofstream(pathToNewFile);
    if (file.is_open()) {
        // write to file
        file << TestClass;
        file.close();
    } else {
        std::cerr << "Failed to open file: " << pathToNewFile << std::endl;
        return 1;
    }

    return 0;
}
