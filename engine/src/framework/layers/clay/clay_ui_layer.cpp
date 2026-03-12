//
// Created by ozzadar on 2025-08-12.
//

#include <lights/core/rendering/shapes.h>
#include <lights/core/text/font_loader.h>
#include <lights/framework/layers/clay/clay_ui_layer.h>
#include <lights/framework/layers/clay/clay_utils.h>

#include <glm/gtc/matrix_transform.hpp>
#include <span>

#include "spdlog/spdlog.h"

ClayUILayer::ClayUILayer(OZZ::InputSubsystem* inInput)
    : inputSubsystem(inInput) {}

ClayUILayer::~ClayUILayer() {
    shutdownClay();
}

auto layoutElement = Clay_LayoutConfig{.padding = {5}};

void ClayUILayer::Init(OZZ::rendering::RHIDevice* inDevice) {
    SceneLayer::Init(inDevice);

    renderTarget = std::make_unique<OZZ::RenderTarget>(inDevice,
                                                       OZZ::RenderTargetParams{
                                                           .Type = OZZ::RenderTargetType::Texture,
                                                           .ClearColor = {1.f, 1.f, 0.f, 0.f},
                                                           .bHasDepth = false,
                                                       });
    renders.insert(std::make_pair(GetRenderableName(), renderTarget.get()));
    reinitializeClay();
    LayerCamera.ViewMatrix = glm::lookAt(glm::vec3{0, 0, 4}, {0, 0, 0}, {0, 1, 0});

    // create camera buffer
    CameraSettings cameraSettings{
        .View = LayerCamera.ViewMatrix,
        .Proj = LayerCamera.ProjectionMatrix,
    };

    cameraBuffer = device->CreateBuffer({
        .Size = sizeof(CameraSettings),
        .Usage = OZZ::rendering::BufferUsage::UniformBuffer,
        .Access = OZZ::rendering::BufferMemoryAccess::CpuToGpu,
    });
    device->UpdateBuffer(cameraBuffer, &cameraSettings, sizeof(CameraSettings), 0);
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
        glm::ortho(0.0f, static_cast<float>(screenSize.x), static_cast<float>(screenSize.y), 0.0f, -1000.0f, 100.0f);
    // update camera buffer
    CameraSettings cameraSettings{
        .View = LayerCamera.ViewMatrix,
        .Proj = LayerCamera.ProjectionMatrix,
    };
    device->UpdateBuffer(cameraBuffer, &cameraSettings, sizeof(CameraSettings), 0);

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
    auto context = Clay_Initialize(clayArena,
                                   Clay_Dimensions{static_cast<float>(screenSize.x), static_cast<float>(screenSize.y)},
                                   {[](Clay_ErrorData error) {
                                        // Handle error
                                        spdlog::error("Clay Error: {}", error.errorText.chars);
                                    },
                                    this});
    Clay_SetCurrentContext(context);
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
    uiShader = std::make_shared<OZZ::Shader>(device,
                                             OZZ::rendering::ShaderSourceParams{
                                                 .Vertex = VertexShader,
                                                 .Fragment = FragmentShader,
                                             });
    textShader = std::make_shared<OZZ::Shader>(device,
                                               OZZ::rendering::ShaderSourceParams{
                                                   .Vertex = VertexShader,
                                                   .Fragment = FontFragmentShader,
                                               });

    const auto emptyImage = std::make_shared<OZZ::Image>();
    emptyImage->FillColor(glm::vec4(0.f, 0.f, 0.f, 0.f), glm::vec2(1.f, 1.f));
    const auto emptyTexture =
        std::make_shared<OZZ::Texture>(device,
                                       OZZ::rendering::TextureDescriptor{
                                           .Width = static_cast<uint32_t>(emptyImage->GetWidth()),
                                           .Height = static_cast<uint32_t>(emptyImage->GetHeight()),
                                           .Format = OZZ::rendering::TextureFormat::RGBA8,
                                           .Usage = OZZ::rendering::TextureUsage::Sampled,
                                       });
    emptyTexture->UploadData(emptyImage.get());
    uiImages["empty"] = emptyTexture;
}

void ClayUILayer::shutdownClay() {
    if (bClayInitialized) {
        spdlog::info("Shutting down clay");
        fontRegistry.clear();
        uiShader.reset();
        textShader.reset();
        uiImages.clear();
        free(clayArena.memory);
        clayArena = {};
        Clay_SetCurrentContext({});
        bClayInitialized = false;
    }
}

void ClayUILayer::refreshSceneObject(const uint32_t& id, const Clay_RenderCommand& command, const ScissorDef& scissor) {
    bool bBuildSceneObject = true;
    if (currentRenderCommand.contains(id)) {
        bBuildSceneObject =
            isRenderCommandChanged(command, currentRenderCommand[id]) ||
            (scissor.bHasScissor && uiSceneObjects.at(id).Mat->GetSettings().Scissor != glm::ivec4{
                                                                                            scissor.ScissorBox.x,
                                                                                            scissor.ScissorBox.y,
                                                                                            scissor.ScissorBox.width,
                                                                                            scissor.ScissorBox.height,
                                                                                        });
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
    spdlog::trace("Building scene object for command ID: {}", id);
    currentRenderCommand[id] = command;

    Clay_CornerRadius borderRadius{0, 0, 0, 0};
    Clay_Color backgroundColor{0, 0, 0, 0};
    Clay_Color borderColor{0, 0, 0, 0};
    Clay_BorderWidth borderWidth{0, 0, 0, 0};

    glm::vec3 scale = {command.boundingBox.width, command.boundingBox.height, 1.f};
    glm::vec3 translation = {command.boundingBox.x, command.boundingBox.y, 0}; // ensure unique Z for proper layering
    std::vector<OZZ::Vertex> vertices;
    std::vector<uint32_t> indices;
    std::shared_ptr<OZZ::Texture> texture = uiImages["empty"];

    const auto material = std::make_shared<OZZ::Material>(device);
    material->SetShader(uiShader);
    material->GetSettings().bHasScissor = true;
    scissor.bHasScissor ? material->GetSettings().Scissor =
                              glm::vec4{
                                  scissor.ScissorBox.x,
                                  scissor.ScissorBox.y,
                                  scissor.ScissorBox.width,
                                  scissor.ScissorBox.height,
                              }
                        : material->GetSettings().Scissor = glm::vec4{
                              0,
                              0,
                              screenSize.x,
                              screenSize.y,
                          };

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
            // translation = {translation.x, translation.y, 0.f};
            material->SetShader(textShader);
            auto fontPath = fontRegistry[command.renderData.text.fontId];
            auto fontSize = command.renderData.text.fontSize;
            auto* FontSet = fontLoader->GetFontSet(fontPath, fontSize);
            auto fontKey = fontPath.stem().string() + "_" + std::to_string(fontSize);
            if (!uiImages.contains(fontKey)) {
                auto textureFormat = OZZ::rendering::TextureFormat::RGBA8;
                if (FontSet->Texture->GetChannels() == 1) {
                    textureFormat = OZZ::rendering::TextureFormat::R8;
                }
                auto newTexture =
                    std::make_shared<OZZ::Texture>(device,
                                                   OZZ::rendering::TextureDescriptor{
                                                       .Width = static_cast<uint32_t>(FontSet->Texture->GetWidth()),
                                                       .Height = static_cast<uint32_t>(FontSet->Texture->GetHeight()),
                                                       .Format = textureFormat,
                                                       .Usage = OZZ::rendering::TextureUsage::Sampled,
                                                   });
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

    auto uiSettings = UIComponentSettings{
        .BackgroundColor =
            {
                backgroundColor.r / 255.f,
                backgroundColor.g / 255.f,
                backgroundColor.b / 255.f,
                backgroundColor.a / 255.f,
            },
        .BorderColor =
            {
                borderColor.r / 255.f,
                borderColor.g / 255.f,
                borderColor.b / 255.f,
                borderColor.a / 255.f,
            },
        .BorderWidth =
            {
                borderWidth.left / command.boundingBox.width,
                borderWidth.right / command.boundingBox.width,
                borderWidth.top / command.boundingBox.height,
                borderWidth.bottom / command.boundingBox.height,
            },
        .BorderRadiusX =
            {
                borderRadius.topLeft / command.boundingBox.width,
                borderRadius.topRight / command.boundingBox.width,
                borderRadius.bottomRight / command.boundingBox.width,
                borderRadius.bottomLeft / command.boundingBox.width,
            },
        .BorderRadiusY = {borderRadius.topLeft / command.boundingBox.height,
                          borderRadius.topRight / command.boundingBox.height,
                          borderRadius.bottomRight / command.boundingBox.height,
                          borderRadius.bottomLeft / command.boundingBox.height},
    };

    auto uiComponentBuffer = device->CreateBuffer({
        .Size = sizeof(UIComponentSettings),
        .Usage = OZZ::rendering::BufferUsage::UniformBuffer,
        .Access = OZZ::rendering::BufferMemoryAccess::CpuToGpu,
    });

    device->UpdateBuffer(uiComponentBuffer, &uiSettings, sizeof(UIComponentSettings), 0);
    uiComponentSettings[id] = uiComponentBuffer;

    material->SetResource(0, 0, cameraBuffer);
    material->SetResource(0, 1, uiComponentBuffer);
    material->SetResource(0, 2, texture->GetRHIHandle());

    const auto vertexBuffer = device->CreateBuffer({
        .Size = sizeof(OZZ::Vertex) * vertices.size(),
        .Usage = OZZ::rendering::BufferUsage::VertexBuffer,
        .Access = OZZ::rendering::BufferMemoryAccess::CpuToGpu,
    });
    const auto indexBuffer = device->CreateBuffer({
        .Size = sizeof(uint32_t) * indices.size(),
        .Usage = OZZ::rendering::BufferUsage::IndexBuffer,
        .Access = OZZ::rendering::BufferMemoryAccess::CpuToGpu,
    });
    device->UpdateBuffer(vertexBuffer, vertices.data(), sizeof(OZZ::Vertex) * vertices.size(), 0);
    device->UpdateBuffer(indexBuffer, indices.data(), sizeof(uint32_t) * indices.size(), 0);

    glm::mat4 transform{1.f};
    transform = glm::translate(transform, translation);
    transform = glm::scale(transform, scale);
    uiSceneObjects[id] = OZZ::scene::SceneObject{
        .Transform = transform,
        .MeshData =
            {
                .VertexBuffer = vertexBuffer,
                .IndexBuffer = indexBuffer,
                .VertexCount = vertices.size(),
                .IndexCount = indices.size(),
            },
        .Mat = material,
    };
}

std::vector<OZZ::scene::SceneObject> ClayUILayer::getSceneObjects() {
    auto sceneObjects = std::vector<OZZ::scene::SceneObject>();

    ScissorDef scissor{};
    uint32_t idx = 0;
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

        idx++;
    }
    return sceneObjects;
}

bool ClayUILayer::isRenderCommandChanged(const Clay_RenderCommand& command, const Clay_RenderCommand& otherCommand) {
    const auto hasChanged = std::memcmp(&command, &otherCommand, sizeof(Clay_RenderCommand)) != 0;
    return hasChanged;
}

void ClayUILayer::generateSquareMesh(std::vector<OZZ::Vertex>& outVertices,
                                     std::vector<uint32_t>& outIndices,
                                     glm::vec2 scale) {
    outIndices = std::vector<uint32_t>{0, 2, 1, 3, 2, 0};
    outVertices =
        std::vector<OZZ::Vertex>{{.Position = {0.f, 0.f, 0.f}, .Normal = {0.f, 0.f, 1.f}, .UV = {0.f, 0.f}},
                                 {.Position = {scale.x, 0.f, 0.f}, .Normal = {0.f, 0.f, 1.f}, .UV = {1.f, 0.f}},
                                 {.Position = {scale.x, scale.y, 0.f}, .Normal = {0.f, 0.f, 1.f}, .UV = {1.f, 1.f}},
                                 {.Position = {0.f, scale.y, 0.f}, .Normal = {0.f, 0.f, 1.f}, .UV = {0.f, 1.f}}};
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
            .Position = {left, bottom, 0.f},
            .Color = {1.f, 1.f, 1.f, 1.f},
            .UV = {UV.x, UV.y},
        };
        // second vertex
        auto bottomRight = OZZ::Vertex{
            .Position = {right, bottom, 0.f},
            .Color = {1.f, 1.f, 1.f, 1.f},
            .UV = {UV.z, UV.y},
        };
        // third vertex
        auto topLeft = OZZ::Vertex{
            .Position = {left, top, 0.f},
            .Color = {1.f, 1.f, 1.f, 1.f},
            .UV = {UV.x, UV.w},
        };
        // fourth vertex
        auto topRight = OZZ::Vertex{
            .Position = {right, top, 0.f},
            .Color = {1.f, 1.f, 1.f, 1.f},
            .UV = {UV.z, UV.w},
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

bool ClayUILayer::render(OZZ::rendering::RHIFrameContext& frameContext) {
    if (!renderTarget) {
        spdlog::error("Renderable {} invalid -- no render target", GetRenderableName());
        return false;
    }
    renderTarget->Begin(frameContext);
    auto attributeDescriptions = OZZ::Vertex::GetAttributeDescriptions();
    device->SetGraphicsState(frameContext,
                             {
                                 .Rasterization =
                                     {
                                         .Cull = OZZ::rendering::CullMode::None,
                                         .Front = OZZ::rendering::FrontFace::CounterClockwise,
                                     },
                                 .DepthStencil =
                                     {
                                         .DepthTestEnable = false,
                                         .DepthWriteEnable = false,
                                         .DepthCompareOp = OZZ::rendering::CompareOp::LessOrEqual,
                                     },
                                 .ColorBlend = {OZZ::rendering::ColorBlendAttachmentState{
                                     .BlendEnable = true,
                                     .SrcColorFactor = OZZ::rendering::BlendFactor::SrcAlpha,
                                     .DstColorFactor = OZZ::rendering::BlendFactor::OneMinusSrcAlpha,
                                     .ColorBlendOp = OZZ::rendering::BlendOp::Add,
                                     .SrcAlphaFactor = OZZ::rendering::BlendFactor::One,
                                     .DstAlphaFactor = OZZ::rendering::BlendFactor::OneMinusSrcAlpha,
                                     .AlphaBlendOp = OZZ::rendering::BlendOp::Add,
                                     .ColorWriteMask = static_cast<OZZ::rendering::ColorComponentFlags>(
                                         OZZ::rendering::ColorComponent::R | OZZ::rendering::ColorComponent::G |
                                         OZZ::rendering::ColorComponent::B | OZZ::rendering::ColorComponent::A),
                                 }},
                                 .ColorBlendAttachmentCount = 1,
                                 .VertexInput =
                                     {
                                         .Bindings = {OZZ::Vertex::GetBindingDescription()},
                                         .BindingCount = 1,
                                         .Attributes = {attributeDescriptions[0],
                                                        attributeDescriptions[1],
                                                        attributeDescriptions[2],
                                                        attributeDescriptions[3]},
                                         .AttributeCount = attributeDescriptions.size(),
                                     },
                             });
    for (auto& object : getSceneObjects()) {
        auto& [transform, mesh, objMat] = object;
        objMat->Bind(frameContext);

        auto pipelineLayout = device->GetShaderPipelineLayoutHandle(objMat->GetShader()->GetRHIHandle());
        device->SetPushConstants(
            frameContext, pipelineLayout, OZZ::rendering::ShaderStageFlags::Vertex, 0, sizeof(glm::mat4), &transform);
        device->BindBuffer(frameContext, mesh.VertexBuffer);
        device->BindBuffer(frameContext, mesh.IndexBuffer);
        device->DrawIndexed(frameContext, mesh.IndexCount, 1, 0, 0, 0);
    }
    renderTarget->End(frameContext);

    return true;
}
