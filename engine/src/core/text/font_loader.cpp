//
// Created by ozzadar on 2025-04-06.
//

#include <lights/core/text/font_loader.h>
#include <freetype/freetype.h>
#include <lights/core/rendering/render_target.h>
#include <lights/core/util/image.h>
#include <spdlog/spdlog.h>

namespace OZZ {
    std::vector<char32_t> DecodeUtf8(std::string_view text) {
        std::vector<char32_t> codePoints;
        codePoints.reserve(text.size());
        for (size_t i = 0; i < text.size();) {
            const auto lead = static_cast<unsigned char>(text[i]);
            const size_t length = lead < 0x80 ? 1 : (lead & 0xE0) == 0xC0 ? 2 : (lead & 0xF0) == 0xE0 ? 3 : (lead & 0xF8) == 0xF0 ? 4 : 0;
            char32_t codePoint = length == 1 ? lead : length == 2 ? (lead & 0x1F) : length == 3 ? (lead & 0x0F) : (lead & 0x07);
            bool bValid = length != 0 && i + length <= text.size();
            for (size_t k = 1; bValid && k < length; ++k) {
                const auto next = static_cast<unsigned char>(text[i + k]);
                bValid = (next & 0xC0) == 0x80;
                codePoint = (codePoint << 6) | (next & 0x3F);
            }
            codePoints.push_back(bValid ? codePoint : 0xFFFD);
            i += bValid ? length : 1;
        }
        return codePoints;
    }

    const CharacterDetails* FontSet::FindGlyph(const char32_t codePoint) const {
        if (const auto it = Characters.find(codePoint); it != Characters.end()) {
            return &it->second;
        }
        const auto fallback = Characters.find(U'?');
        return fallback != Characters.end() ? &fallback->second : nullptr;
    }

    glm::ivec2 FontSet::MeasureText(const std::string& text) const {
        int width = 0;
        for (const auto codePoint : DecodeUtf8(text)) {
            if (const auto* glyph = FindGlyph(codePoint)) {
                width += glyph->Advance.x >> 6;
            }
        }
        return {width, Ascender - Descender};
    }

    FontLoader::FontLoader() {
        spdlog::info("Loading font subsystem ...");
        if (FT_Init_FreeType(&library)) {
            spdlog::error("Could not initialize FreeType library");
            return;
        }
    }

    FontSet* FontLoader::GetFontSet(const std::filesystem::path& fontPath, uint16_t fontSize) {
        // look for existing entry
        if (const auto fontSetEntry = fontSets.find(fontPath); fontSetEntry != fontSets.end()) {
            if (auto sizeEntry = fontSetEntry->second.find(fontSize); sizeEntry != fontSetEntry->second.end()) {
                return sizeEntry->second.get();
            }
        }

        auto NewFontSet = std::make_unique<FontSet>();

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

        NewFontSet->Ascender = static_cast<int>(face->size->metrics.ascender >> 6);
        NewFontSet->Descender = static_cast<int>(face->size->metrics.descender >> 6);

        auto characterImages = std::vector<std::unique_ptr<Image>>();
        auto characterDetails = std::unordered_map<char32_t, CharacterDetails>();
        auto loadedOrder = std::vector<char32_t>(); // atlas cell order, for the UV pass below
        for (const auto& [first, last] : GlyphRanges) {
            for (char32_t character = first; character <= last; ++character) {
                if (character != U' ' && FT_Get_Char_Index(face, character) == 0) {
                    continue; // the font has no glyph for it
                }
                if (FT_Load_Char(face, character, FT_LOAD_RENDER)) {
                    spdlog::error("Could not load character U+{:04X}", static_cast<uint32_t>(character));
                    continue;
                }

                auto characterImage = std::make_unique<Image>(face->glyph->bitmap.buffer, face->glyph->bitmap.width, face->glyph->bitmap.rows,
                                                              1);
                // characterImage->FlipPixels(true, false);
                characterImages.emplace_back(std::move(characterImage));

                auto charDetails = CharacterDetails{
                    .Size = glm::vec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
                    .Bearing = glm::vec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
                    .Advance = glm::vec2(face->glyph->advance.x, face->glyph->advance.y)
                };
                characterDetails[character] = charDetails;
                loadedOrder.push_back(character);
            }
        }

        auto [gridSize, imageSize, texture] = Image::MergeImages(characterImages);
        NewFontSet->Texture = std::move(texture);
        NewFontSet->Texture->FlipPixels(true, false);
        NewFontSet->Characters = characterDetails;
        NewFontSet->CharacterSize = imageSize;
        NewFontSet->Name = fontPath.stem().string() + "_" + std::to_string(fontSize);
        fontSets[fontPath][fontSize] = std::move(NewFontSet);

        auto* currentFontSet = fontSets[fontPath][fontSize].get();
        // now let's set the UV coordinates
        glm::vec2 currentGridPosition = {0.f, 0.f};
        for (const auto character : loadedOrder) {
            auto& details = currentFontSet->Characters[character];
            if (currentGridPosition.x >= gridSize.x) {
                currentGridPosition.x = 0.f;
                currentGridPosition.y++;
            }
            if (currentGridPosition.y >= gridSize.y) {
                spdlog::error("Font texture is too small for character set");
                return {};
            }

            auto uvYPosition = gridSize.y - currentGridPosition.y;
            auto uvStart = glm::vec2(currentGridPosition.x * imageSize.x, uvYPosition * imageSize.y);
            glm::vec4 uv = {
                uvStart.x / static_cast<float>(currentFontSet->Texture->GetWidth()),
                (uvStart.y - details.Size.y) / static_cast<float>(currentFontSet->Texture->GetHeight()),
                (uvStart.x + details.Size.x) / static_cast<float>(currentFontSet->Texture->GetWidth()),
                uvStart.y / static_cast<float>(currentFontSet->Texture->GetHeight()),
            };

            details.UV = uv;
            currentGridPosition.x++;
        }
        return currentFontSet;
    }

    bool FontLoader::loadFont(const path& fontPath) {
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
} // OZZ
