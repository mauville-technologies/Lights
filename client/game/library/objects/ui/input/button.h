//
// Created by ozzadar on 2025-04-13.
//

#pragma once
#include <game/library/objects/ui/ui_component.h>
#include <lights/input/input_subsystem.h>

namespace OZZ::game::objects {
    class TextLabel;

    class Button : public UIComponent {
    public:
        using ClickCallback = std::function<void()>;
        struct ButtonParams {
            uint16_t FontSize { 32 };
            std::filesystem::path FontPath {""};
            std::string Text;

            glm::vec2 Size {
                200.f, 50.f
            };

            glm::vec4 BackgroundColor {
                0.2f, 0.2f, 0.2f, 1.f
            };

            glm::vec3 CursorColor {
                1.f, 1.f, 1.f
            };

            glm::vec3 TextColor {
                1.f, 1.f, 0.f
            };

            glm::vec4 FocusedColor {
                1.f, 0.f, 0.5f, 1.f
            };

            glm::vec4 FocusedThickness {5.f};

            ClickCallback OnClick {nullptr};
            ButtonParams() :
                FontSize(32),
                FontPath(""),
                Text(""),
                Size(200.f, 50.f),
                BackgroundColor(0.2f, 0.2f, 0.2f, 1.f),
                CursorColor(1.f, 1.f, 1.f),
                TextColor(1.f, 1.f, 0.f),
                FocusedColor(1.f, 0.f, 0.5f, 1.f),
                FocusedThickness(5.f),
                OnClick(nullptr) {};
        };

        using ParamsType = ButtonParams;

        explicit Button(GameWorld *inGameWorld, std::shared_ptr<OzzWorld2D> inPhysicsWorld, const ParamsType &inParams = {});
        ~Button() override;
        void Tick(float DeltaTime) override;
        std::vector<scene::SceneObject> GetSceneObjects() override;

        void SetupInput(InputSubsystem* inInputSubsystem);
        void SetFocused(bool focused);
        glm::vec2 GetSize() const { return params.Size; }
        bool TryClick(const glm::vec2& worldPos);

    protected:
        void onPositionChanged() override;

    private:
        void setText(const std::string &inText);
        void updateTextLabel() const;

    private:
        std::pair<uint64_t, TextLabel*> label {UINT16_MAX, nullptr};
        InputSubsystem* inputSubsystem { nullptr };
        std::string inputSID {};

        std::string currentString {};
        ParamsType params {};

        scene::SceneObject backgroundBox {};
        scene::SceneObject cursor {};
        bool isFocused { false };
    };
}
