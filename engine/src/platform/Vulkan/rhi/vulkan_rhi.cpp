#include "yutrel_pch.hpp"

#include "vulkan_rhi.hpp"

#include "platform/Vulkan/initializers/initializers.hpp"
#include "platform/Vulkan/mesh/vulkan_mesh.hpp"
#include "platform/Vulkan/utils/vulkan_utils.hpp"

#include <GLFW/glfw3.h>
#include <VKBootstrap.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_vulkan.h>

namespace Yutrel
{
    void VulkanRHI::Init(RHIInitInfo info)
    {
        InitVulkan(info.raw_window);

        InitSwapchain(info.width, info.height);

        InitCommands();

        InitSyncStructures();

        InitDescriptorPool();

        InitImgui(info.raw_window);
    }

    void VulkanRHI::Clear()
    {
        // 确保GPU已经工作完
        vkDeviceWaitIdle(m_device);

        // 删除队列清空
        for (int i = 0; i < FRAME_OVERLAP; i++)
        {
            m_frames[i].deletion_queue.flush();
        }

        m_main_deletion_queue.flush();

        // 销毁交换链
        DestroySwapchain();

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

        // 重设fence
        YUTREL_ASSERT(vkResetFences(m_device, 1, &GetCurrentFrame().render_fence) == VK_SUCCESS, "Failed to reset fence");
    }

    void VulkanRHI::ResetCommandPool()
    {
        YUTREL_ASSERT(vkResetCommandPool(m_device, GetCurrentFrame().command_pool, 0) == VK_SUCCESS, "Failed to reset command pool");
    }

    void VulkanRHI::PrepareBeforePass()
    {
        // 等待 fence
        YUTREL_ASSERT(vkWaitForFences(m_device, 1, &GetCurrentFrame().render_fence, VK_TRUE, UINT64_MAX) == VK_SUCCESS, "Failed to synchronize");

        // 清除为单独帧创建的对象
        GetCurrentFrame().deletion_queue.flush();
        GetCurrentFrame().descriptors.ClearPools();

        // 请求图像索引
        auto result = vkAcquireNextImageKHR(m_device, m_swapchain, UINT64_MAX, GetCurrentFrame().available_for_render_semaphore, VK_NULL_HANDLE, &m_cur_swapchain_image_index);
        // 若窗口大小发生改变
        if (result == VK_ERROR_OUT_OF_DATE_KHR)
        {
            m_resize_requested = true;
            return;
        }

        // 重设fence
        YUTREL_ASSERT(vkResetFences(m_device, 1, &GetCurrentFrame().render_fence) == VK_SUCCESS, "Failed to reset fence");

        // 重置命令缓冲
        YUTREL_ASSERT(vkResetCommandBuffer(m_cur_command_buffer, 0) == VK_SUCCESS, "Failed to reset command buffer");

        // 开始指令缓冲
        VkCommandBufferBeginInfo cmd_begin_info = vkinit::CommandBufferBeginInfo(0);

        YUTREL_ASSERT(vkBeginCommandBuffer(m_cur_command_buffer, &cmd_begin_info) == VK_SUCCESS, "Failed to begin command buffer");
    }

    void VulkanRHI::SubmitRendering()
    {
        //-------------终止指令缓冲-------------
        YUTREL_ASSERT(vkEndCommandBuffer(m_cur_command_buffer) == VK_SUCCESS, "Failed to end command buffer");

        // --------------提交指令---------------
        VkCommandBufferSubmitInfo cmd_info = vkinit::CommandBufferSubmitInfo(m_cur_command_buffer);

        VkSemaphoreSubmitInfo wait_info   = vkinit::SemaphoreSubmitInfo(VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT_KHR, GetCurrentFrame().available_for_render_semaphore);
        VkSemaphoreSubmitInfo signal_info = vkinit::SemaphoreSubmitInfo(VK_PIPELINE_STAGE_2_ALL_GRAPHICS_BIT, GetCurrentFrame().finished_for_presentation_semaphore);

        VkSubmitInfo2 submit_info = vkinit::SubmitInfo2(&cmd_info, &signal_info, &wait_info);

        // 提交到队列
        YUTREL_ASSERT(vkQueueSubmit2(m_graphics_queue, 1, &submit_info, GetCurrentFrame().render_fence) == VK_SUCCESS, "Failed to submit command");

        //--------------显示图像------------
        VkPresentInfoKHR present_info = vkinit::PresentInfo();

        present_info.swapchainCount     = 1;
        present_info.pSwapchains        = &m_swapchain;
        present_info.waitSemaphoreCount = 1;
        present_info.pWaitSemaphores    = &GetCurrentFrame().finished_for_presentation_semaphore;
        present_info.pImageIndices      = &m_cur_swapchain_image_index;

        auto result = vkQueuePresentKHR(m_graphics_queue, &present_info);
        if (result == VK_ERROR_OUT_OF_DATE_KHR)
        {
            m_resize_requested = true;
        }

        m_cur_frame++;
    }

    void VulkanRHI::InitVulkan(GLFWwindow* raw_window)
    {
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

        // vulkan 1.3 特性
        VkPhysicalDeviceVulkan13Features features_13{};
        features_13.dynamicRendering = true;
        features_13.synchronization2 = true;

        // vulkan 1.2 特性
        VkPhysicalDeviceVulkan12Features features_12{};
        features_12.bufferDeviceAddress = true;
        features_12.descriptorIndexing  = true;

        // 选择物理设备
        vkb::PhysicalDeviceSelector selector{vkb_instance};
        vkb::PhysicalDevice physical_device =
            selector
                .set_minimum_version(1, 3)
                .set_required_features_13(features_13)
                .set_required_features_12(features_12)
                .set_surface(m_surface)
                .select()
                .value();

        // 创建逻辑设备
        vkb::DeviceBuilder device_builder{physical_device};

        // todo 以后开启
        // // 启用shader draw parameters功能
        // VkPhysicalDeviceShaderDrawParametersFeatures shader_draw_parameters_features{};
        // shader_draw_parameters_features.sType                = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_DRAW_PARAMETER_FEATURES;
        // shader_draw_parameters_features.pNext                = nullptr;
        // shader_draw_parameters_features.shaderDrawParameters = VK_TRUE;

        vkb::Device vkb_device = device_builder
                                     //  .add_pNext(&shader_draw_parameters_features)
                                     .build()
                                     .value();

        // 获取设备
        m_physical_device = physical_device.physical_device;
        m_device          = vkb_device.device;

        // 获取图形队列
        m_graphics_queue        = vkb_device.get_queue(vkb::QueueType::graphics).value();
        m_graphics_queue_family = vkb_device.get_queue_index(vkb::QueueType::graphics).value();

        // 初始化内存分配器
        VmaAllocatorCreateInfo allocator_info{};
        allocator_info.physicalDevice = m_physical_device;
        allocator_info.device         = m_device;
        allocator_info.instance       = m_instance;
        allocator_info.flags          = VMA_ALLOCATOR_CREATE_BUFFER_DEVICE_ADDRESS_BIT;
        vmaCreateAllocator(&allocator_info, &m_allocator);

        // 加入销毁队列
        m_main_deletion_queue.PushFunction(
            [&]()
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
        //--------创建描述符池-----------
        // todo 控制描述符池大小
        std::vector<VkDescriptorPoolSize> sizes{
            {VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 10},
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

        //------------创建每帧动态分配的描述符池---------
        for (int i = 0; i < FRAME_OVERLAP; i++)
        {
            std::vector<DescriptorAllocator::PoolSizeRatio> frame_sizes = {
                {VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 3},
                {VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 3},
                {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 3},
                {VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 4},
            };

            m_frames[i].descriptors = DescriptorAllocator{};
            m_frames[i].descriptors.Init(this, 1000, frame_sizes);

            m_main_deletion_queue.PushFunction(
                [&, i]()
                {
                    m_frames[i].descriptors.DestroyPools();
                });
        }
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

    void VulkanRHI::InitSwapchain(uint32_t width, uint32_t height)
    {
        //---------vkb创建交换链-----------
        vkb::SwapchainBuilder swapchain_builder{m_physical_device, m_device, m_surface};

        vkb::Swapchain vkb_swapchain =
            swapchain_builder
                .use_default_format_selection()
                .set_desired_present_mode(VK_PRESENT_MODE_MAILBOX_KHR)
                .set_desired_extent(width, height)
                .add_image_usage_flags(VK_IMAGE_USAGE_TRANSFER_DST_BIT)
                .build()
                .value();

        // 记录交换范围大小
        m_swapchain_extent = vkb_swapchain.extent;

        // 获取交换链和图像
        m_swapchain              = vkb_swapchain.swapchain;
        m_swapchain_image_format = vkb_swapchain.image_format;
        m_swapchain_images       = vkb_swapchain.get_images().value();
        m_swapchain_image_views  = vkb_swapchain.get_image_views().value();
    }

    void VulkanRHI::InitImgui(GLFWwindow* raw_window)
    {
        std::vector<VkDescriptorPoolSize> pool_sizes = {
            {VK_DESCRIPTOR_TYPE_SAMPLER, 1000},
            {VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000},
            {VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000},
            {VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000},
            {VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000},
            {VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000},
            {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000},
            {VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000},
            {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000},
            {VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000},
            {VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000},
        };

        VkDescriptorPoolCreateInfo pool_info{};
        pool_info.sType         = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        pool_info.flags         = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
        pool_info.maxSets       = 1000;
        pool_info.poolSizeCount = (uint32_t)std::size(pool_sizes);
        pool_info.pPoolSizes    = pool_sizes.data();

        VkDescriptorPool imgui_pool;
        YUTREL_ASSERT(vkCreateDescriptorPool(m_device, &pool_info, nullptr, &imgui_pool) == VK_SUCCESS, "Failed to create descriptor pool");

        //---------初始化imgui-----------
        ImGui::CreateContext();

        ImGui_ImplGlfw_InitForVulkan(raw_window, true);

        ImGui_ImplVulkan_InitInfo init_info{};
        init_info.Instance              = m_instance;
        init_info.PhysicalDevice        = m_physical_device;
        init_info.Device                = m_device;
        init_info.Queue                 = m_graphics_queue;
        init_info.DescriptorPool        = imgui_pool;
        init_info.MinImageCount         = 3;
        init_info.ImageCount            = 3;
        init_info.UseDynamicRendering   = true;
        init_info.ColorAttachmentFormat = m_swapchain_image_format;
        init_info.MSAASamples           = VK_SAMPLE_COUNT_1_BIT;

        ImGui_ImplVulkan_Init(&init_info, VK_NULL_HANDLE);

        ImGui_ImplVulkan_CreateFontsTexture();
        ImGui_ImplVulkan_DestroyFontsTexture();

        m_main_deletion_queue.PushFunction(
            [=]()
            {
                ImGui_ImplVulkan_Shutdown();
                vkDestroyDescriptorPool(m_device, imgui_pool, nullptr);
            });
    }

    RHISwapChainDesc VulkanRHI::GetSwapChainInfo()
    {
        RHISwapChainDesc desc{};
        desc.extent       = m_swapchain_extent;
        desc.image_format = m_swapchain_image_format;
        desc.image_views  = &m_swapchain_image_views;

        return desc;
    }

    void VulkanRHI::CreateRenderPass(const VkRenderPassCreateInfo* info, VkRenderPass* out_render_pass)
    {
        VkRenderPass render_pass;
        YUTREL_ASSERT(vkCreateRenderPass(m_device, info, nullptr, &render_pass) == VK_SUCCESS, "Failed to create render pass");

        m_main_deletion_queue.PushFunction(
            [=]()
            {
                vkDestroyRenderPass(m_device, render_pass, nullptr);
            });

        *out_render_pass = render_pass;
    }

    void VulkanRHI::CreateFramebuffer(const VkFramebufferCreateInfo* info, VkFramebuffer* out_framebuffer)
    {
        VkFramebuffer framebuffer;
        YUTREL_ASSERT(vkCreateFramebuffer(m_device, info, nullptr, &framebuffer) == VK_SUCCESS, "Failed to create framebuffer");

        m_main_deletion_queue.PushFunction(
            [=]()
            {
                vkDestroyFramebuffer(m_device, framebuffer, nullptr);
            });

        *out_framebuffer = framebuffer;
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

    void VulkanRHI::CreatePipelineLayout(const VkPipelineLayoutCreateInfo* info, VkPipelineLayout* out_layout)
    {
        VkPipelineLayout layout;
        YUTREL_ASSERT(vkCreatePipelineLayout(m_device, info, nullptr, &layout) == VK_SUCCESS, "Failed to create pipeline layout");

        m_main_deletion_queue.PushFunction(
            [=]()
            {
                vkDestroyPipelineLayout(m_device, layout, nullptr);
            });

        *out_layout = layout;
    }

    void VulkanRHI::CreateGraphicsPipelines(const RHIGraphicsPipelineCreateInfo& info, VkPipeline* out_pipeline)
    {
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

        VkPipeline pipeline;
        YUTREL_ASSERT(vkCreateGraphicsPipelines(m_device, VK_NULL_HANDLE, 1, &pipeline_info, nullptr, &pipeline) == VK_SUCCESS, "Failed to create graphics pipelines");

        m_main_deletion_queue.PushFunction(
            [=]()
            {
                vkDestroyPipeline(m_device, pipeline, nullptr);
            });

        *out_pipeline = pipeline;
    }

    void VulkanRHI::CreateComputePipelines(VkPipelineCache pipelineCache,
                                           uint32_t createInfoCount,
                                           const VkComputePipelineCreateInfo* pCreateInfos,
                                           const VkAllocationCallbacks* pAllocator,
                                           VkPipeline* pPipelines)
    {
        VkPipeline pipeline;
        YUTREL_ASSERT(vkCreateComputePipelines(m_device, pipelineCache, createInfoCount, pCreateInfos, pAllocator, &pipeline) == VK_SUCCESS, "Failed to create compute pipelines");

        m_main_deletion_queue.PushFunction(
            [=]()
            {
                vkDestroyPipeline(m_device, pipeline, nullptr);
            });

        *pPipelines = pipeline;
    }

    void VulkanRHI::CreateDynamicPipelines(const RHIDynamicPipelineCreateInfo& info, VkPipeline* out_pipeline)
    {
        // 视口和裁剪
        VkPipelineViewportStateCreateInfo viewport_state{};
        viewport_state.sType         = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
        viewport_state.pNext         = nullptr;
        viewport_state.viewportCount = 1;
        viewport_state.scissorCount  = 1;

        // 颜色混合
        VkPipelineColorBlendStateCreateInfo color_blending{};
        color_blending.sType           = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
        color_blending.pNext           = nullptr;
        color_blending.logicOpEnable   = VK_FALSE;
        color_blending.logicOp         = VK_LOGIC_OP_COPY;
        color_blending.attachmentCount = 1;
        color_blending.pAttachments    = &info.color_blend_attachment;

        // 顶点阶段
        // 不需要
        VkPipelineVertexInputStateCreateInfo vertex_input_info{};
        vertex_input_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
        vertex_input_info.pNext = nullptr;

        // 动态状态
        std::array<VkDynamicState, 2> state =
            {
                VK_DYNAMIC_STATE_VIEWPORT,
                VK_DYNAMIC_STATE_SCISSOR,
            };
        VkPipelineDynamicStateCreateInfo dynamic_info{};
        dynamic_info.sType             = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
        dynamic_info.dynamicStateCount = static_cast<uint32_t>(state.size());
        dynamic_info.pDynamicStates    = state.data();

        //---------管线创建信息-------------
        VkGraphicsPipelineCreateInfo pipeline_info{};
        pipeline_info.sType               = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
        pipeline_info.pNext               = &info.render_info;
        pipeline_info.stageCount          = static_cast<uint32_t>(info.shader_stages.size());
        pipeline_info.pStages             = info.shader_stages.data();
        pipeline_info.pVertexInputState   = &vertex_input_info;
        pipeline_info.pInputAssemblyState = &info.input_assembly;
        pipeline_info.pViewportState      = &viewport_state;
        pipeline_info.pRasterizationState = &info.rasterizer;
        pipeline_info.pMultisampleState   = &info.multisampling;
        pipeline_info.pColorBlendState    = &color_blending;
        pipeline_info.pDepthStencilState  = &info.depth_stencil;
        pipeline_info.layout              = info.pipeline_layout;
        pipeline_info.pDynamicState       = &dynamic_info;

        VkPipeline pipeline;
        YUTREL_ASSERT(vkCreateGraphicsPipelines(m_device, VK_NULL_HANDLE, 1, &pipeline_info, nullptr, &pipeline) == VK_SUCCESS, "Failed to create dynamic pipelines");

        m_main_deletion_queue.PushFunction(
            [=]()
            {
                vkDestroyPipeline(m_device, pipeline, nullptr);
            });

        *out_pipeline = pipeline;
    }

    void VulkanRHI::CreateImage(const VkImageCreateInfo* create_info, const VmaAllocationCreateInfo* alloc_info, AllocatedImage* out_image)
    {
        VkImage image;
        VmaAllocation allocation;
        YUTREL_ASSERT(vmaCreateImage(m_allocator, create_info, alloc_info, &image, &allocation, nullptr) == VK_SUCCESS, "Failed to create image");

        m_main_deletion_queue.PushFunction(
            [=]()
            {
                vmaDestroyImage(m_allocator, image, allocation);
            });

        out_image->image      = image;
        out_image->allocation = allocation;
    }

    void VulkanRHI::CreateImageView(const VkImageViewCreateInfo* info, AllocatedImage* out_image)
    {
        VkImageView image_view;
        YUTREL_ASSERT(vkCreateImageView(m_device, info, nullptr, &image_view) == VK_SUCCESS, "Failed to create image view");

        m_main_deletion_queue.PushFunction(
            [=]()
            {
                vkDestroyImageView(m_device, image_view, nullptr);
            });

        out_image->image_view = image_view;
    }

    void VulkanRHI::CreateDescriptorLayout(RHIDescriptorLayoutCreateInfo& info, VkDescriptorSetLayout* out_layout)
    {
        for (auto& b : info.bindings)
        {
            b.stageFlags |= info.shader_stages;
        }

        VkDescriptorSetLayoutCreateInfo create_info{};
        create_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        create_info.pNext = nullptr;

        create_info.pBindings    = info.bindings.data();
        create_info.bindingCount = static_cast<uint32_t>(info.bindings.size());
        create_info.flags        = 0;

        VkDescriptorSetLayout layout;
        YUTREL_ASSERT(vkCreateDescriptorSetLayout(m_device, &create_info, nullptr, &layout) == VK_SUCCESS, "Failed to create descriptor set layout");

        m_main_deletion_queue.PushFunction(
            [=]()
            {
                vkDestroyDescriptorSetLayout(m_device, layout, nullptr);
            });

        *out_layout = layout;
    }

    void VulkanRHI::CreateDescriptorPool(const VkDescriptorPoolCreateInfo* info, VkDescriptorPool* out_pool)
    {
        VkDescriptorPool pool;
        YUTREL_ASSERT(vkCreateDescriptorPool(m_device, info, nullptr, &pool) == VK_SUCCESS, "Failed to create descriptor pool");

        // 暂时不用加入删除队列，因为描述符池有自己的删除函数
        // m_main_deletion_queue.PushFunction(
        //     [=]()
        //     {
        //         vkDestroyDescriptorPool(m_device, pool, nullptr);
        //     });

        *out_pool = pool;
    }

    void VulkanRHI::ResetDescriptorPool(VkDescriptorPool pool)
    {
        vkResetDescriptorPool(m_device, pool, 0);
    }

    void VulkanRHI::DestroyDescriptorPool(VkDescriptorPool pool)
    {
        vkDestroyDescriptorPool(m_device, pool, nullptr);
    }

    void VulkanRHI::AllocateDescriptorSets(VkDescriptorSetLayout layout, VkDescriptorSet* out_set)
    {
        VkDescriptorSetAllocateInfo info{};
        info.sType              = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        info.pNext              = nullptr;
        info.descriptorPool     = m_descriptor_pool;
        info.descriptorSetCount = 1;
        info.pSetLayouts        = &layout;

        VkDescriptorSet set;
        YUTREL_ASSERT(vkAllocateDescriptorSets(m_device, &info, &set) == VK_SUCCESS, "Failed to allocate descriptor sets");

        *out_set = set;
    }

    VkResult VulkanRHI::AllocateDescriptorSets(const VkDescriptorSetAllocateInfo* info, VkDescriptorSet* out_set)
    {
        VkDescriptorSet set;
        auto result = vkAllocateDescriptorSets(m_device, info, &set);

        *out_set = set;
        return result;
    }

    void VulkanRHI::UpdateDescriptorSets(uint32_t descriptor_write_count, const VkWriteDescriptorSet* p_descriptor_writes, uint32_t descriptor_copy_count, const VkCopyDescriptorSet* p_descriptor_copies)
    {
        vkUpdateDescriptorSets(m_device, descriptor_write_count, p_descriptor_writes, descriptor_copy_count, p_descriptor_copies);
    }

    void VulkanRHI::UpdateDescriptorSets(DescriptorWriter& writer, VkDescriptorSet set)
    {
        for (VkWriteDescriptorSet& write : writer.writes)
        {
            write.dstSet = set;
        }
        vkUpdateDescriptorSets(m_device, static_cast<uint32_t>(writer.writes.size()), writer.writes.data(), 0, nullptr);
    }

    VkCommandBuffer VulkanRHI::BeginSingleTimeCommands()
    {
        VkCommandBufferAllocateInfo alloc_info = vkinit::CommandBufferAllocateInfo(m_rhi_command_pool, 1, VK_COMMAND_BUFFER_LEVEL_PRIMARY);

        VkCommandBuffer cmd_buffer;
        vkAllocateCommandBuffers(m_device, &alloc_info, &cmd_buffer);

        VkCommandBufferBeginInfo beginInfo = vkinit::CommandBufferBeginInfo(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

        YUTREL_ASSERT(vkBeginCommandBuffer(cmd_buffer, &beginInfo) == VK_SUCCESS, "Failed to begin single time command buffer");

        return cmd_buffer;
    }

    void VulkanRHI::EndSingleTimeCommands(VkCommandBuffer cmd_buffer)
    {
        YUTREL_ASSERT(vkEndCommandBuffer(cmd_buffer) == VK_SUCCESS, "Failed to end signle time command buffer");

        VkCommandBufferSubmitInfo cmd_info = vkinit::CommandBufferSubmitInfo(cmd_buffer);
        VkSubmitInfo2 submit               = vkinit::SubmitInfo2(&cmd_info, nullptr, nullptr);

        vkQueueSubmit2(m_graphics_queue, 1, &submit, VK_NULL_HANDLE);
        vkQueueWaitIdle(m_graphics_queue);

        vkFreeCommandBuffers(m_device, m_rhi_command_pool, 1, &cmd_buffer);
    }

    AllocatedBuffer VulkanRHI::CreateBuffer(size_t alloc_size, VkBufferUsageFlags usage_flags, VmaMemoryUsage usage)
    {
        VkBufferCreateInfo bufferInfo{};
        bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        bufferInfo.pNext = nullptr;
        bufferInfo.size  = alloc_size;

        bufferInfo.usage = usage_flags;

        VmaAllocationCreateInfo vmaallocInfo{};
        vmaallocInfo.usage = usage;
        vmaallocInfo.flags = VMA_ALLOCATION_CREATE_MAPPED_BIT;

        AllocatedBuffer new_buffer{};
        YUTREL_ASSERT(vmaCreateBuffer(m_allocator, &bufferInfo, &vmaallocInfo, &new_buffer.buffer, &new_buffer.allocation, &new_buffer.info) == VK_SUCCESS, "Failed to create buffer");

        return new_buffer;
    }

    void VulkanRHI::DestroyBuffer(const AllocatedBuffer& buffer)
    {
        vmaDestroyBuffer(m_allocator, buffer.buffer, buffer.allocation);
    }

    void VulkanRHI::UploadMesh(Mesh& mesh)
    {
        const size_t VERTEX_BUFFER_SIZE = mesh.vertices->size() * sizeof(Vertex);
        const size_t INDEX_BUFFER_SIZE  = mesh.indices->size() * sizeof(uint32_t);

        mesh.gpu_buffers = CreateRef<GPUMeshBuffers>();

        // 顶点缓冲
        mesh.gpu_buffers->vertex_buffer =
            CreateBuffer(VERTEX_BUFFER_SIZE,
                         VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT,
                         VMA_MEMORY_USAGE_GPU_ONLY);

        VkBufferDeviceAddressInfo device_address_info{};
        device_address_info.sType  = VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_INFO;
        device_address_info.buffer = mesh.gpu_buffers->vertex_buffer.buffer;

        mesh.gpu_buffers->vertex_buffer_address = vkGetBufferDeviceAddress(m_device, &device_address_info);

        // 索引缓冲
        mesh.gpu_buffers->index_buffer =
            CreateBuffer(INDEX_BUFFER_SIZE,
                         VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
                         VMA_MEMORY_USAGE_GPU_ONLY);

        // 创建暂存缓冲区
        AllocatedBuffer staging =
            CreateBuffer(VERTEX_BUFFER_SIZE + INDEX_BUFFER_SIZE,
                         VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                         VMA_MEMORY_USAGE_CPU_ONLY);

        // 将数据拷贝到暂存缓冲区
        void* data = staging.info.pMappedData;

        memcpy(data, mesh.vertices->data(), VERTEX_BUFFER_SIZE);
        memcpy((char*)data + VERTEX_BUFFER_SIZE, mesh.indices->data(), INDEX_BUFFER_SIZE);

        // 从暂存缓冲区拷贝到gpu
        VkCommandBuffer cmd_buffer = BeginSingleTimeCommands();

        VkBufferCopy vertex_copy{0};
        vertex_copy.dstOffset = 0;
        vertex_copy.srcOffset = 0;
        vertex_copy.size      = VERTEX_BUFFER_SIZE;

        vkCmdCopyBuffer(cmd_buffer, staging.buffer, mesh.gpu_buffers->vertex_buffer.buffer, 1, &vertex_copy);

        VkBufferCopy index_copy{0};
        index_copy.dstOffset = 0;
        index_copy.srcOffset = VERTEX_BUFFER_SIZE;
        index_copy.size      = INDEX_BUFFER_SIZE;

        vkCmdCopyBuffer(cmd_buffer, staging.buffer, mesh.gpu_buffers->index_buffer.buffer, 1, &index_copy);

        EndSingleTimeCommands(cmd_buffer);

        // 顶点缓冲区和索引缓冲区加入销毁队列
        m_main_deletion_queue.PushFunction(
            [=]()
            {
                vmaDestroyBuffer(m_allocator, mesh.gpu_buffers->vertex_buffer.buffer, mesh.gpu_buffers->vertex_buffer.allocation);
                vmaDestroyBuffer(m_allocator, mesh.gpu_buffers->index_buffer.buffer, mesh.gpu_buffers->index_buffer.allocation);
            });

        DestroyBuffer(staging);
    }

    void VulkanRHI::TransitionImage(VkCommandBuffer cmd_buffer, VkImage image, VkImageLayout cur_layout, VkImageLayout new_layout)
    {
        VkImageMemoryBarrier2 imageBarrier{};
        imageBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2;
        imageBarrier.pNext = nullptr;

        imageBarrier.srcStageMask  = VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT;
        imageBarrier.srcAccessMask = VK_ACCESS_2_MEMORY_WRITE_BIT;
        imageBarrier.dstStageMask  = VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT;
        imageBarrier.dstAccessMask = VK_ACCESS_2_MEMORY_WRITE_BIT | VK_ACCESS_2_MEMORY_READ_BIT;

        imageBarrier.oldLayout = cur_layout;
        imageBarrier.newLayout = new_layout;

        VkImageAspectFlags aspectMask = (new_layout == VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL) ? VK_IMAGE_ASPECT_DEPTH_BIT : VK_IMAGE_ASPECT_COLOR_BIT;
        imageBarrier.subresourceRange = vkinit::ImageSubresourceRange(aspectMask);
        imageBarrier.image            = image;

        VkDependencyInfo depInfo{};
        depInfo.sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO;
        depInfo.pNext = nullptr;

        depInfo.imageMemoryBarrierCount = 1;
        depInfo.pImageMemoryBarriers    = &imageBarrier;

        vkCmdPipelineBarrier2(cmd_buffer, &depInfo);
    }

    void VulkanRHI::CopyImageToImage(VkCommandBuffer cmd_buffer, VkImage source, VkImage destination, VkExtent2D src_size, VkExtent2D dst_size)
    {
        VkImageBlit2 blit_region{};
        blit_region.sType = VK_STRUCTURE_TYPE_IMAGE_BLIT_2;
        blit_region.pNext = nullptr;

        blit_region.srcOffsets[1].x = src_size.width;
        blit_region.srcOffsets[1].y = src_size.height;
        blit_region.srcOffsets[1].z = 1;

        blit_region.dstOffsets[1].x = dst_size.width;
        blit_region.dstOffsets[1].y = dst_size.height;
        blit_region.dstOffsets[1].z = 1;

        blit_region.srcSubresource.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
        blit_region.srcSubresource.baseArrayLayer = 0;
        blit_region.srcSubresource.layerCount     = 1;
        blit_region.srcSubresource.mipLevel       = 0;

        blit_region.dstSubresource.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
        blit_region.dstSubresource.baseArrayLayer = 0;
        blit_region.dstSubresource.layerCount     = 1;
        blit_region.dstSubresource.mipLevel       = 0;

        VkBlitImageInfo2 blit_info{};
        blit_info.sType          = VK_STRUCTURE_TYPE_BLIT_IMAGE_INFO_2;
        blit_info.pNext          = nullptr;
        blit_info.dstImage       = destination;
        blit_info.dstImageLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
        blit_info.srcImage       = source;
        blit_info.srcImageLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
        blit_info.filter         = VK_FILTER_LINEAR;
        blit_info.regionCount    = 1;
        blit_info.pRegions       = &blit_region;

        vkCmdBlitImage2(cmd_buffer, &blit_info);
    }

    void VulkanRHI::UpdateSwapchainSize(uint32_t width, uint32_t height)
    {
        m_new_swapchain_extent.width  = width;
        m_new_swapchain_extent.height = height;
    }

    void VulkanRHI::ResizeSwapchain()
    {
        vkDeviceWaitIdle(m_device);

        DestroySwapchain();

        InitSwapchain(m_new_swapchain_extent.width, m_new_swapchain_extent.height);

        m_resize_requested = false;
    }

    void VulkanRHI::DestroySwapchain()
    {
        vkDestroySwapchainKHR(m_device, m_swapchain, nullptr);

        for (int i = 0; i < m_swapchain_image_views.size(); i++)
        {
            vkDestroyImageView(m_device, m_swapchain_image_views[i], nullptr);
        }
    }

} // namespace Yutrel