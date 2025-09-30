//
// Created by ozzadar on 2025-09-20.
//

#pragma once

#define CLAY_IMAGE_STRING(Arg) static_cast<void*>(const_cast<char*>(Arg))
#define CLAY_EXTRACT_IMAGE_STRING(RenderCommand) static_cast<const char*>(RenderCommand.renderData.image.imageData)
