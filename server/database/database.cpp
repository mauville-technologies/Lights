//
// Created by ozzadar on 2024-12-17.
//

#include "database.h"
#include "lights/models/user.h"

#include "mongocxx/exception/exception.hpp"
#include "bsoncxx/json.hpp"
#include "spdlog/spdlog.h"
#include "lights/util/crypto.h"
#include <cryptopp/cryptlib.h>

namespace OZZ {
    using bsoncxx::builder::basic::kvp;
    using bsoncxx::builder::basic::make_document;

    Database::Database() {
        mongocxx::uri uri(CONNECTION_STRING);
        client = mongocxx::client(uri);

        try
        {
            // Ping the server to verify that the connection works
            auto admin = client["admin"];
            auto command = make_document(kvp("ping", 1));
            auto result = admin.run_command(command.view());
            std::cout << bsoncxx::to_json(result) << "\n";
            std::cout << "Pinged your deployment. You successfully connected to MongoDB!\n";

            // Use the main database
            mainDB = client[DATABASE_NAME];

            migrate();
        }
        catch (const mongocxx::exception &e)
        {
            std::cerr << "An exception occurred: " << e.what() << "\n";
        }
    }

    Database::~Database() {

    }

    void Database::migrate() {
        // create a migration table
        auto users = mainDB["users"];
        // create unique key email
        users.create_index(make_document(kvp("email", 1)), mongocxx::options::index().unique(true));

        // Create a default user
        // TODO: this should be a flag, also things shouldnt be hardcoded, also probably could use some salt + hash
        auto defaultUser = User {
            "ozzadar",
            "p.a.mauviel@gmail.com",
            "password"
        };

        // get all current users
        if (!FindUser(defaultUser.Email).has_value()) {
            spdlog::info("Creating default user");

            // Create default user
            CreateUser(defaultUser);
        }
    }

    std::optional<User> Database::FindUser(std::string email) {
        auto users = mainDB["users"];
        auto result = users.find_one(make_document(kvp("email", email)));

        if (!result) {
            return {};
        }

        User user{};
        result->get<User>(user);
        return {user};
    }

    bool Database::CreateUser(User& user) {
        auto users = mainDB["users"];
        // hash the password

        user.Salt = OZZ::GenerateSalt(32);
        user.Password = OZZ::HashPassword(user.Salt + user.Password);
        auto userDoc = bsoncxx::document::value(user);

        try {
            if (!users.insert_one(userDoc.view())) {
                spdlog::error("Failed to insert default user");
                return false;
            }
        } catch (const mongocxx::exception &e) {
            spdlog::error("An exception occurred: {}", e.what());
            return false;
        }
        return true;
    }

    std::optional<User> Database::LoginUser(const std::string& email, const std::string& password) {
        auto User = FindUser(email);
        if (!User.has_value()) {
            return std::nullopt;
        }

        // hash the password
        return OZZ::VerifyPassword(User->Salt + password, User->Password) ? User : std::nullopt;
    }

} // OZZ