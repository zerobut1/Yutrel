#pragma once

#include <vulkan/vulkan.hpp>

#include <deque>

namespace Yutrel
{
    struct DescriptorSetLayoutCreateInfo
    {
        std::vector<vk::DescriptorSetLayoutBinding> bindings;

        vk::ShaderStageFlags shader_stages;

        DescriptorSetLayoutCreateInfo& AddBinding(uint32_t binding, vk::DescriptorType type);

        DescriptorSetLayoutCreateInfo& SetShaderStage(vk::ShaderStageFlags shader_stage);

        void Clear();
    };

    struct DescriptorWriter
    {
        std::deque<vk::DescriptorImageInfo> image_infos;
        std::deque<vk::DescriptorBufferInfo> buffer_infos;
        std::vector<vk::WriteDescriptorSet> writes;

        DescriptorWriter& WriteImage(int binding, vk::DescriptorImageInfo info, vk::DescriptorType type);
        DescriptorWriter& WriteImage(int binding, vk::ImageView image, vk::Sampler sampler, vk::ImageLayout layout, vk::DescriptorType type);
        DescriptorWriter& WriteBuffer(int binding, vk::Buffer buffer, size_t size, size_t offset, vk::DescriptorType type);

        void Clear();
    };
} // namespace Yutrel