#pragma once

#include "platform/Vulkan/vulkan_types.hpp"

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

#include <string>
#include <vector>

namespace Yutrel
{
    struct VertexInputDescription
    {
        std::vector<VkVertexInputBindingDescription> bindings;
        std::vector<VkVertexInputAttributeDescription> attributes;

        VkPipelineVertexInputStateCreateFlags flags{0};
    };

    struct Vertex
    {
        glm::vec3 position;
        glm::vec3 normal;
        glm::vec3 color;
        glm::vec2 uv;

        static VertexInputDescription GetVertexDescription()
        {
            VertexInputDescription description;

            // 顶点输入绑定
            VkVertexInputBindingDescription main_binding{};
            main_binding.binding   = 0;
            main_binding.stride    = sizeof(Vertex);
            main_binding.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

            description.bindings.push_back(main_binding);

            // Position
            VkVertexInputAttributeDescription position_attribute{};
            position_attribute.binding  = 0;
            position_attribute.location = 0;
            position_attribute.format   = VK_FORMAT_R32G32B32_SFLOAT;
            position_attribute.offset   = offsetof(Vertex, position);

            // Normal
            VkVertexInputAttributeDescription normal_attribute{};
            normal_attribute.binding  = 0;
            normal_attribute.location = 1;
            normal_attribute.format   = VK_FORMAT_R32G32B32_SFLOAT;
            normal_attribute.offset   = offsetof(Vertex, normal);

            // color
            VkVertexInputAttributeDescription color_attribute{};
            color_attribute.binding  = 0;
            color_attribute.location = 2;
            color_attribute.format   = VK_FORMAT_R32G32B32_SFLOAT;
            color_attribute.offset   = offsetof(Vertex, color);

            // uv
            VkVertexInputAttributeDescription uv_attribute{};
            uv_attribute.binding  = 0;
            uv_attribute.location = 3;
            uv_attribute.format   = VK_FORMAT_R32G32_SFLOAT;
            uv_attribute.offset   = offsetof(Vertex, uv);

            description.attributes.push_back(position_attribute);
            description.attributes.push_back(normal_attribute);
            description.attributes.push_back(color_attribute);
            description.attributes.push_back(uv_attribute);

            return description;
        }
    };

    struct Mesh
    {
        std::vector<Vertex> vertices;

        AllocatedBuffer vertex_buffer;

        // bool LoadFromObj(const std::string& file_path);
    };

} // namespace Yutrel
