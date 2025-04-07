//
// Created by ozzadar on 2025-04-06.
//

#pragma once
#include <filesystem>
#include <unordered_map>
#include <freetype/freetype.h>
#include <glm/glm.hpp>
#include <set>
#include <tuple>

namespace OZZ {
	class Image;

	struct Character {
		std::unique_ptr<Image> Texture;
		glm::ivec2 Size; // Size of glyph
		glm::ivec2 Bearing; // Offset from baseline to left/top of glyph
		uint32_t Advance; // Offset to advance to next glyph
	};

	class FontLoader {
	using path = std::filesystem::path;
	public:
		FontLoader();
		~FontLoader() = default;

		// Load a font from a file
		std::unordered_map<char, Character*> GetString(const path& fontPath, const std::string& str, uint16_t fontSize);
	private:
		bool loadFont(const path& fontPath);

		bool isCharacterLoaded(const path& fontPath, uint16_t fontSize, char character);
	private:
		FT_Library library;

		std::unordered_map<path, FT_Face> faces;

		// The map is a pair of (char, size_t) to allow for different sizes of the same character
		// path, fontSize, character, = Character
		std::unordered_map<path, std::unordered_map<uint16_t, std::unordered_map<char, std::unique_ptr<Character>>>> characters;
	};

	inline bool FontLoader::isCharacterLoaded(const path &fontPath, uint16_t fontSize, char character) {
		const auto fontIt = characters.find(fontPath);
		if (fontIt == characters.end()) {
			return false;
		}

		const auto sizeIt = fontIt->second.find(fontSize);
		if (sizeIt == fontIt->second.end()) {
			return false;
		}

		const auto charIt = sizeIt->second.find(character);
		if (charIt == sizeIt->second.end()) {
			return false;
		}

		return true;
	}
} // OZZ
