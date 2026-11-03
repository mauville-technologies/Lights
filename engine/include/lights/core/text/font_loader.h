//
// Created by ozzadar on 2025-04-06.
//

#pragma once
#include <filesystem>
#include <unordered_map>
#include <freetype/freetype.h>
#include <glm/glm.hpp>
#include <set>
#include <string>
#include <string_view>
#include <utility>
#include <vector>
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
        std::unique_ptr<Image> Texture{nullptr};
        std::unordered_map<char32_t, CharacterDetails> Characters{};
        glm::vec2 CharacterSize{};
        std::string Name{};
        int Ascender{0};   // font-level ascender in pixels (above baseline, positive)
        int Descender{0};  // font-level descender in pixels (below baseline, negative)

        // Falls back to '?' for characters the set lacks; null only if neither exists.
        [[nodiscard]] const CharacterDetails* FindGlyph(char32_t codePoint) const;

        // Text is UTF-8.
        [[nodiscard]] glm::ivec2 MeasureText(const std::string& text) const;
    };

    // Lenient decode: invalid or truncated bytes become U+FFFD.
    std::vector<char32_t> DecodeUtf8(std::string_view text);

    class FontLoader {
        using path = std::filesystem::path;

    public:
        FontLoader();
        ~FontLoader() = default;

        FontSet* GetFontSet(const std::filesystem::path& fontPath, uint16_t fontSize);

        // Code point ranges baked into each atlas; only glyphs the font actually has are kept.
        // ASCII, Latin-1, Latin Extended-A/B, Greek, Cyrillic, Latin Extended Additional, common punctuation, euro.
        constexpr static std::pair<char32_t, char32_t> GlyphRanges[] = {
            {0x20, 0x7E}, {0xA0, 0xFF}, {0x100, 0x24F}, {0x370, 0x3FF},
            {0x400, 0x4FF}, {0x1E00, 0x1EFF}, {0x2010, 0x2027}, {0x20AC, 0x20AC},
        };

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
