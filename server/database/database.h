//
// Created by ozzadar on 2024-12-17.
//

#pragma once
#include <string>

namespace OZZ {

    class Database {
    public:
        Database();
        ~Database();
    private:
        std::string host;
        std::string port;
    };

} // OZZ