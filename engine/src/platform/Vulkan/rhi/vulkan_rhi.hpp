#pragma once

#include "core/macro.hpp"
#include "platform/Vulkan/vulkan_types.hpp"

#include <array>
#include <deque>
#include <functional>
#include <stdint.h>
#include <vector>

struct GLFWwindow;

namespace Yutrel
{
    // todo 临时
    class Mesh;

    struct RHIInitInfo
    {
        GLFWwindow* raw_window;
        uint32_t width;
        uint32_t height;
    };

    class VulkanRHI
    {
    public:
        void Init(RHIInitInfo info);

        void Clear();

        //----------Tick---------
        void PrepareContext();

        void WaitForFences();

        void ResetCommandPool();

        void PrepareBeforePass();

        void SubmitRendering();

        //---------获取信息------------
        uint32_t GetCurrentFrameCount() { return m_cur_frame; }

        FrameData& GetCurrentFrame() { return m_frames[m_cur_frame % FRAME_OVERLAP]; }

        uint32_t GetCurrentSwapchainImageIndex() { return m_cur_swapchain_image_index; }

        VkImage GetCurrentSwapchainImage() { return m_swapchain_images[m_cur_swapchain_image_index]; }

        RHISwapChainDesc GetSwapChainInfo();

        VkCommandBuffer GetCurrentCommandBuffer() const { return m_cur_command_buffer; }

        //---------创建对象------------
        void CreateRenderPass(const VkRenderPassCreateInfo* info, VkRenderPass* out_render_pass);

        void CreateFramebuffer(const VkFramebufferCreateInfo* info, VkFramebuffer* out_framebuffer);

        bool CreateShaderModule(const std::vector<unsigned char>& shader_code, VkShaderModule* out_shader_module);

        void DestroyShaderModule(VkShaderModule shader);

        void CreatePipelineLayout(const VkPipelineLayoutCreateInfo* info, VkPipelineLayout* out_layout);

        void CreateGraphicsPipelines(const RHIGraphicsPipelineCreateInfo& info, VkPipeline* out_pipeline);

        // todo 用rhipipelinecreateinfo初始化
        void CreateComputePipelines(VkPipelineCache pipelineCache,
                                    uint32_t createInfoCount,
                                    const VkComputePipelineCreateInfo* pCreateInfos,
                                    const VkAllocationCallbacks* pAllocator,
                                    VkPipeline* pPipelines);

        void CreateImage(const VkImageCreateInfo* create_info, const VmaAllocationCreateInfo* alloc_info, AllocatedImage* out_image);

        void CreateImageView(const VkImageViewCreateInfo* info, AllocatedImage* out_image);

        void CreateDescriptorLayout(RHIDescriptorLayoutCreateInfo& info, VkDescriptorSetLayout* out_layout);

        void AllocateDescriptorSets(VkDescriptorSetLayout layout, VkDescriptorSet* out_set);

        void UpdateDescriptorSets(uint32_t descriptor_write_count, const VkWriteDescriptorSet* p_descriptor_writes, uint32_t descriptor_copy_count, const VkCopyDescriptorSet* p_descriptor_copies);

        //----------单次指令----------
        VkCommandBuffer BeginSingleTimeCommands();

        void EndSingleTimeCommands(VkCommandBuffer cmd_buffer);

        //----------渲染资源----------
        void UploadMesh(Mesh& mesh);

        // 转换图像布局
        void TransitionImage(VkCommandBuffer cmd_buffer, VkImage image, VkImageLayout cur_layout, VkImageLayout new_layout);

        // 拷贝图像
        void CopyImageToImage(VkCommandBuffer cmd_buffer, VkImage source, VkImage destination, VkExtent2D src_size, VkExtent2D dst_size);

    private:
        //--------初始化----------
        void InitVulkan(GLFWwindow* raw_window);
        // 指令池与指令缓冲
        void InitCommands();
        // 描述符
        void InitDescriptorPool();
        // 同步设施
        void InitSyncStructures();
        // 交换链
        void InitSwapchain(uint32_t width, uint32_t height);
        // 帧缓冲
        void InitDepthImage();
        //------------------------

    private:
        // 同时渲染的帧数
        static constexpr uint8_t FRAME_OVERLAP{2};

        // 允许验证层
        bool m_enable_validation_layers{true};

        // 当前帧数
        uint32_t m_cur_frame{0};

        // 删除队列
        DeletionQueue m_main_deletion_queue;

        // vulkan实例
        VkInstance m_instance;
        // vulkan DebugMessenger
        VkDebugUtilsMessengerEXT m_debug_messenger;
        // 物理设备
        VkPhysicalDevice m_physical_device;
        // 逻辑设备
        VkDevice m_device;
        // 窗口表面
        VkSurfaceKHR m_surface;
        // 队列
        VkQueue m_graphics_queue;
        uint32_t m_graphics_queue_family;
        // 内存分配器
        VmaAllocator m_allocator;

        // 单次指令的指令池
        VkCommandPool m_rhi_command_pool;

        // 帧数据
        std::array<FrameData, FRAME_OVERLAP> m_frames;

        // 描述符池
        VkDescriptorPool m_descriptor_pool;

        // 视口和裁剪
        VkViewport m_viewport;
        VkRect2D m_scissor;
        // 交换链
        VkSwapchainKHR m_swapchain;
        VkFormat m_swapchain_image_format;
        // 交换范围
        VkExtent2D m_swapchain_extent;
        // 交换链图像
        std::vector<VkImage> m_swapchain_images;
        std::vector<VkImageView> m_swapchain_image_views;

        // 深度图像
        AllocatedImage m_depth_image;
        VkImageView m_depth_image_view;
        VkFormat m_depth_format;

        // 当前指令缓冲
        VkCommandBuffer m_cur_command_buffer;
        // 当前交换链图像索引
        uint32_t m_cur_swapchain_image_index;
    };
} // namespace Yutrel