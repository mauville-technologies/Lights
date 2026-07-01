//
// Created by ozzadar on 2024-12-19.
//

#include "lights/core/rendering/material.h"
#include "lights/core/util/profiling.h"

#include "glm/gtc/type_ptr.hpp"

#include <climits>
#include <ranges>

namespace OZZ {
    Material::~Material() {
        for (const auto descriptorSet : descriptorSets | std::views::values) {
            device->FreeDescriptorSet(descriptorSet);
        }
        shader.reset();
    }

    void Material::Bind(rendering::RHIFrameContext& frameContext) {
        OZZ_PROFILE_FUNCTION;
        if (!shader)
            return;

        const auto layout = shader->GetLayoutDescriptor();
        const auto setLayouts = device->GetShaderDescriptorSetLayoutHandles(shader->GetRHIHandle());
        // Update any dirty descriptor sets
        for (const auto& [set, binding] : resources) {
            const auto setLayout = setLayouts[set];
            // Are they dirty?
            if (dirtyDescriptorSets[set]) {
                // Create the set if it doesn't exist
                if (descriptorSets.contains(set)) {
                    device->FreeDescriptorSet(descriptorSets[set]);
                }
                descriptorSets[set] = device->CreateDescriptorSet(setLayout);
                const auto descriptorSet = descriptorSets[set];
                std::vector<rendering::RHIDescriptorWrite> writes;
                for (const auto& [bindingPoint, resource] : binding) {
                    // Search by OriginalBinding (falls back to Binding when there's no WebGPU
                    // GLSL remap). Must NOT also match on the raw `Binding` field: for remapped
                    // shaders, one entry's post-remap physical Binding can coincidentally equal
                    // a *different* entry's pre-remap OriginalBinding (e.g. game_composite.frag's
                    // overlayColorTexture ends up at physical binding 3, which collides with
                    // sceneDepthTexture's original binding 3), causing a false-positive match.
                    // Skip Count==0 slots — these are consumed/empty placeholder entries.
                    const rendering::RHIDescriptorSetLayoutBinding* bd = nullptr;
                    for (uint32_t bi = 0; bi < layout.Sets[set].BindingCount; bi++) {
                        const auto& b = layout.Sets[set].Bindings[bi];
                        if (b.Count == 0) continue;
                        uint32_t effOrig = (b.OriginalBinding == UINT32_MAX) ? b.Binding : b.OriginalBinding;
                        if (effOrig == bindingPoint) {
                            bd = &b;
                            break;
                        }
                    }
                    if (!bd) {
                        spdlog::error("Bind: binding {} not found in set {} layout", bindingPoint, set);
                        continue;
                    }
                    if (std::holds_alternative<rendering::RHITextureHandle>(resource)) {
                        writes.push_back(rendering::RHIDescriptorWrite{
                            .Binding = bd->Binding, // use actual (possibly remapped) binding
                            .Type = bd->Type,
                            .Image =
                                {
                                    .Texture = std::get<rendering::RHITextureHandle>(resource),
                                },
                        });
                    } else if (std::holds_alternative<rendering::RHIBufferHandle>(resource)) {
                        writes.push_back(rendering::RHIDescriptorWrite{
                            .Binding = bd->Binding, // use actual (possibly remapped) binding
                            .Type = bd->Type,
                            .Buffer =
                                {
                                    .Buffer = std::get<rendering::RHIBufferHandle>(resource),
                                    .Offset = 0,
                                    .Range = ~0ULL,
                                },
                        });
                    }
                }
                device->UpdateDescriptorSet(descriptorSet, writes);
                dirtyDescriptorSets[set] = false;
            }
        }

        // Bind the descriptor sets
        shader->Bind(frameContext);
        const auto pipelineLayout = device->GetShaderPipelineLayoutHandle(shader->GetRHIHandle());
        for (const auto& [set, descriptorSet] : descriptorSets) {
            device->BindDescriptorSet(frameContext, pipelineLayout, set, descriptorSet);
        }
        if (settings.bHasScissor) {
            device->SetScissor(frameContext,
                               {
                                   .X = settings.Scissor.x,
                                   .Y = settings.Scissor.y,
                                   .Width = static_cast<uint32_t>(settings.Scissor.z),
                                   .Height = static_cast<uint32_t>(settings.Scissor.w),
                               });
        }
    }

    bool Material::SetResource(const uint32_t set,
                               const uint32_t binding,
                               const std::variant<rendering::RHITextureHandle, rendering::RHIBufferHandle>& resource) {
        const auto layout = shader->GetLayoutDescriptor();
        if (layout.SetCount < set) {
            spdlog::error("Attempted to set resource for set {} which is out of bounds for shader with {} sets.",
                          set,
                          layout.SetCount);
            return false;
        }

        const auto setLayout = layout.Sets[set];

        // Search by OriginalBinding (falls back to Binding when there's no WebGPU GLSL
        // remap) — see the matching comment in Bind() for why the raw Binding field must
        // not also be checked here. Skip Count==0 slots which are consumed/empty placeholders.
        const rendering::RHIDescriptorSetLayoutBinding* bindingDesc = nullptr;
        for (uint32_t i = 0; i < setLayout.BindingCount; i++) {
            const auto& b = setLayout.Bindings[i];
            if (b.Count == 0) continue;
            uint32_t effOrig = (b.OriginalBinding == UINT32_MAX) ? b.Binding : b.OriginalBinding;
            if (effOrig == binding) {
                bindingDesc = &b;
                break;
            }
        }
        if (!bindingDesc) {
            spdlog::error("Attempted to set resource for binding {} in set {} which was not found in the shader layout.",
                          binding, set);
            return false;
        }

        const auto bindingType = bindingDesc->Type;
        if (bindingType == rendering::DescriptorType::CombinedImageSampler ||
            bindingType == rendering::DescriptorType::SampledImage ||
            bindingType == rendering::DescriptorType::StorageImage ||
            bindingType == rendering::DescriptorType::DepthSampledImage) {
            if (!std::holds_alternative<rendering::RHITextureHandle>(resource)) {
                spdlog::error("Attempted to set non-texture resource for binding {} in set {} which expects a texture.",
                              binding,
                              set);
                return false;
            }
        } else if (bindingType == rendering::DescriptorType::UniformBuffer ||
                   bindingType == rendering::DescriptorType::StorageBuffer ||
                   bindingType == rendering::DescriptorType::ReadOnlyStorageBuffer) {
            if (!std::holds_alternative<rendering::RHIBufferHandle>(resource)) {
                spdlog::error("Attempted to set non-buffer resource for binding {} in set {} which expects a buffer.",
                              binding,
                              set);
                return false;
            }
        } else {
            spdlog::error("Unsupported descriptor type for binding {} in set {}.", binding, set);
            return false;
        }

        if (!resources.contains(set)) {
            resources[set] = {};
        }

        dirtyDescriptorSets[set] = true;
        resources[set][binding] = resource;

        return true;
    }

    void Material::RemoveResource(const uint32_t set, const uint32_t binding) {
        if (resources.contains(set)) {
            resources[set].erase(binding);
            if (resources[set].empty()) {
                resources.erase(set);
            }
        }
    }
} // namespace OZZ
