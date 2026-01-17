//
// Created by ozzadar on 2025-08-12.
//

#include <lights/library/layers/clay/clay_ui_layer.h>
#include <lights/library/layers/clay/clay_utils.h>
#include <lights/rendering/shapes.h>

#include <glm/gtc/matrix_transform.hpp>
#include <span>

#include "lights/text/font_loader.h"
#include "spdlog/spdlog.h"

ClayUILayer::ClayUILayer(OZZ::InputSubsystem* inInput) : inputSubsystem(inInput) {}

ClayUILayer::~ClayUILayer() {
    shutdownClay();
}

auto layoutElement = Clay_LayoutConfig{.padding = {5}};

void ClayUILayer::Init() {
    SceneLayer::Init();
    renderTarget = std::make_unique<OZZ::RenderTarget>(OZZ::RenderTargetParams{.Type = OZZ::RenderTargetType::Texture});
    renders.insert(std::make_pair(GetRenderableName(), renderTarget.get()));
    reinitializeClay();
    LayerCamera.ViewMatrix = glm::mat4(1.f);
}

void ClayUILayer::PhysicsTick(float DeltaTime) {
    SceneLayer::PhysicsTick(DeltaTime);
}

void ClayUILayer::Tick(float DeltaTime) {
    SceneLayer::Tick(DeltaTime);

    const auto mousePosition = inputSubsystem->GetMousePosition();
    const auto mouseState = inputSubsystem->GetKeyState({OZZ::EDeviceID::Mouse, OZZ::EMouseButton::Left});

    static bool colorsGenerated = false;
    static uint16_t randomFontSizes[10] = {};
    static Clay_Color randomColors[10] = {};
    if (!colorsGenerated) {
        for (auto i = 0; i < 10; i++) {
            randomFontSizes[i] = 12 + (rand() % 20);
            randomColors[i] = Clay_Color{static_cast<float>(rand() % 256),
                                         static_cast<float>(rand() % 256),
                                         static_cast<float>(rand() % 256),
                                         255};
        }
        colorsGenerated = true;
    }
    if (bClayInitialized) {
        // TODO: handle input
        Clay_SetPointerState({mousePosition.x, mousePosition.y}, mouseState == OZZ::EKeyState::KeyPressed);
        Clay_UpdateScrollContainers(true, {0, 0}, DeltaTime);

        Clay_BeginLayout();
        if (tickDefinitionFunction) {
            tickDefinitionFunction();
        }

        clayRenderCommandArray = Clay_EndLayout();
    }
}

void ClayUILayer::RenderTargetResized(const glm::ivec2 size) {
    screenSize = size;
    if (renderTarget) {
        renderTarget->Resize(size);
    }

    LayerCamera.ProjectionMatrix =
        glm::ortho(0.0f, static_cast<float>(screenSize.x), static_cast<float>(screenSize.y), 0.0f, -1.0f, 1.0f);
    if (bClayInitialized) {
        Clay_SetLayoutDimensions(
            Clay_Dimensions{.width = static_cast<float>(size.x), .height = static_cast<float>(size.y)});
        return;
    }

    reinitializeClay();
}

void ClayUILayer::RegisterFont(const uint16_t& fontId, const std::filesystem::path& fontPath) {
    fontRegistry[fontId] = fontPath;
}

void ClayUILayer::UnregisterFont(const uint16_t& fontId) {
    fontRegistry.erase(fontId);
}

void ClayUILayer::SetDebugPanelOpened(bool bOpened) {
    if (!bClayInitialized) {
        spdlog::warn("Clay not initialized, cannot set debug panel state");
        return;
    }
    Clay_SetDebugModeEnabled(bOpened);
}

bool ClayUILayer::IsDebugPanelOpened() const {
    if (!bClayInitialized) {
        spdlog::warn("Clay not initialized, cannot get debug panel state");
        return false;
    }
    return Clay_IsDebugModeEnabled();
}

void ClayUILayer::reinitializeClay() {
    shutdownClay();
    currentRenderCommand.clear();
    uiSceneObjects.clear();
    fontRegistry = {};
    fontLoader = std::make_unique<OZZ::FontLoader>();
    clayTotalMemorySize = Clay_MinMemorySize();
    clayArena = Clay_CreateArenaWithCapacityAndMemory(clayTotalMemorySize, malloc(clayTotalMemorySize));
    Clay_Initialize(clayArena,
                    Clay_Dimensions{static_cast<float>(screenSize.x), static_cast<float>(screenSize.y)},
                    {[](Clay_ErrorData error) {
                         // Handle error
                         spdlog::error("Clay Error: {}", error.errorText.chars);
                     },
                     this});
    Clay_SetDebugModeEnabled(false);
    Clay_SetMeasureTextFunction(
        [](Clay_StringSlice text, Clay_TextElementConfig* config, void* userData) -> Clay_Dimensions {
            auto* ClayUILayerInstance = static_cast<ClayUILayer*>(userData);
            if (!ClayUILayerInstance->fontLoader) {
                spdlog::error("Font loader not initialized.", text.chars);
                return Clay_Dimensions{0, 0};
            }

            if (!ClayUILayerInstance->fontRegistry.contains(config->fontId)) {
                spdlog::error("Font not found {}", config->fontId);
                return Clay_Dimensions{0, 0};
            }

            const auto fontPath = ClayUILayerInstance->fontRegistry[config->fontId];
            const auto font = ClayUILayerInstance->fontLoader->GetFontSet(fontPath, config->fontSize);
            if (!font) {
                spdlog::error("Font not loaded: {} with size {}", fontPath.string(), config->fontSize);
                return Clay_Dimensions{0, 0};
            }

            auto measuredSize = font->MeasureText(std::string(text.chars, text.length));

            return Clay_Dimensions{.width = static_cast<float>(measuredSize.x),
                                   // <- this will only work for monospace fonts, see the
                                   // renderers/ directory for more advanced text measurement
                                   .height = static_cast<float>(measuredSize.y)};
        },
        this);

    buildShaders();
    bClayInitialized = true;
}

void ClayUILayer::buildShaders() {
    uiShader = std::make_shared<OZZ::Shader>(VertexShader, FragmentShader, true);
    textShader = std::make_shared<OZZ::Shader>(VertexShader, FontFragmentShader, true);

    const auto emptyImage = std::make_shared<OZZ::Image>();
    emptyImage->FillColor(glm::vec4(0.f, 0.f, 0.f, 0.f), glm::vec2(1.f, 1.f));
    const auto emptyTexture = std::make_shared<OZZ::Texture>();
    emptyTexture->UploadData(emptyImage.get());
    uiImages["empty"] = emptyTexture;
}

void ClayUILayer::shutdownClay() {
    if (bClayInitialized) {
        fontRegistry.clear();
        uiShader.reset();
        textShader.reset();
        uiImages.clear();
        free(clayArena.memory);
        clayArena = {};
        bClayInitialized = false;
    }
}

void ClayUILayer::refreshSceneObject(const uint32_t& id, const Clay_RenderCommand& command, const ScissorDef& scissor) {
    bool bBuildSceneObject = true;
    if (currentRenderCommand.contains(id)) {
        bBuildSceneObject =
            isRenderCommandChanged(command, currentRenderCommand[id]) ||
            scissor.bHasScissor != uiSceneObjects.at(id).Mat->GetSettings().bHasScissor ||
            (scissor.bHasScissor &&
             uiSceneObjects.at(id).Mat->GetSettings().Scissor !=
                 glm::vec4{scissor.ScissorBox.x,
                           static_cast<float>(screenSize.y) - (scissor.ScissorBox.y + scissor.ScissorBox.height),
                           scissor.ScissorBox.width,
                           scissor.ScissorBox.height});
    }

    if (command.commandType == CLAY_RENDER_COMMAND_TYPE_SCISSOR_START ||
        command.commandType == CLAY_RENDER_COMMAND_TYPE_SCISSOR_END) {
        return;
    }

    if (bBuildSceneObject) {
        buildSceneObject(id, command, scissor);
    }
}

void ClayUILayer::buildSceneObject(const uint32_t& id, const Clay_RenderCommand& command, const ScissorDef& scissor) {
    // cache the command
    spdlog::debug("Building scene object for command ID: {}", id);
    currentRenderCommand[id] = command;

    Clay_CornerRadius borderRadius{0, 0, 0, 0};
    Clay_Color backgroundColor{0, 0, 0, 0};
    Clay_Color borderColor{0, 0, 0, 0};
    Clay_BorderWidth borderWidth{0, 0, 0, 0};

    glm::vec3 scale = {command.boundingBox.width, command.boundingBox.height, 1.f};
    glm::vec3 translation = {command.boundingBox.x, command.boundingBox.y, 0.f};
    std::vector<OZZ::Vertex> vertices;
    std::vector<uint32_t> indices;
    std::shared_ptr<OZZ::Texture> texture = uiImages["empty"];

    const auto material = std::make_shared<OZZ::Material>();
    material->SetShader(uiShader);
    material->GetSettings().bHasScissor = scissor.bHasScissor;
    material->GetSettings().Scissor =
        glm::vec4{scissor.ScissorBox.x,
                  static_cast<float>(screenSize.y) - (scissor.ScissorBox.y + scissor.ScissorBox.height),
                  scissor.ScissorBox.width,
                  scissor.ScissorBox.height};

    // build the vertex data
    switch (command.commandType) {
        case CLAY_RENDER_COMMAND_TYPE_RECTANGLE: {
            borderRadius = command.renderData.rectangle.cornerRadius;
            backgroundColor = command.renderData.rectangle.backgroundColor;
            generateSquareMesh(vertices, indices);
            break;
        }
        case CLAY_RENDER_COMMAND_TYPE_BORDER: {
            borderRadius = command.renderData.border.cornerRadius;
            borderColor = command.renderData.border.color;
            borderWidth = command.renderData.border.width;
            generateSquareMesh(vertices, indices);
            break;
        }
        case CLAY_RENDER_COMMAND_TYPE_IMAGE: {
            texture = UnwrapUITexture(command.renderData.image.imageData);
            borderRadius = command.renderData.image.cornerRadius;
            generateSquareMesh(vertices, indices);
            break;
        }
        case CLAY_RENDER_COMMAND_TYPE_TEXT: {
            scale = {1.f, 1.f, 1.f};
            translation = {translation.x, translation.y, 0.f};
            material->SetShader(textShader);
            auto fontPath = fontRegistry[command.renderData.text.fontId];
            auto fontSize = command.renderData.text.fontSize;
            auto* FontSet = fontLoader->GetFontSet(fontPath, fontSize);
            auto fontKey = fontPath.stem().string() + "_" + std::to_string(fontSize);
            if (!uiImages.contains(fontKey)) {
                auto newTexture = std::make_shared<OZZ::Texture>();
                newTexture->UploadData(FontSet->Texture.get());
                uiImages[fontKey] = newTexture;
            }
            texture = uiImages[fontKey];
            const auto magenta = glm::vec4{1.f, 0.f, 1.f, 1.f};
            const auto yellow = glm::vec4{1.f, 1.f, 0.f, 1.f};
            backgroundColor = {.r = magenta.r * 255, .g = magenta.g * 255, .b = magenta.b * 255, .a = 0.f};

            borderColor = {.r = command.renderData.text.textColor.r,
                           .g = command.renderData.text.textColor.g,
                           .b = command.renderData.text.textColor.b,
                           .a = command.renderData.text.textColor.a};
            // generateSquareMesh(vertices, indices, {command.boundingBox.width,
            // command.boundingBox.height});
            generateTextMesh(std::string(command.renderData.text.stringContents.chars,
                                         command.renderData.text.stringContents.length),
                             FontSet,
                             vertices,
                             indices,
                             {
                                 command.boundingBox.width,
                                 command.boundingBox.height,
                             });
            break;
        }
        default: {
            spdlog::info("Unknown render command type: {}", static_cast<int>(command.commandType));
            break;
        }
    }
    material->AddUniformSetting({.Name = "backgroundColor",
                                 .Value = glm::vec4{backgroundColor.r / 255.f,
                                                    backgroundColor.g / 255.f,
                                                    backgroundColor.b / 255.f,
                                                    backgroundColor.a / 255.f}});

    material->AddUniformSetting(
        {.Name = "borderColor",
         .Value =
             glm::vec4{borderColor.r / 255.f, borderColor.g / 255.f, borderColor.b / 255.f, borderColor.a / 255.f}});

    material->AddUniformSetting({.Name = "borderWidth",
                                 .Value = glm::vec4{borderWidth.left / command.boundingBox.width,
                                                    borderWidth.right / command.boundingBox.width,
                                                    borderWidth.top / command.boundingBox.height,
                                                    borderWidth.bottom / command.boundingBox.height}});

    material->AddUniformSetting({.Name = "borderRadiusX",
                                 .Value = glm::vec4{borderRadius.topLeft / command.boundingBox.width,
                                                    borderRadius.topRight / command.boundingBox.width,
                                                    borderRadius.bottomRight / command.boundingBox.width,
                                                    borderRadius.bottomLeft / command.boundingBox.width}});

    material->AddUniformSetting({.Name = "borderRadiusY",
                                 .Value = glm::vec4{borderRadius.topLeft / command.boundingBox.height,
                                                    borderRadius.topRight / command.boundingBox.height,
                                                    borderRadius.bottomRight / command.boundingBox.height,
                                                    borderRadius.bottomLeft / command.boundingBox.height}});

    material->AddTextureMapping({.SlotName = "image", .SlotNumber = GL_TEXTURE0, .TextureResource = texture});

    const auto mesh = std::make_shared<OZZ::IndexVertexBuffer>();
    mesh->UploadData(vertices, indices);

    glm::mat4 transform{1.f};
    transform = glm::translate(transform, translation);
    transform = glm::scale(transform, scale);
    uiSceneObjects[id] = OZZ::scene::SceneObject{.Transform = transform, .Mesh = mesh, .Mat = material};
}

std::vector<OZZ::scene::SceneObject> ClayUILayer::getSceneObjects() {
    auto sceneObjects = std::vector<OZZ::scene::SceneObject>();

    ScissorDef scissor{};
    for (const auto command : std::span(clayRenderCommandArray.internalArray, clayRenderCommandArray.length)) {
        if (command.commandType == CLAY_RENDER_COMMAND_TYPE_SCISSOR_START) {
            scissor.bHasScissor = true;
            scissor.ScissorBox = command.boundingBox;
        }

        if (command.commandType == CLAY_RENDER_COMMAND_TYPE_SCISSOR_END) {
            scissor.bHasScissor = false;
        }

        refreshSceneObject(command.id, command, scissor);
        if (uiSceneObjects.contains(command.id))
            sceneObjects.emplace_back(uiSceneObjects[command.id]);
    }
    return sceneObjects;
}

bool ClayUILayer::isRenderCommandChanged(const Clay_RenderCommand& command, const Clay_RenderCommand& otherCommand) {
    return std::memcmp(&command, &otherCommand, sizeof(Clay_RenderCommand)) != 0;
}

void ClayUILayer::generateSquareMesh(std::vector<OZZ::Vertex>& outVertices,
                                     std::vector<uint32_t>& outIndices,
                                     glm::vec2 scale) {
    outIndices = std::vector<uint32_t>{0, 2, 1, 3, 2, 0};
    outVertices =
        std::vector<OZZ::Vertex>{{.position = {0.f, 0.f, 0.f}, .normal = {0.f, 0.f, 1.f}, .uv = {0.f, 0.f}},
                                 {.position = {scale.x, 0.f, 0.f}, .normal = {0.f, 0.f, 1.f}, .uv = {1.f, 0.f}},
                                 {.position = {scale.x, scale.y, 0.f}, .normal = {0.f, 0.f, 1.f}, .uv = {1.f, 1.f}},
                                 {.position = {0.f, scale.y, 0.f}, .normal = {0.f, 0.f, 1.f}, .uv = {0.f, 1.f}}};
}

void ClayUILayer::generateTextMesh(const std::string& text,
                                   OZZ::FontSet* fontSet,
                                   std::vector<OZZ::Vertex>& outVertices,
                                   std::vector<uint32_t>& outIndices,
                                   const glm::vec2& baseline) {
    // build the text object
    int nextCharacterX = 0;
    int startIndex = outVertices.size();

    for (const auto& character : text) {
        // get index of the character
        auto characterIndex = std::string(OZZ::FontLoader::CharacterSet).find(character);
        if (characterIndex == std::string::npos) {
            spdlog::error("Character {} not found in character set", character);
            continue;
        }

        auto [UV, Size, Bearing, Advance] = fontSet->Characters[character];

        float left = nextCharacterX + Bearing.x;
        float top = -Bearing.y + (baseline.y / 1.0);
        float bottom = top + Size.y;
        float right = left + Size.x;

        // first vertex
        auto bottomLeft = OZZ::Vertex{
            .position = {left, bottom, 0.f},
            .color = {1.f, 1.f, 1.f, 1.f},
            .uv = {UV.x, UV.y},
        };
        // second vertex
        auto bottomRight = OZZ::Vertex{
            .position = {right, bottom, 0.f},
            .color = {1.f, 1.f, 1.f, 1.f},
            .uv = {UV.z, UV.y},
        };
        // third vertex
        auto topLeft = OZZ::Vertex{
            .position = {left, top, 0.f},
            .color = {1.f, 1.f, 1.f, 1.f},
            .uv = {UV.x, UV.w},
        };
        // fourth vertex
        auto topRight = OZZ::Vertex{
            .position = {right, top, 0.f},
            .color = {1.f, 1.f, 1.f, 1.f},
            .uv = {UV.z, UV.w},
        };

        outVertices.push_back(topLeft);
        outVertices.push_back(bottomLeft);
        outVertices.push_back(bottomRight);
        outVertices.push_back(topRight);
        outIndices.push_back(startIndex + 0);
        outIndices.push_back(startIndex + 1);
        outIndices.push_back(startIndex + 2);
        outIndices.push_back(startIndex + 0);
        outIndices.push_back(startIndex + 2);
        outIndices.push_back(startIndex + 3);

        startIndex += 4;
        nextCharacterX += Advance.x >> 6;
    }
}

bool ClayUILayer::render() {
    if (!renderTarget) {
        spdlog::error("Renderable {} invalid -- no render target", GetRenderableName());
        return false;
    }
    renderTarget->Begin();
    glClearColor(0.f, 0.f, 0.f, 0.f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    for (auto& object : getSceneObjects()) {
        auto& [transform, objMesh, objMat] = object;
        objMat->Bind();
        objMat->GetShader()->SetMat4("view", GetCamera().ViewMatrix);
        objMat->GetShader()->SetMat4("projection", GetCamera().ProjectionMatrix);
        objMat->GetShader()->SetMat4("model", transform);
        objMesh->Bind();
        const auto drawMode = ToGLEnum(objMat->GetSettings().Mode);
        glDrawElements(drawMode, objMesh->GetIndexCount(), GL_UNSIGNED_INT, nullptr);
    }
    renderTarget->End();
    return true;
}
