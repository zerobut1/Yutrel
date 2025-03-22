#pragma once

#include <vk_mem_alloc.h>
#include <vulkan/vulkan.hpp>

struct GLFWwindow;

namespace Yutrel
{
    class Renderer final
    {
    public:
        struct CreateInfo
        {
            vk::PhysicalDeviceFeatures device_features{};
            vk::PhysicalDeviceVulkan12Features device_features_12{};
            vk::PhysicalDeviceVulkan13Features device_features_13{};
        };

    public:
        Renderer() = delete;
        explicit Renderer(const CreateInfo& info);
        ~Renderer();

        Renderer(const Renderer&)            = delete;
        Renderer& operator=(const Renderer&) = delete;

    public:
        std::shared_ptr<class Context> getContext() { return m_context; }
        std::shared_ptr<class ResourceManager> getResourceManager() { return m_resource_manager; }
        std::shared_ptr<class Frame> getCurrentFrame() const { return m_frames[m_frame_count % s_max_frame]; }

        std::shared_ptr<Frame> prepareBeforeRender();
        void submitRendering(std::shared_ptr<Frame> cur_frame);

        vk::CommandBuffer beginSingleTimeCommandBuffer();
        void endSingleTimeCommandBuffer(vk::CommandBuffer cmd_buffer);

        //--------Buffer---------
        struct Buffer createBuffer(size_t alloc_size, vk::BufferUsageFlags buffer_usage, VmaMemoryUsage memory_usage, VkMemoryPropertyFlags required_flags = 0);

        //--------Image---------
        void transitionImageLayout(vk::CommandBuffer cmd_buffer, vk::Image image, vk::ImageLayout cur_layout, vk::ImageLayout new_layout);

        void copyImageToImage(vk::CommandBuffer cmd_buffer, vk::Image source, vk::Image destination, vk::Extent2D src_size, vk::Extent2D dst_size);

        //--------DescriptorSet----------
        vk::DescriptorSetLayout createDescriptorSetLayout(struct DescriptorSetLayoutCreateInfo& info);

        vk::DescriptorSet allocateDescriptorSets(vk::DescriptorSetLayout layout);

        void updateDescriptorSets(struct DescriptorWriter& writer, vk::DescriptorSet set);

        //--------Pipeline--------------
        vk::ShaderModule createShaderModule(const std::vector<unsigned char>& shader_code);

        void destroyShaderModule(vk::ShaderModule shader);

        vk::PipelineLayout createPipelineLayout(const vk::PipelineLayoutCreateInfo& info);

        vk::Pipeline createRenderPipeline(const struct RenderPipelineCreateInfo& info);

        vk::Pipeline createComputePipeline(const vk::ComputePipelineCreateInfo& info);

    private:
        void init(const CreateInfo& info);
        void shutdown();

    private:
        static constexpr uint8_t s_max_frame{2};

        std::shared_ptr<Context> m_context;
        std::shared_ptr<ResourceManager> m_resource_manager;

        uint32_t m_frame_count{0};
        std::array<std::shared_ptr<Frame>, s_max_frame> m_frames;
        vk::CommandPool m_cmd_pool{nullptr};
        vk::DescriptorPool m_descriptor_pool{nullptr};
    };

} // namespace Yutrel