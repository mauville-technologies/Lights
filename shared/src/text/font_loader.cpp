//
// Created by ozzadar on 2025-04-06.
//

#include <lights/text/font_loader.h>
#include <freetype/freetype.h>
#include <lights/util/image.h>
#include <spdlog/spdlog.h>

namespace OZZ {
	FontLoader::FontLoader() {
		spdlog::info("Loading font subsystem ...");
		if (FT_Init_FreeType(&library)) {
			spdlog::error("Could not initialize FreeType library");
			return;
		}
	}

	std::unordered_map<char, Character *> FontLoader::GetString(const path& fontPath, const std::string &str, uint16_t fontSize) {
		std::unordered_map<char, Character *> outCharacters;

		ensureCharactersLoaded(fontPath, fontSize);
		for (auto character : str) {
			outCharacters[character] = characters[fontPath][fontSize][character].get();
		}

		return outCharacters;
	}

	bool FontLoader::loadFont(const path &fontPath) {
		spdlog::info("Loading font from {}", fontPath.string());
		FT_Face face;
		// c_str to wide string
		if (FT_New_Face(library, fontPath.string().c_str(), 0, &face)) {
			spdlog::error("Could not load font: {}", fontPath.string());
			return false;
		}

		faces[fontPath] = face;
		return true;
	}

	inline bool FontLoader::ensureCharactersLoaded(const path &fontPath, uint16_t fontSize) {
		bool bCharactersLoaded = true;

		const auto fontIt = characters.find(fontPath);
		if (fontIt == characters.end()) {
			bCharactersLoaded = false;
		} else {
			const auto sizeIt = fontIt->second.find(fontSize);
			if (sizeIt == fontIt->second.end()) {
				bCharactersLoaded = false;
			}
		}

		if (bCharactersLoaded) {
			return true;
		}

		auto faceEntry = faces.find(fontPath);
		if (faceEntry == faces.end()) {
			if (!loadFont(fontPath)) {
				spdlog::error("Could not load font: {}", fontPath.string());
				return {};
			}
			// set faceEntry
			faceEntry = faces.find(fontPath);
		}

		auto face = faceEntry->second;
		if (FT_Set_Pixel_Sizes(face, 0, fontSize)) {
			spdlog::error("Could not set font size: {}", fontSize);
			return {};
		}

		for (auto character : std::string(CharacterSet)) {
			// load the character
			if (FT_Load_Char(face, character, FT_LOAD_RENDER)) {
				spdlog::error("Could not load character: {}", character);
				continue;
			}

			auto characterImage = std::make_unique<Image>(face->glyph->bitmap.buffer, face->glyph->bitmap.width, face->glyph->bitmap.rows, 1);
			auto newCharacter = std::make_unique<Character>(std::move(characterImage),
															glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
															glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
															face->glyph->advance.x);

			characters[fontPath][fontSize][character] = std::move(newCharacter);
		}
		return true;
	}
} // OZZ