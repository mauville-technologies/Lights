#include <utility>

//
// Created by ozzadar on 2025-09-20.
//

#pragma once
#include <lights/core/rendering/texture.h>
#include <memory>
#include <unordered_set>

#define CLAY_IMAGE_STRING(Arg) static_cast<void*>(const_cast<char*>(Arg))
#define CLAY_EXTRACT_IMAGE_STRING(RenderCommand) static_cast<const char*>(RenderCommand.renderData.image.imageData)

inline std::unordered_set<void*>& Clay__ActiveImageWrappers() {
    static std::unordered_set<void*> s;
    return s;
}

inline void* WrapTextureForUI(std::shared_ptr<OZZ::Texture> texture) {
    auto* ptr = new std::shared_ptr(std::move(texture));
    Clay__ActiveImageWrappers().insert(ptr);
    return ptr;
}

inline std::shared_ptr<OZZ::Texture> UnwrapUITexture(void* inTexture) {
    auto& active = Clay__ActiveImageWrappers();
    if (!active.contains(inTexture)) {
        // Pointer not registered — arena corruption or duplicate unwrap. Skip safely.
        return nullptr;
    }
    active.erase(inTexture);

    auto* sharedPointerPointer = static_cast<std::shared_ptr<OZZ::Texture>*>(inTexture);
    std::shared_ptr<OZZ::Texture> newTexture = std::move(*sharedPointerPointer);
    delete sharedPointerPointer;
    return newTexture;
}

// Call once per frame after Clay_EndLayout to clear wrappers that Clay culled
// (elements offscreen don't emit render commands, so their wrappers are never unwrapped).
inline void ClayUI_FlushLeakedWrappers() {
    for (void* ptr : Clay__ActiveImageWrappers()) {
        delete static_cast<std::shared_ptr<OZZ::Texture>*>(ptr);
    }
    Clay__ActiveImageWrappers().clear();
}
