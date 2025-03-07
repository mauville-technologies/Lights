//
// Created by ozzadar on 2025-02-17.
//
#pragma once
#include <lights/game/game_object.h>
#include <lights/scene/scene.h>
#include <unordered_map>
#include <string>

namespace OZZ::game::scene {
    class Sprite final : public GameObject {
    public:
        explicit Sprite(std::shared_ptr<OzzWorld2D> inWorld, const std::filesystem::path& texture);
        void Tick(float DeltaTime) override;

        std::vector<SceneObject> GetSceneObjects() override;

        void SetTexture(const std::filesystem::path& inPath);

        /**
         * Creates a new body for the sprite, replaces the old one if it exists
         * @tparam Args Same arguments as OzzWorld2D::CreateBody
         * @param args  Same arguments as OzzWorld2D::CreateBody
         */
        template <typename... Args>
    	void AddBody(Args&&... args) {
        	if (MainBody == InvalidBodyID) {
        		world->DestroyBody(MainBody);
        		MainBody = InvalidBodyID;
        	}
			MainBody = world->CreateBody(std::forward<Args>(args)...);
		}

    	[[nodiscard]] Body* GetBody() const {
		    return world->GetBody(MainBody);
    	}

    private:
        BodyID MainBody { InvalidBodyID };

#ifdef OZZ_DEBUG
        bool bDrawDebug = true;
#else
        bool bDrawDebug = false;
#endif

        float debugDrawSize = 2;

        SceneObject sceneObject;

        static std::unordered_map<std::string, SceneObject> debugShapes;
        static std::shared_ptr<Shader> debugShader;
    };
}
