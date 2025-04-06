//
// Created by ozzadar on 2025-04-06.
//

#pragma once
#include <lights/scene/scene_layer.h>

class MMOCharacterSelect : public OZZ::scene::SceneLayer {
public:
	~MMOCharacterSelect() override = default;

	/// Override
	void Init() override;
	void PhysicsTick(float DeltaTime) override;
	void Tick(float DeltaTime) override;

    void RenderTargetResized(glm::ivec2 size) override;
    std::vector<OZZ::scene::SceneObject> GetSceneObjects() override;
};
