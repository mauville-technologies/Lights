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
		glm::vec2 UV;
	};

	struct CharacterDetails {
		glm::vec4 UV;
		glm::ivec2 Size;
		glm::ivec2 Bearing;
		glm::ivec2 Advance;
	};

	struct FontSet {
		std::unique_ptr<Image> Texture { nullptr };
		std::unordered_map<char, CharacterDetails> Characters {};
	};

	class FontLoader {
	using path = std::filesystem::path;
	public:
		FontLoader();
		~FontLoader() = default;

		std::unordered_map<char, Character*> GetString(const path& fontPath, const std::string& str, uint16_t fontSize);
		std::unordered_map<char, Character*> GetCharacterSet(const path& fontPath, uint16_t fontSize);
		FontSet* GetFontSet(const std::filesystem::path& fontPath, uint16_t fontSize);

		constexpr static auto CharacterSet =
			"abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789!@#$%^&*()_+[]{}|;':\",.<>?`~ ";
	private:
		bool loadFont(const path& fontPath);
		bool ensureCharactersLoaded(const path& fontPath, uint16_t fontSize);
	private:
		FT_Library library;

		std::unordered_map<path, FT_Face> faces;

		// The font sets are textures that contain the characters in a single texture atlas
		// path, fontSize -> FontSet
		std::unordered_map<path, std::unordered_map<uint16_t, std::unique_ptr<FontSet>>> fontSets;

		// The map is a pair of (char, size_t) to allow for different sizes of the same character
		// path, fontSize, character, = Character
		std::unordered_map<path, std::unordered_map<uint16_t, std::unordered_map<char, std::unique_ptr<Character>>>> characters;
	};


} // OZZ
