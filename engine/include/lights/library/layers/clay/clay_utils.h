#include <utility>

//
// Created by ozzadar on 2025-09-20.
//

#pragma once

#define CLAY_IMAGE_STRING(Arg) static_cast<void*>(const_cast<char*>(Arg))
#define CLAY_EXTRACT_IMAGE_STRING(RenderCommand) static_cast<const char*>(RenderCommand.renderData.image.imageData)

inline void* WrapTextureForUI(std::shared_ptr<OZZ::Texture> texture) {
    return new std::shared_ptr(std::move(texture));
}

inline std::shared_ptr<OZZ::Texture> UnwrapUITexture(void* inTexture) {
    // convert it back
    auto* sharedPointerPointer = static_cast<std::shared_ptr<OZZ::Texture>*>(inTexture);

    // increase ref count
    std::shared_ptr<OZZ::Texture> newTexture = *sharedPointerPointer;

    // delete the texture
    delete sharedPointerPointer;

    // return the ref counted texture
    return newTexture;
}
