#pragma once

#include "core/macro.hpp"
#include "platform/Vulkan/utils/vulkan_utils.hpp"
#include "platform/Vulkan/vulkan_types.hpp"

#include <array>
#include <deque>
#include <functional>
#include <stdint.h>
#include <vector>
#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_handles.hpp>
#include <vulkan/vulkan_structs.hpp>

struct GLFWwindow;

namespace Yutrel
{
    struct RHIInitInfo
    {
        GLFWwindow* raw_window;
        uint32_t width;
        uint32_t height;
    };

    // 帧数据
    struct FrameData
    {
        // 指令
        vk::CommandPool cmd_pool;
        vk::CommandBuffer main_cmd_buffer;

        // 同步
        vk::Semaphore finished_for_presentation_semaphore, available_for_render_semaphore;
        vk::Fence render_fence;

        // 删除队列
        DeletionQueue deletion_queue;
    };

    class VulkanRHI
    {
    public:
        void Init(RHIInitInfo info);

        void Clear();

        //----------Tick---------
        void PrepareBeforePass();

        void SubmitRendering();

        //-----------Resize-----------
        void UpdateSwapchainSize(uint32_t width, uint32_t height);

        bool RequestResize() { return m_resize_requested; }

        void ResizeSwapchain();

        //----------单次指令----------
        vk::CommandBuffer BeginSingleTimeCommands();

        void EndSingleTimeCommands(vk::CommandBuffer cmd_buffer);

        //-----------图像操作----------
        // 转换图像布局
        void TransitionImage(vk::CommandBuffer cmd_buffer, vk::Image image, vk::ImageLayout cur_layout, vk::ImageLayout new_layout);

        // 拷贝图像
        void CopyImageToImage(vk::CommandBuffer cmd_buffer, vk::Image source, vk::Image destination, vk::Extent2D src_size, vk::Extent2D dst_size);

        // 生成mipmap
        void GenerateMipmaps(vk::CommandBuffer cmd_buffer, vk::Image image, vk::Extent2D extent);

        //-----------获取----------
        vk::CommandBuffer GetCurrentCommandBuffer() { return GetCurrentFrame().main_cmd_buffer; }

        vk::Extent2D GetSwapChainExtent() const { return m_swapchain_extent; }

        vk::Image GetCurrentSwapchainImage() const { return m_swapchain_images[m_cur_swapchain_image_index]; }

        vk::ImageView GetCurrentSwapchainImageView() const { return m_swapchain_image_views[m_cur_swapchain_image_index]; }

        vk::PhysicalDeviceProperties GetPhysicalDeviceProperties() const { return m_GPU_properties; }

        //-----------创建------------
        AllocatedImage CreateImage(vk::Extent3D extent, vk::Format format, vk::ImageUsageFlags usage, bool mipmapped = false);

        void UploadImageData(void* data, AllocatedImage& image);

        AllocatedBuffer CreateBuffer(size_t alloc_size, vk::BufferUsageFlags buffer_usage, vma::MemoryUsage memory_usage, bool auto_destroy = true);

        void DestroyBuffer(const AllocatedBuffer& buffer);

        vk::ShaderModule CreateShaderModule(const std::vector<unsigned char>& shader_code);

        void DestroyShaderModule(vk::ShaderModule shader);

        vk::PipelineLayout CreatePipelineLayout(const vk::PipelineLayoutCreateInfo& info);

        vk::Pipeline CreateRenderPipeline(const RenderPipelineCreateInfo& info);

        vk::DescriptorSetLayout CreateDescriptorSetLayout(DescriptorSetLayoutCreateInfo& info);

        vk::DescriptorSet AllocateDescriptorSets(vk::DescriptorSetLayout layout);

        void UpdateDescriptorSets(DescriptorWriter& writer, vk::DescriptorSet set);

        vk::DeviceAddress GetBufferDeviceAddress(const vk::BufferDeviceAddressInfo& info);

        vk::Sampler CreateSampler(const vk::SamplerCreateInfo& info);

    private:
        //--------初始化----------
        void InitVulkan(GLFWwindow* raw_window);
        // 交换链
        void InitSwapchain(uint32_t width, uint32_t height);
        void DestroySwapchain();
        // 指令池与指令缓冲
        void InitCommands();
        // 同步设施
        void InitSyncStructures();
        // 描述符
        void InitDescriptorPool();
        // Imgui
        void InitImgui(GLFWwindow* raw_window);
        //------------------------

        FrameData& GetCurrentFrame() { return m_frames[m_cur_frame % MAX_FRAMES_IN_FLIGHT]; }

    private:
        // 同时渲染的帧数
        static constexpr uint8_t MAX_FRAMES_IN_FLIGHT{2};

        // 允许验证层
        bool m_enable_validation_layers{true};

        // 当前帧数
        uint32_t m_cur_frame{0};

        // 删除队列
        DeletionQueue m_main_deletion_queue;

        vk::Instance m_instance;
        vk::DebugUtilsMessengerEXT m_debug_messenger;
        vk::SurfaceKHR m_surface;
        vk::PhysicalDevice m_GPU;
        vk::PhysicalDeviceProperties m_GPU_properties;
        vk::Device m_device;
        vk::Queue m_graphics_queue;
        uint32_t m_graphics_queue_family;

        vma::Allocator m_allocator;

        // 交换链
        vk::SwapchainKHR m_swapchain;
        vk::Format m_swapchain_format;
        vk::Extent2D m_swapchain_extent;
        std::vector<vk::Image> m_swapchain_images;
        std::vector<vk::ImageView> m_swapchain_image_views;

        // 帧数据
        std::array<FrameData, MAX_FRAMES_IN_FLIGHT> m_frames;

        // 单次指令指令池
        vk::CommandPool m_rhi_cmd_pool;

        // 描述符池
        vk::DescriptorPool m_descriptor_pool;

        // 每一帧信息
        uint32_t m_cur_swapchain_image_index;

        // 窗口大小改变
        bool m_resize_requested{false};
        VkExtent2D m_new_swapchain_extent;
    };
} // namespace Yutrel