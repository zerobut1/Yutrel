#include "DescriptorSet.h"

namespace Yutrel
{
    DescriptorSetLayoutCreateInfo& DescriptorSetLayoutCreateInfo::AddBinding(uint32_t binding, vk::DescriptorType type)
    {
        auto new_bind =
            vk::DescriptorSetLayoutBinding()
                .setBinding(binding)
                .setDescriptorCount(1)
                .setDescriptorType(type);

        bindings.push_back(new_bind);

        return *this;
    }

    DescriptorSetLayoutCreateInfo& DescriptorSetLayoutCreateInfo::SetShaderStage(vk::ShaderStageFlags shader_stage)
    {
        shader_stages = shader_stage;

        return *this;
    }

    void DescriptorSetLayoutCreateInfo::Clear()
    {
        bindings.clear();
    }

    DescriptorWriter& DescriptorWriter::WriteImage(int binding, vk::DescriptorImageInfo info, vk::DescriptorType type)
    {
        auto write =
            vk::WriteDescriptorSet()
                .setDstSet({})
                .setDstBinding(binding)
                .setDescriptorCount(1)
                .setDescriptorType(type)
                .setImageInfo(info);

        writes.push_back(write);

        return *this;
    }

    DescriptorWriter& DescriptorWriter::WriteImage(int binding, vk::ImageView image, vk::Sampler sampler, vk::ImageLayout layout, vk::DescriptorType type)
    {
        auto& info =
            image_infos.emplace_back(
                vk::DescriptorImageInfo()
                    .setSampler(sampler)
                    .setImageView(image)
                    .setImageLayout(layout));

        return WriteImage(binding, info, type);
    }

    DescriptorWriter& DescriptorWriter::WriteBuffer(int binding, vk::Buffer buffer, size_t size, size_t offset, vk::DescriptorType type)
    {
        auto& info =
            buffer_infos.emplace_back(
                vk::DescriptorBufferInfo()
                    .setBuffer(buffer)
                    .setOffset(offset)
                    .setRange(size));

        auto write =
            vk::WriteDescriptorSet()
                .setDstSet({})
                .setDstBinding(binding)
                .setDescriptorCount(1)
                .setDescriptorType(type)
                .setBufferInfo(info);

        writes.push_back(write);

        return *this;
    }

    void DescriptorWriter::Clear()
    {
        image_infos.clear();
        buffer_infos.clear();
        writes.clear();
    }

} // namespace Yutrel