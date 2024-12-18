//
// Created by ozzadar on 2024-12-17.
//

#pragma once
#include <string>
#include "mongocxx/instance.hpp"
#include "mongocxx/client.hpp"
#include "lights/models/user.h"

namespace OZZ {

    // TODO: This probably shouldnt be hardcoded, or local only. Gotta load settings from somewhere probably
    constexpr const char* CONNECTION_STRING = "mongodb://ozzadar:password@localhost:27017";
    constexpr const char* DATABASE_NAME = "lights";

    class Database {
    public:
        Database();
        ~Database();

    public:
        std::optional<User> FindUser(std::string email);
        bool CreateUser(User &user);
        std::optional<User> LoginUser(const std::string& email, const std::string& password);
        void LogoutUser(const std::string& email);
    private:
        void migrate();

    private:
        mongocxx::instance instance;
        mongocxx::client client;
        mongocxx::database mainDB;

    };

} // OZZ