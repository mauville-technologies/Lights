//
// Created by ozzadar on 2025-06-17.
//

#include "generator.h"

#include <iostream>

namespace OZZ::typegen {
    void trimString(std::string &str) {
        // Remove leading whitespace
        str.erase(str.begin(), std::find_if(str.begin(), str.end(), [](unsigned char ch) {
            return !std::isspace(ch);
        }));
        // Remove trailing whitespace
        str.erase(std::find_if(str.rbegin(), str.rend(), [](unsigned char ch) {
            return !std::isspace(ch);
        }).base(), str.end());
    }

    OZZ::typegen::internal::Node::Node(std::stringstream &inStream) {
        std::string firstLine;
        std::getline(inStream, firstLine);

        std::cout << "Processing first line: " << firstLine << std::endl;
        std::string token;
        std::stringstream firstLineStream(firstLine);
        while (firstLineStream >> token) {
            // Process the token
            if (token == "struct") {
                type = NodeType::Struct;
                std::cout << "Found struct definition." << std::endl;
                continue;
            }

            if (token == "enum") {
                type = NodeType::Enum;
                continue;
            }

            if (type == NodeType::Unknown) {
                std::cerr << "Error: Node type not specified in definition." << std::endl;
                return; // Invalid node definition, no type specified
            }

            if (token == "{") {
                break; // We reached the start of the node definition
            }

            // otherwise, trim the token just in case
            trimString(token);
            if (name.empty()) {
                name = token; // The first token is the name of the node
                break;
            } else {
                // If we already have a name, this is unexpected, handle it as needed
                throw std::runtime_error("Unexpected token after node name: " + token);
            }
        }

        std::cout << "Node name: " << name << ", type: " << (type == NodeType::Struct ? "Struct" : "Enum") << std::endl;

        // process line until we reach curly brace
        for (std::string line; std::getline(inStream, line);) {
            trimString(line);
            if (line.empty() || line.starts_with("//")) {
                continue; // skip empty lines and comments
            }
            if (line == "}") {
                break; // End of node definition
            }

            // the line should be a field definition
            std::stringstream lineStream(line);
            auto& field = fields.emplace_back();
            std::string fieldType;
            lineStream >> fieldType;
            if (fieldType.empty()) {
                std::cerr << "Error: line is empty" << line << std::endl;
                continue; // Skip this line if the field type is empty
            }

            if (type == NodeType::Enum) {
                // For enums, we expect the first token to be the enum value
                field.Name = fieldType;
                // there's nothing else to parse for enums, so we can skip the rest
                continue;
            }

            // first token is the type, rest is the name
            field.Type = parseFieldType(fieldType);
            if (field.Type == FieldType::GeneratedType) {
                field.CustomType = fieldType;
            }

            // second token is either `array` or the name of the field
            std::string secondToken;
            if (!(lineStream >> secondToken)) {
                std::cerr << "Error: Field name is missing in line: " << line << std::endl;
                continue; // Skip this line if the field name is missing
            }
            if (secondToken == "array") {
                field.IsArray = true;
                if (!(lineStream >> field.Name)) {
                    std::cerr << "Error: Field name is missing after 'array' in line: " << line << std::endl;
                    continue; // Skip this line if the field name is missing
                }
            } else {
                field.IsArray = false;
                field.Name = secondToken; // The rest of the line is the field name
            }
        }
    }

    OZZ::typegen::internal::Node::FieldType OZZ::typegen::internal::Node::parseFieldType(const std::string &typeStr) {
        if (typeStr == "int") return FieldType::Int;
        if (typeStr == "float") return FieldType::Float;
        if (typeStr == "double") return FieldType::Double;
        if (typeStr == "bool") return FieldType::Bool;
        if (typeStr == "string") return FieldType::String;

        return FieldType::GeneratedType; // Default case
    }

    OZZ::typegen::internal::File::File(std::stringstream &inStream) {
        // go through each line
        std::string line;
        std::streampos startPos = inStream.tellg();
        while (std::getline(inStream, line)) {
            std::stringstream lineStream(line);
            // Trim the line
            trimString(line);
            if (line.empty() || line.starts_with("//")) {
                startPos = inStream.tellg();
                continue; // skip empty lines and comments
            }

            inStream.seekg(startPos); // Reset the stream position to the start of the line
            nodes.emplace_back(inStream);
            //TODO: I should probably get a success flag from Node constructor or something to early exit
        }
    }

    std::stringstream Generate(std::stringstream &definition) {
        // Parse into internal::File structure
        const OZZ::typegen::internal::File file(definition);
        std::stringstream output;
        output << file;
        return output;
    }
}
