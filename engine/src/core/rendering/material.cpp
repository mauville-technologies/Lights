//
// Created by ozzadar on 2024-12-19.
//

#include "lights/core/rendering/material.h"

#include "glm/gtc/type_ptr.hpp"

#include <ranges>

namespace OZZ {
    Material::~Material() {
        for (const auto descriptorSet : descriptorSets | std::views::values) {
            device->FreeDescriptorSet(descriptorSet);
        }
        shader.reset();
    }

    void Material::Bind(rendering::RHIFrameContext& frameContext) {
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
                    if (std::holds_alternative<rendering::RHITextureHandle>(resource)) {
                        writes.push_back(rendering::RHIDescriptorWrite{
                            .Binding = bindingPoint,
                            .Type = layout.Sets[set].Bindings[bindingPoint].Type,
                            .Image =
                                {
                                    .Texture = std::get<rendering::RHITextureHandle>(resource),
                                },
                        });
                    } else if (std::holds_alternative<rendering::RHIBufferHandle>(resource)) {
                        writes.push_back(rendering::RHIDescriptorWrite{
                            .Binding = bindingPoint,
                            .Type = layout.Sets[set].Bindings[bindingPoint].Type,
                            .Buffer =
                                {
                                    .Buffer = std::get<rendering::RHIBufferHandle>(resource),
                                    .Offset = 0,
                                    .Range = ~0ULL, // VK_WHOLE_SIZE equivalent
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
        if (setLayout.BindingCount < binding) {
            spdlog::error("Attempted to set resource for binding {} in set {} which is out of bounds for shader with "
                          "{} bindings in that set.",
                          binding,
                          set,
                          setLayout.BindingCount);
            return false;
        }

        const auto bindingType = setLayout.Bindings[binding].Type;
        if (bindingType == rendering::DescriptorType::CombinedImageSampler ||
            bindingType == rendering::DescriptorType::SampledImage ||
            bindingType == rendering::DescriptorType::StorageImage) {
            if (!std::holds_alternative<rendering::RHITextureHandle>(resource)) {
                spdlog::error("Attempted to set non-texture resource for binding {} in set {} which expects a texture.",
                              binding,
                              set);
                return false;
            }
        } else if (bindingType == rendering::DescriptorType::UniformBuffer ||
                   bindingType == rendering::DescriptorType::StorageBuffer) {
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
