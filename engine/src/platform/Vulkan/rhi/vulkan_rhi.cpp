#include "yutrel_pch.hpp"

#include "vulkan_rhi.hpp"

#include "platform/Vulkan/initializers/initializers.hpp"
#include "platform/Vulkan/mesh/vulkan_mesh.hpp"
#include "platform/Vulkan/utils/vulkan_utils.hpp"

#include <GLFW/glfw3.h>
#include <VKBootstrap.h>

#define VMA_IMPLEMENTATION
#include <vk_mem_alloc.h>

namespace Yutrel
{
    void VulkanRHI::Init(RHIInitInfo info)
    {
        InitVulkan(info.raw_window, info.width, info.height);

        InitCommands();

        InitDescriptorPool();

        InitSyncStructures();

        InitSwapchain();

        InitDepthImage();
    }

    void VulkanRHI::Clear()
    {
        // 确保GPU已经工作完
        vkDeviceWaitIdle(m_device);

        // 删除队列清空
        m_main_deletion_queue.flush();

        // 销毁窗口表面
        vkDestroySurfaceKHR(m_instance, m_surface, nullptr);

        // 销毁逻辑设备
        vkDestroyDevice(m_device, nullptr);

        // 销毁debug messenger
        vkb::destroy_debug_utils_messenger(m_instance, m_debug_messenger);

        // 销毁vulkan实例
        vkDestroyInstance(m_instance, nullptr);
    }

    void VulkanRHI::PrepareContext()
    {
        m_cur_command_buffer = GetCurrentFrame().main_command_buffer;
    }

    void VulkanRHI::WaitForFences()
    {
        YUTREL_ASSERT(vkWaitForFences(m_device, 1, &GetCurrentFrame().render_fence, VK_TRUE, UINT64_MAX) == VK_SUCCESS, "Failed to synchronize");
    }

    void VulkanRHI::ResetCommandPool()
    {
        YUTREL_ASSERT(vkResetCommandPool(m_device, GetCurrentFrame().command_pool, 0) == VK_SUCCESS, "Failed to reset command pool");
    }

    void VulkanRHI::PrepareBeforePass()
    {
        // 请求图像索引
        // todo recreate swapchain
        YUTREL_ASSERT(vkAcquireNextImageKHR(m_device, m_swapchain, UINT64_MAX, GetCurrentFrame().available_for_render_semaphore, VK_NULL_HANDLE, &m_cur_swapchain_image_index) == VK_SUCCESS, "Failed to acquire next image KHR");

        // 开始指令缓冲
        VkCommandBufferBeginInfo cmd_begin_info = vkinit::CommandBufferBeginInfo(0);

        YUTREL_ASSERT(vkBeginCommandBuffer(GetCurrentFrame().main_command_buffer, &cmd_begin_info) == VK_SUCCESS, "Failed to begin command buffer");
    }

    void VulkanRHI::SubmitRendering()
    {
        // 终止指令缓冲
        YUTREL_ASSERT(vkEndCommandBuffer(m_cur_command_buffer) == VK_SUCCESS, "Failed to end command buffer");

        // --------------提交指令---------
        VkSubmitInfo submit_info         = vkinit::SubmitInfo(&m_cur_command_buffer);
        VkPipelineStageFlags wait_stage  = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        submit_info.pWaitDstStageMask    = &wait_stage;
        submit_info.waitSemaphoreCount   = 1;
        submit_info.pWaitSemaphores      = &GetCurrentFrame().available_for_render_semaphore;
        submit_info.signalSemaphoreCount = 1;
        submit_info.pSignalSemaphores    = &GetCurrentFrame().finished_for_presentation_semaphore;

        // 重设fence
        YUTREL_ASSERT(vkResetFences(m_device, 1, &GetCurrentFrame().render_fence) == VK_SUCCESS, "Failed to reset fence");

        // 提交到队列
        YUTREL_ASSERT(vkQueueSubmit(m_graphics_queue, 1, &submit_info, GetCurrentFrame().render_fence) == VK_SUCCESS, "Failed to submit command");

        //--------------显示图像------------
        VkPresentInfoKHR present_info = vkinit::PresentInfo();

        present_info.pSwapchains        = &m_swapchain;
        present_info.swapchainCount     = 1;
        present_info.pWaitSemaphores    = &GetCurrentFrame().finished_for_presentation_semaphore;
        present_info.waitSemaphoreCount = 1;
        present_info.pImageIndices      = &m_cur_swapchain_image_index;

        // todo recreate swapchain
        YUTREL_ASSERT(vkQueuePresentKHR(m_graphics_queue, &present_info) == VK_SUCCESS, "Failed to present");

        m_cur_frame++;
    }

    void VulkanRHI::CmdBeginRenderPass(VkCommandBuffer cmd_buffer, const VkRenderPassBeginInfo* info, VkSubpassContents contents)
    {
        vkCmdBeginRenderPass(cmd_buffer, info, contents);
    }

    void VulkanRHI::CmdEndRenderPass(VkCommandBuffer cmd_buffer)
    {
        vkCmdEndRenderPass(cmd_buffer);
    }

    void VulkanRHI::CmdBindPipeline(VkCommandBuffer cmd_buffer, VkPipelineBindPoint bind_point, VkPipeline pipeline)
    {
        vkCmdBindPipeline(cmd_buffer, bind_point, pipeline);
    }

    void VulkanRHI::CmdBindVertexBuffers(VkCommandBuffer cmd_buffer, uint32_t first_binding, uint32_t binding_count, const VkBuffer* p_buffers, const VkDeviceSize* p_offsets)
    {
        vkCmdBindVertexBuffers(cmd_buffer, first_binding, binding_count, p_buffers, p_offsets);
    }

    void VulkanRHI::CmdDraw(VkCommandBuffer cmd_buffer, uint32_t vertex_count, uint32_t instance_count, uint32_t first_vertex, uint32_t first_isnstance)
    {
        vkCmdDraw(cmd_buffer, vertex_count, instance_count, first_vertex, first_isnstance);
    }

    void VulkanRHI::InitVulkan(GLFWwindow* raw_window, uint32_t width, uint32_t height)
    {
        LOG_INFO("Initialize Vulkan RHI");

        // 记录交换范围大小
        m_swapchain_extent.width  = width;
        m_swapchain_extent.height = height;

        m_viewport = {0.0f, 0.0f, static_cast<float>(width), static_cast<float>(height), 0.0f, 1.0f};
        m_scissor  = {{0, 0}, m_swapchain_extent};

        // vkb创建器
        vkb::InstanceBuilder builder;

        // 创建vulkan instance
        auto inst_ret =
            builder.set_app_name("Vulkan RHI")
                .request_validation_layers(m_enable_validation_layers)
                .use_default_debug_messenger()
                .require_api_version(1, 3, 0)
                .build();

        vkb::Instance vkb_instance = inst_ret.value();

        // 获取instance
        m_instance        = vkb_instance.instance;
        m_debug_messenger = vkb_instance.debug_messenger;

        // 创建窗口表面
        glfwCreateWindowSurface(m_instance, raw_window, nullptr, &m_surface);

        // 选择物理设备
        vkb::PhysicalDeviceSelector selector{vkb_instance};
        vkb::PhysicalDevice physical_device =
            selector
                .set_minimum_version(1, 3)
                .set_surface(m_surface)
                .select()
                .value();

        // 创建逻辑设备
        vkb::DeviceBuilder device_builder{physical_device};
        // 启用shader draw parameters功能
        VkPhysicalDeviceShaderDrawParametersFeatures shader_draw_parameters_features{};
        shader_draw_parameters_features.sType                = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_DRAW_PARAMETER_FEATURES;
        shader_draw_parameters_features.pNext                = nullptr;
        shader_draw_parameters_features.shaderDrawParameters = VK_TRUE;

        vkb::Device vkb_device = device_builder
                                     .add_pNext(&shader_draw_parameters_features)
                                     .build()
                                     .value();

        // 获取设备
        m_physical_device            = physical_device.physical_device;
        m_physical_device_properties = vkb_device.physical_device.properties;
        m_device                     = vkb_device.device;

        // 获取图形队列
        m_graphics_queue        = vkb_device.get_queue(vkb::QueueType::graphics).value();
        m_graphics_queue_family = vkb_device.get_queue_index(vkb::QueueType::graphics).value();

        // 初始化内存分配器
        VmaAllocatorCreateInfo allocator_info{};
        allocator_info.physicalDevice = m_physical_device;
        allocator_info.device         = m_device;
        allocator_info.instance       = m_instance;
        vmaCreateAllocator(&allocator_info, &m_allocator);

        // 加入销毁队列
        m_main_deletion_queue.PushFunction(
            [=]()
            {
                vmaDestroyAllocator(m_allocator);
            });
    }

    void VulkanRHI::InitCommands()
    {
        // 为并行的每一帧分别创建指令池
        VkCommandPoolCreateInfo cmd_pool_info = vkinit::CommandPoolCreateInfo(m_graphics_queue_family, VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);

        for (int i = 0; i < FRAME_OVERLAP; i++)
        {
            YUTREL_ASSERT(vkCreateCommandPool(m_device, &cmd_pool_info, nullptr, &m_frames[i].command_pool) == VK_SUCCESS, "Failed to create command pool");

            // 分配指令缓冲
            auto cmd_alloc_info = vkinit::CommandBufferAllocateInfo(m_frames[i].command_pool, 1, VK_COMMAND_BUFFER_LEVEL_PRIMARY);

            YUTREL_ASSERT(vkAllocateCommandBuffers(m_device, &cmd_alloc_info, &m_frames[i].main_command_buffer) == VK_SUCCESS, "Failed to allocate command buffer");

            // 放入删除队列
            m_main_deletion_queue.PushFunction(
                [=]()
                {
                    vkDestroyCommandPool(m_device, m_frames[i].command_pool, nullptr);
                });
        }

        // 创建单次指令指令池
        VkCommandPoolCreateInfo upload_command_pool_info = vkinit::CommandPoolCreateInfo(m_graphics_queue_family, 0);

        YUTREL_ASSERT(vkCreateCommandPool(m_device, &upload_command_pool_info, nullptr, &m_rhi_command_pool) == VK_SUCCESS, "Failed to create upload command pool");

        m_main_deletion_queue.PushFunction(
            [=]()
            {
                vkDestroyCommandPool(m_device, m_rhi_command_pool, nullptr);
            });
    }

    void VulkanRHI::InitDescriptorPool()
    {
        // 创建描述符池
        // todo 控制描述符池大小
        std::vector<VkDescriptorPoolSize> sizes{
            // 支持十个uniform buffer
            {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 10},
            // 支持十个dynamic uniform buffer
            {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 10},
            // 支持十个SSBO
            {VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 10},
            // image采样
            {VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 10},
        };

        VkDescriptorPoolCreateInfo pool_create_info{};
        pool_create_info.sType         = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        pool_create_info.flags         = 0;
        pool_create_info.maxSets       = 10;
        pool_create_info.poolSizeCount = static_cast<uint32_t>(sizes.size());
        pool_create_info.pPoolSizes    = sizes.data();

        YUTREL_ASSERT(vkCreateDescriptorPool(m_device, &pool_create_info, nullptr, &m_descriptor_pool) == VK_SUCCESS, "Failed to create descriptor pool");

        // 加入删除队列
        m_main_deletion_queue.PushFunction(
            [=]()
            {
                vkDestroyDescriptorPool(m_device, m_descriptor_pool, nullptr);
            });
    }

    void VulkanRHI::InitSyncStructures()
    {
        // 同步设施创建信息
        VkFenceCreateInfo fence_create_info         = vkinit::FenceCreateInfo(VK_FENCE_CREATE_SIGNALED_BIT);
        VkSemaphoreCreateInfo semaphore_create_info = vkinit::SemaphoreCreateInfo(0);

        // 为并行的每一帧创建
        for (int i = 0; i < FRAME_OVERLAP; i++)
        {
            YUTREL_ASSERT(vkCreateFence(m_device, &fence_create_info, nullptr, &m_frames[i].render_fence) == VK_SUCCESS, "Failed to create fence");

            // 放入删除队列
            m_main_deletion_queue.PushFunction(
                [=]()
                {
                    vkDestroyFence(m_device, m_frames[i].render_fence, nullptr);
                });

            YUTREL_ASSERT(vkCreateSemaphore(m_device, &semaphore_create_info, nullptr, &m_frames[i].finished_for_presentation_semaphore) == VK_SUCCESS, "Failed to create semaphore");
            YUTREL_ASSERT(vkCreateSemaphore(m_device, &semaphore_create_info, nullptr, &m_frames[i].available_for_render_semaphore) == VK_SUCCESS, "Failed to create semaphore");

            // 放入删除队列
            m_main_deletion_queue.PushFunction(
                [=]()
                {
                    vkDestroySemaphore(m_device, m_frames[i].finished_for_presentation_semaphore, nullptr);
                    vkDestroySemaphore(m_device, m_frames[i].available_for_render_semaphore, nullptr);
                });
        }
    }

    void VulkanRHI::InitSwapchain()
    {
        // vkb创建交换链
        vkb::SwapchainBuilder swapchain_builder{m_physical_device, m_device, m_surface};

        vkb::Swapchain vkb_swapchain =
            swapchain_builder
                .use_default_format_selection()
                .set_desired_present_mode(VK_PRESENT_MODE_MAILBOX_KHR)
                .set_desired_extent(m_swapchain_extent.width, m_swapchain_extent.height)
                .add_image_usage_flags(VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT)
                // .add_image_usage_flags(VK_IMAGE_USAGE_TRANSFER_DST_BIT)
                .build()
                .value();

        // 获取交换链和图像
        m_swapchain              = vkb_swapchain.swapchain;
        m_swapchain_image_format = vkb_swapchain.image_format;
        m_swapchain_images       = vkb_swapchain.get_images().value();
        m_swapchain_image_views  = vkb_swapchain.get_image_views().value();

        // 放入删除队列
        m_main_deletion_queue.PushFunction(
            [=]()
            {
                vkDestroySwapchainKHR(m_device, m_swapchain, nullptr);

                for (int i = 0; i < m_swapchain_image_views.size(); i++)
                {
                    vkDestroyImageView(m_device, m_swapchain_image_views[i], nullptr);
                }
            });
    }

    void VulkanRHI::InitDepthImage()
    {
        m_depth_format = VK_FORMAT_D32_SFLOAT;

        m_depth_image =
            vkutil::CreateImage(m_allocator,
                                m_swapchain_extent.width,
                                m_swapchain_extent.height,
                                m_depth_format,
                                VK_IMAGE_TILING_OPTIMAL,
                                VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT | VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT,
                                VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                                0,
                                1,
                                1);

        m_depth_image_view = vkutil::CreateImageView(m_device, m_depth_image.image, m_depth_format, VK_IMAGE_ASPECT_DEPTH_BIT, VK_IMAGE_VIEW_TYPE_2D, 1, 1);

        // 加入删除队列
        m_main_deletion_queue.PushFunction(
            [=]()
            {
                vkDestroyImageView(m_device, m_depth_image_view, nullptr);
                vmaDestroyImage(m_allocator, m_depth_image.image, m_depth_image.allocation);
            });
    }

    RHISwapChainDesc VulkanRHI::GetSwapChainInfo()
    {
        RHISwapChainDesc desc{};
        desc.extent       = m_swapchain_extent;
        desc.image_format = m_swapchain_image_format;
        desc.viewport     = m_viewport;
        desc.scissor      = m_scissor;
        desc.image_views  = &m_swapchain_image_views;

        return desc;
    }

    bool VulkanRHI::CreateRenderPass(const VkRenderPassCreateInfo* info, VkRenderPass* out_render_pass)
    {
        VkRenderPass render_pass;
        bool result = vkCreateRenderPass(m_device, info, nullptr, &render_pass) == VK_SUCCESS;

        m_main_deletion_queue.PushFunction(
            [=]()
            {
                vkDestroyRenderPass(m_device, render_pass, nullptr);
            });

        *out_render_pass = render_pass;
        return result;
    }

    bool VulkanRHI::CreateFramebuffer(const VkFramebufferCreateInfo* info, VkFramebuffer* out_framebuffer)
    {
        VkFramebuffer framebuffer;
        bool result = vkCreateFramebuffer(m_device, info, nullptr, &framebuffer) == VK_SUCCESS;

        m_main_deletion_queue.PushFunction(
            [=]()
            {
                vkDestroyFramebuffer(m_device, framebuffer, nullptr);
            });

        *out_framebuffer = framebuffer;
        return result;
    }

    bool VulkanRHI::CreateShaderModule(const std::vector<unsigned char>& shader_code, VkShaderModule* out_shader_module)
    {
        VkShaderModuleCreateInfo create_info{};
        create_info.sType    = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        create_info.pNext    = nullptr;
        create_info.codeSize = shader_code.size();
        create_info.pCode    = reinterpret_cast<const uint32_t*>(shader_code.data());

        VkShaderModule shader_module;
        bool result = vkCreateShaderModule(m_device, &create_info, nullptr, &shader_module) == VK_SUCCESS;

        *out_shader_module = shader_module;
        return result;
    }

    void VulkanRHI::DestroyShaderModule(VkShaderModule shader)
    {
        vkDestroyShaderModule(m_device, shader, nullptr);
    }

    bool VulkanRHI::CreatePipelineLayout(const VkPipelineLayoutCreateInfo* info, VkPipelineLayout* out_layout)
    {
        VkPipelineLayout layout;
        bool result = vkCreatePipelineLayout(m_device, info, nullptr, &layout) == VK_SUCCESS;

        m_main_deletion_queue.PushFunction(
            [=]()
            {
                vkDestroyPipelineLayout(m_device, layout, nullptr);
            });

        *out_layout = layout;
        return result;
    }

    bool VulkanRHI::CreateGraphicsPipeline(const RHIGraphicsPipelineCreateInfo& info, VkPipeline* out_pipeline)
    {
        VkPipeline pipeline;

        VkGraphicsPipelineCreateInfo pipeline_info{};
        pipeline_info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
        pipeline_info.pNext = nullptr;

        pipeline_info.stageCount          = info.shader_stages.size();
        pipeline_info.pStages             = info.shader_stages.data();
        pipeline_info.pVertexInputState   = &info.vertex_input;
        pipeline_info.pInputAssemblyState = &info.input_assembly;
        pipeline_info.pViewportState      = &info.viewport_state;
        pipeline_info.pRasterizationState = &info.rasterizer;
        pipeline_info.pMultisampleState   = &info.multisampling;
        pipeline_info.pColorBlendState    = &info.color_blend;
        pipeline_info.layout              = info.pipeline_layout;
        pipeline_info.renderPass          = info.render_pass;
        pipeline_info.subpass             = 0;
        pipeline_info.basePipelineHandle  = VK_NULL_HANDLE;

        bool result = vkCreateGraphicsPipelines(m_device, VK_NULL_HANDLE, 1, &pipeline_info, nullptr, &pipeline) == VK_SUCCESS;

        m_main_deletion_queue.PushFunction(
            [=]()
            {
                vkDestroyPipeline(m_device, pipeline, nullptr);
            });

        *out_pipeline = pipeline;
        return result;
    }

    void VulkanRHI::UploadMesh(Mesh& mesh)
    {
        mesh.vertex_buffer = CreateRef<AllocatedBuffer>();

        // 创建缓冲区
        VkBufferCreateInfo buffer_info{};
        buffer_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        buffer_info.size  = mesh.vertices->size() * sizeof(Vertex);
        buffer_info.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;

        // 内存由CPU写入，GPU读取
        VmaAllocationCreateInfo vma_alloc_info{};
        vma_alloc_info.usage = VMA_MEMORY_USAGE_CPU_TO_GPU;

        // 内存分配
        YUTREL_ASSERT(vmaCreateBuffer(m_allocator, &buffer_info, &vma_alloc_info, &mesh.vertex_buffer->buffer, &mesh.vertex_buffer->allocation, nullptr) == VK_SUCCESS, "Failed to create buffer");

        // 加入销毁队列
        m_main_deletion_queue.PushFunction(
            [=]()
            {
                vmaDestroyBuffer(m_allocator, mesh.vertex_buffer->buffer, mesh.vertex_buffer->allocation);
            });

        // 将数据拷贝到缓冲区
        void* data;
        vmaMapMemory(m_allocator, mesh.vertex_buffer->allocation, &data);
        memcpy(data, mesh.vertices->data(), mesh.vertices->size() * sizeof(Vertex));
        vmaUnmapMemory(m_allocator, mesh.vertex_buffer->allocation);
    }

} // namespace Yutrel