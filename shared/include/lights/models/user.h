//
// Created by ozzadar on 2024-12-17.
//

#pragma once
#include <string>

namespace OZZ {
    struct User {
        std::string Name;
        std::string Email;
        std::string Password;
        std::string Salt;
    };

    // provide from_bson function for serialization
    inline void from_bson(User& user, bsoncxx::document::view view) {
        user.Name = view["name"].get_string().value;
        user.Email = view["email"].get_string().value;
        user.Password = view["password"].get_string().value;
        user.Salt = view["salt"].get_string().value;
    }

    inline void to_bson(const User& user, bsoncxx::document::value& view) {
        using bsoncxx::builder::basic::kvp;
        view = bsoncxx::builder::basic::make_document(
                kvp("name", user.Name),
                kvp("email", user.Email),
                kvp("password", user.Password),
                kvp("salt", user.Salt)
        );
    }
}