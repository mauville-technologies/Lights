//
// Created by paulm on 5/14/25.
//

#include <iostream>
#include <fstream>
#include <sstream>

#include "generator.h"

int main(int argc, char *argv[]) {
    // Print the command line arguments
    std::cout << "Command line arguments:" << std::endl;
    for (int i = 0; i < argc; ++i) {
        std::cout << argv[i] << std::endl;
    }

    auto pathToDefinition = argv[1];
    // load the file into a string
    std::ifstream defFile(pathToDefinition);
    std::stringstream defStream;
    defStream << defFile.rdbuf();
    defFile.close();

    auto generated = OZZ::typegen::Generate(defStream);

    auto pathToNewFile = argv[2];
    auto file = std::ofstream(pathToNewFile);
    if (file.is_open()) {
        // write to file
        file << generated.rdbuf();
        file.close();
    } else {
        std::cerr << "Failed to open file: " << pathToNewFile << std::endl;
        return 1;
    }

    return 0;
}
