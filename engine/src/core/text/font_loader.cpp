//
// Created by ozzadar on 2025-04-06.
//

#include <lights/core/text/font_loader.h>
#include <freetype/freetype.h>
#include <lights/core/rendering/render_target.h>
#include <lights/core/util/image.h>
#include <spdlog/spdlog.h>

namespace OZZ {
    glm::ivec2 FontSet::MeasureText(const std::string& text) const {
        int width = 0;
        int maxTop = 0;
        int minBottom = 0;
        bool first = true;

        for (const auto character : text) {
            if (!Characters.contains(character)) {
                spdlog::warn("Character '{}' not found in font set", character);
                return {0, 0};
            }

            const auto& fontCharacter = Characters.at(character);
            width += fontCharacter.Advance.x >> 6;

            int top = static_cast<int>(fontCharacter.Bearing.y);
            int bottom = top - static_cast<int>(fontCharacter.Size.y);

            if (first) {
                maxTop = top;
                minBottom = bottom;
                first = false;
            }
            else {
                maxTop = std::max(maxTop, top);
                minBottom = std::min(minBottom, bottom);
            }
        }

        int height = maxTop - minBottom;
        return {width, height};
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

        auto characterImages = std::vector<std::unique_ptr<Image>>();
        auto characterDetails = std::unordered_map<char, CharacterDetails>();
        for (auto character : std::string(CharacterSet)) {
            // load the character
            if (FT_Load_Char(face, character, FT_LOAD_RENDER)) {
                spdlog::error("Could not load character: {}", character);
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
        for (auto character : std::string(CharacterSet)) {
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
