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

		FontSet* GetFontSet(const std::filesystem::path& fontPath, uint16_t fontSize);

		constexpr static auto CharacterSet =
			"abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789!@#$%^&*()_+[]{}|;':\",.<>?`~ ";
	private:
		bool loadFont(const path& fontPath);
	private:
		FT_Library library;

		std::unordered_map<path, FT_Face> faces;

		// The font sets are textures that contain the characters in a single texture atlas
		// path, fontSize -> FontSet
		std::unordered_map<path, std::unordered_map<uint16_t, std::unique_ptr<FontSet>>> fontSets;
	};


} // OZZ
