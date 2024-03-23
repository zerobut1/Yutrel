#include "yutrel_pch.hpp"

#include "vulkan_rhi.hpp"

#include "platform/Vulkan/utils/imgui_style.hpp"

#include <GLFW/glfw3.h>
#include <VKBootstrap.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_vulkan.h>

#include <stdint.h>
#include <tuple>
#include <vcruntime.h>
#include <vector>
#include <vulkan/vulkan_enums.hpp>
#include <vulkan/vulkan_structs.hpp>

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
        m_device.waitIdle();

        // 删除队列清空
        for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
        {
            m_frames[i].deletion_queue.flush();
        }
        m_main_deletion_queue.flush();

        // 销毁交换链
        DestroySwapchain();

        // 销毁内存分配器
        m_allocator.destroy();

        // 销毁窗口表面
        m_instance.destroySurfaceKHR(m_surface);

        // 销毁逻辑设备
        m_device.destroy();

        // 销毁debug messenger
        vkb::destroy_debug_utils_messenger(m_instance, m_debug_messenger);

        // 销毁vulkan实例
        m_instance.destroy();
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
        VkSurfaceKHR surface;
        glfwCreateWindowSurface(m_instance, raw_window, nullptr, &surface);
        m_surface = static_cast<vk::SurfaceKHR>(surface);

        // 设备特性
        vk::PhysicalDeviceFeatures device_features{};
        device_features.samplerAnisotropy = vk::True;

        // vulkan 1.2 特性
        vk::PhysicalDeviceVulkan12Features features_12{};
        features_12.bufferDeviceAddress = vk::True;
        features_12.descriptorIndexing  = vk::True;

        // vulkan 1.3 特性
        vk::PhysicalDeviceVulkan13Features features_13{};
        features_13.dynamicRendering = vk::True;
        features_13.synchronization2 = vk::True;

        // 选择物理设备
        vkb::PhysicalDeviceSelector selector{vkb_instance};
        vkb::PhysicalDevice physical_device =
            selector
                .set_minimum_version(1, 3)
                .set_required_features(device_features)
                .set_required_features_12(features_12)
                .set_required_features_13(features_13)
                .set_surface(m_surface)
                .select()
                .value();

        // 创建逻辑设备
        vkb::DeviceBuilder device_builder{physical_device};

        vkb::Device vkb_device = device_builder
                                     .build()
                                     .value();

        // 获取设备
        m_GPU            = physical_device.physical_device;
        m_GPU_properties = vkb_device.physical_device.properties;
        m_device         = vkb_device.device;

        // 获取图形队列
        m_graphics_queue        = vkb_device.get_queue(vkb::QueueType::graphics).value();
        m_graphics_queue_family = vkb_device.get_queue_index(vkb::QueueType::graphics).value();

        // 初始化内存分配器
        auto allocator_ci =
            vma::AllocatorCreateInfo()
                .setPhysicalDevice(m_GPU)
                .setDevice(m_device)
                .setInstance(m_instance)
                .setFlags(vma::AllocatorCreateFlagBits::eBufferDeviceAddress);

        m_allocator = vma::createAllocator(allocator_ci);
    }

    void VulkanRHI::InitSwapchain(uint32_t width, uint32_t height)
    {
        //---------vkb创建交换链-----------
        vkb::SwapchainBuilder swapchain_builder{m_GPU, m_device, m_surface};

        vkb::Swapchain vkb_swapchain =
            swapchain_builder
                .use_default_format_selection()
                .set_desired_present_mode(VK_PRESENT_MODE_MAILBOX_KHR)
                .set_desired_extent(width, height)
                .add_image_usage_flags(VK_IMAGE_USAGE_TRANSFER_DST_BIT)
                .build()
                .value();

        // 获取交换链和图像
        m_swapchain        = vkb_swapchain.swapchain;
        m_swapchain_extent = vkb_swapchain.extent;
        m_swapchain_format = static_cast<vk::Format>(vkb_swapchain.image_format);
        m_swapchain_images.clear();
        m_swapchain_image_views.clear();
        auto swapchain_images      = vkb_swapchain.get_images().value();
        auto swapchain_image_views = vkb_swapchain.get_image_views().value();
        for (auto image : swapchain_images)
        {
            m_swapchain_images.push_back(static_cast<vk::Image>(image));
        }
        for (auto&& view : swapchain_image_views)
        {
            m_swapchain_image_views.push_back(static_cast<vk::ImageView>(view));
        }
    }

    void VulkanRHI::DestroySwapchain()
    {
        m_device.destroySwapchainKHR(m_swapchain);

        for (auto view : m_swapchain_image_views)
        {
            m_device.destroyImageView(view);
        }
    }

    void VulkanRHI::InitCommands()
    {
        // 为并行的每一帧分别创建指令池
        auto cmd_pool_ci =
            vk::CommandPoolCreateInfo()
                .setQueueFamilyIndex(m_graphics_queue_family)
                .setFlags(vk::CommandPoolCreateFlagBits::eResetCommandBuffer);

        for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
        {
            m_frames[i].cmd_pool = m_device.createCommandPool(cmd_pool_ci);

            // 分配指令缓冲
            auto cmd_buffer_ai =
                vk::CommandBufferAllocateInfo()
                    .setCommandPool(m_frames[i].cmd_pool)
                    .setCommandBufferCount(1)
                    .setLevel(vk::CommandBufferLevel::ePrimary);

            m_frames[i].main_cmd_buffer = m_device.allocateCommandBuffers(cmd_buffer_ai).front();

            // 放入删除队列
            m_main_deletion_queue.PushFunction(
                [=]()
                {
                    m_device.destroyCommandPool(m_frames[i].cmd_pool);
                });
        }

        // 创建单次指令指令池
        cmd_pool_ci.setFlags({});
        m_rhi_cmd_pool = m_device.createCommandPool(cmd_pool_ci);
        m_main_deletion_queue.PushFunction(
            [=]()
            {
                m_device.destroyCommandPool(m_rhi_cmd_pool);
            });
    }

    void VulkanRHI::InitSyncStructures()
    {
        // 同步设施创建信息
        auto fence_ci =
            vk::FenceCreateInfo()
                .setFlags(vk::FenceCreateFlagBits::eSignaled);
        auto semaphore_ci =
            vk::SemaphoreCreateInfo()
                .setFlags({});

        // 为并行的每一帧创建
        for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
        {
            m_frames[i].render_fence = m_device.createFence(fence_ci);

            // 放入删除队列
            m_main_deletion_queue.PushFunction(
                [=]()
                {
                    m_device.destroyFence(m_frames[i].render_fence);
                });

            m_frames[i].finished_for_presentation_semaphore = m_device.createSemaphore(semaphore_ci);
            m_frames[i].available_for_render_semaphore      = m_device.createSemaphore(semaphore_ci);

            // 放入删除队列
            m_main_deletion_queue.PushFunction(
                [=]()
                {
                    m_device.destroySemaphore(m_frames[i].finished_for_presentation_semaphore);
                    m_device.destroySemaphore(m_frames[i].available_for_render_semaphore);
                });
        }
    }

    void VulkanRHI::InitDescriptorPool()
    {
        //--------创建描述符池-----------
        // todo 控制描述符池大小
        std::vector<vk::DescriptorPoolSize> sizes{
            {vk::DescriptorType::eUniformBuffer, 100},
            {vk::DescriptorType::eStorageImage, 100},
            {vk::DescriptorType::eCombinedImageSampler, 100},
        };

        auto pool_ci =
            vk::DescriptorPoolCreateInfo()
                .setFlags({})
                .setMaxSets(100)
                .setPoolSizes(sizes);

        m_descriptor_pool = m_device.createDescriptorPool(pool_ci);

        // 加入删除队列
        m_main_deletion_queue.PushFunction(
            [=]()
            {
                m_device.destroyDescriptorPool(m_descriptor_pool);
            });
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
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();

        ImGuiIO& io = ImGui::GetIO();
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
        io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

        ImGui::StyleColorsDark();

        ImGuiStyle& style                      = ImGui::GetStyle();
        style.GrabRounding                     = 4.0f;
        ImVec4* colors                         = style.Colors;
        colors[ImGuiCol_Text]                  = ImGui::ColorConvertU32ToFloat4(Spectrum::GRAY800);
        colors[ImGuiCol_TextDisabled]          = ImGui::ColorConvertU32ToFloat4(Spectrum::GRAY500);
        colors[ImGuiCol_WindowBg]              = ImGui::ColorConvertU32ToFloat4(Spectrum::GRAY100);
        colors[ImGuiCol_ChildBg]               = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
        colors[ImGuiCol_PopupBg]               = ImGui::ColorConvertU32ToFloat4(Spectrum::GRAY50);
        colors[ImGuiCol_Border]                = ImGui::ColorConvertU32ToFloat4(Spectrum::GRAY300);
        colors[ImGuiCol_BorderShadow]          = ImGui::ColorConvertU32ToFloat4(Spectrum::Static::NONE);
        colors[ImGuiCol_FrameBg]               = ImGui::ColorConvertU32ToFloat4(Spectrum::GRAY75);
        colors[ImGuiCol_FrameBgHovered]        = ImGui::ColorConvertU32ToFloat4(Spectrum::GRAY50);
        colors[ImGuiCol_FrameBgActive]         = ImGui::ColorConvertU32ToFloat4(Spectrum::GRAY200);
        colors[ImGuiCol_TitleBg]               = ImGui::ColorConvertU32ToFloat4(Spectrum::GRAY300);
        colors[ImGuiCol_TitleBgActive]         = ImGui::ColorConvertU32ToFloat4(Spectrum::GRAY200);
        colors[ImGuiCol_TitleBgCollapsed]      = ImGui::ColorConvertU32ToFloat4(Spectrum::GRAY400);
        colors[ImGuiCol_MenuBarBg]             = ImGui::ColorConvertU32ToFloat4(Spectrum::GRAY100);
        colors[ImGuiCol_ScrollbarBg]           = ImGui::ColorConvertU32ToFloat4(Spectrum::GRAY100);
        colors[ImGuiCol_ScrollbarGrab]         = ImGui::ColorConvertU32ToFloat4(Spectrum::GRAY400);
        colors[ImGuiCol_ScrollbarGrabHovered]  = ImGui::ColorConvertU32ToFloat4(Spectrum::GRAY600);
        colors[ImGuiCol_ScrollbarGrabActive]   = ImGui::ColorConvertU32ToFloat4(Spectrum::GRAY700);
        colors[ImGuiCol_CheckMark]             = ImGui::ColorConvertU32ToFloat4(Spectrum::BLUE500);
        colors[ImGuiCol_SliderGrab]            = ImGui::ColorConvertU32ToFloat4(Spectrum::GRAY700);
        colors[ImGuiCol_SliderGrabActive]      = ImGui::ColorConvertU32ToFloat4(Spectrum::GRAY800);
        colors[ImGuiCol_Button]                = ImGui::ColorConvertU32ToFloat4(Spectrum::GRAY75);
        colors[ImGuiCol_ButtonHovered]         = ImGui::ColorConvertU32ToFloat4(Spectrum::GRAY50);
        colors[ImGuiCol_ButtonActive]          = ImGui::ColorConvertU32ToFloat4(Spectrum::GRAY200);
        colors[ImGuiCol_Header]                = ImGui::ColorConvertU32ToFloat4(Spectrum::BLUE400);
        colors[ImGuiCol_HeaderHovered]         = ImGui::ColorConvertU32ToFloat4(Spectrum::BLUE500);
        colors[ImGuiCol_HeaderActive]          = ImGui::ColorConvertU32ToFloat4(Spectrum::BLUE600);
        colors[ImGuiCol_Separator]             = ImGui::ColorConvertU32ToFloat4(Spectrum::GRAY400);
        colors[ImGuiCol_SeparatorHovered]      = ImGui::ColorConvertU32ToFloat4(Spectrum::GRAY600);
        colors[ImGuiCol_SeparatorActive]       = ImGui::ColorConvertU32ToFloat4(Spectrum::GRAY700);
        colors[ImGuiCol_ResizeGrip]            = ImGui::ColorConvertU32ToFloat4(Spectrum::GRAY400);
        colors[ImGuiCol_ResizeGripHovered]     = ImGui::ColorConvertU32ToFloat4(Spectrum::GRAY600);
        colors[ImGuiCol_ResizeGripActive]      = ImGui::ColorConvertU32ToFloat4(Spectrum::GRAY700);
        colors[ImGuiCol_PlotLines]             = ImGui::ColorConvertU32ToFloat4(Spectrum::BLUE400);
        colors[ImGuiCol_PlotLinesHovered]      = ImGui::ColorConvertU32ToFloat4(Spectrum::BLUE600);
        colors[ImGuiCol_PlotHistogram]         = ImGui::ColorConvertU32ToFloat4(Spectrum::BLUE400);
        colors[ImGuiCol_PlotHistogramHovered]  = ImGui::ColorConvertU32ToFloat4(Spectrum::BLUE600);
        colors[ImGuiCol_TextSelectedBg]        = ImGui::ColorConvertU32ToFloat4((Spectrum::BLUE400 & 0x00FFFFFF) | 0x33000000);
        colors[ImGuiCol_DragDropTarget]        = ImVec4(1.00f, 1.00f, 0.00f, 0.90f);
        colors[ImGuiCol_NavHighlight]          = ImGui::ColorConvertU32ToFloat4((Spectrum::GRAY900 & 0x00FFFFFF) | 0x0A000000);
        colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
        colors[ImGuiCol_NavWindowingDimBg]     = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
        colors[ImGuiCol_ModalWindowDimBg]      = ImVec4(0.20f, 0.20f, 0.20f, 0.35f);

        ImGui_ImplGlfw_InitForVulkan(raw_window, true);

        ImGui_ImplVulkan_InitInfo init_info{};
        init_info.Instance              = m_instance;
        init_info.PhysicalDevice        = m_GPU;
        init_info.Device                = m_device;
        init_info.QueueFamily           = m_graphics_queue_family;
        init_info.Queue                 = m_graphics_queue;
        init_info.DescriptorPool        = imgui_pool;
        init_info.MinImageCount         = 3;
        init_info.ImageCount            = 3;
        init_info.MSAASamples           = VK_SAMPLE_COUNT_1_BIT;
        init_info.UseDynamicRendering   = true;
        init_info.ColorAttachmentFormat = static_cast<VkFormat>(m_swapchain_format);

        ImGui_ImplVulkan_Init(&init_info, VK_NULL_HANDLE);

        io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\msyh.ttc", 24.0f, NULL, io.Fonts->GetGlyphRangesChineseFull());

        auto cmd_buffer = BeginSingleTimeCommands();
        ImGui_ImplVulkan_CreateFontsTexture(cmd_buffer);
        EndSingleTimeCommands(cmd_buffer);
        ImGui_ImplVulkan_DestroyFontUploadObjects();

        m_main_deletion_queue.PushFunction(
            [=]()
            {
                ImGui_ImplVulkan_Shutdown();
                vkDestroyDescriptorPool(m_device, imgui_pool, nullptr);
            });
    }

    void VulkanRHI::PrepareBeforePass()
    {
        // 等待 fence
        YUTREL_ASSERT(m_device.waitForFences(GetCurrentFrame().render_fence, vk::True, UINT64_MAX) == vk::Result::eSuccess, "Failed to synchronize");

        // 清除为单独帧创建的对象
        GetCurrentFrame().deletion_queue.flush();

        // 请求图像索引
        // auto result = m_device.acquireNextImageKHR(m_swapchain, UINT64_MAX, GetCurrentFrame().available_for_render_semaphore, {});

        auto result = vkAcquireNextImageKHR(m_device, m_swapchain, UINT64_MAX, GetCurrentFrame().available_for_render_semaphore, VK_NULL_HANDLE, &m_cur_swapchain_image_index);
        // 若窗口大小发生改变
        // if (result == VK_ERROR_OUT_OF_DATE_KHR)
        if (result != VK_SUCCESS)
        {
            m_resize_requested = true;
            return;
        }

        // 重置命令缓冲
        vk::CommandBuffer cmd_buffer = GetCurrentCommandBuffer();

        cmd_buffer.reset();

        // 开始指令缓冲
        cmd_buffer.begin(vk::CommandBufferBeginInfo());
    }

    void VulkanRHI::SubmitRendering()
    {
        //-------------终止指令缓冲-------------
        vk::CommandBuffer cmd_buffer = GetCurrentCommandBuffer();
        cmd_buffer.end();

        // 重设fence
        m_device.resetFences(GetCurrentFrame().render_fence);

        // --------------提交指令---------------
        auto cmd_buffer_si =
            vk::CommandBufferSubmitInfo()
                .setCommandBuffer(cmd_buffer);

        auto semaphore_wait_si =
            vk::SemaphoreSubmitInfo()
                .setSemaphore(GetCurrentFrame().available_for_render_semaphore)
                .setStageMask(vk::PipelineStageFlagBits2::eColorAttachmentOutput)
                .setDeviceIndex(0)
                .setValue(1);

        auto semaphore_signal_si =
            vk::SemaphoreSubmitInfo()
                .setSemaphore(GetCurrentFrame().finished_for_presentation_semaphore)
                .setStageMask(vk::PipelineStageFlagBits2::eAllGraphics)
                .setDeviceIndex(0)
                .setValue(1);

        auto submit_info =
            vk::SubmitInfo2()
                .setWaitSemaphoreInfos(semaphore_wait_si)
                .setSignalSemaphoreInfos(semaphore_signal_si)
                .setCommandBufferInfos(cmd_buffer_si);

        // 提交到队列
        m_graphics_queue.submit2(submit_info, GetCurrentFrame().render_fence);

        //--------------显示图像------------
        auto present_info =
            vk::PresentInfoKHR()
                .setSwapchains(m_swapchain)
                .setWaitSemaphores(GetCurrentFrame().finished_for_presentation_semaphore)
                .setImageIndices(m_cur_swapchain_image_index);

        VkPresentInfoKHR _present_info = static_cast<VkPresentInfoKHR>(present_info);

        auto result = vkQueuePresentKHR(m_graphics_queue, &_present_info);
        if (result == VK_ERROR_OUT_OF_DATE_KHR)
        {
            m_resize_requested = true;
        }

        m_cur_frame++;
    }

    void VulkanRHI::UpdateSwapchainSize(uint32_t width, uint32_t height)
    {
        m_new_swapchain_extent.width  = width;
        m_new_swapchain_extent.height = height;
    }

    void VulkanRHI::ResizeSwapchain()
    {
        m_device.waitIdle();

        DestroySwapchain();

        InitSwapchain(m_new_swapchain_extent.width, m_new_swapchain_extent.height);

        m_resize_requested = false;
    }

    vk::CommandBuffer VulkanRHI::BeginSingleTimeCommands()
    {
        auto cmd_buffer_ai =
            vk::CommandBufferAllocateInfo()
                .setCommandPool(m_rhi_cmd_pool)
                .setCommandBufferCount(1)
                .setLevel(vk::CommandBufferLevel::ePrimary);

        vk::CommandBuffer cmd_buffer = m_device.allocateCommandBuffers(cmd_buffer_ai).front();

        auto cmd_buffer_bi =
            vk::CommandBufferBeginInfo()
                .setFlags(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);

        cmd_buffer.begin(cmd_buffer_bi);

        return cmd_buffer;
    }

    void VulkanRHI::EndSingleTimeCommands(vk::CommandBuffer cmd_buffer)
    {
        cmd_buffer.end();

        auto cmd_buffer_si =
            vk::CommandBufferSubmitInfo()
                .setCommandBuffer(cmd_buffer);
        auto submit_info =
            vk::SubmitInfo2()
                .setCommandBufferInfos(cmd_buffer_si);

        m_graphics_queue.submit2(submit_info);
        m_graphics_queue.waitIdle();

        m_device.freeCommandBuffers(m_rhi_cmd_pool, cmd_buffer);
    }

    void VulkanRHI::TransitionImage(vk::CommandBuffer cmd_buffer, vk::Image image, vk::ImageLayout cur_layout, vk::ImageLayout new_layout)
    {
        vk::ImageAspectFlags aspect_mask;
        switch (new_layout)
        {
        case vk::ImageLayout::eDepthAttachmentOptimal:
            aspect_mask = vk::ImageAspectFlagBits::eDepth;
            break;
        case vk::ImageLayout::eDepthStencilAttachmentOptimal:
        case vk::ImageLayout::eDepthAttachmentStencilReadOnlyOptimal:
        case vk::ImageLayout::eDepthStencilReadOnlyOptimal:
            aspect_mask = vk::ImageAspectFlagBits::eDepth | vk::ImageAspectFlagBits::eStencil;
            break;
        default:
            aspect_mask = vk::ImageAspectFlagBits::eColor;
            break;
        }

        auto image_barrier =
            vk::ImageMemoryBarrier2()
                .setSrcStageMask(vk::PipelineStageFlagBits2::eAllCommands)
                .setSrcAccessMask(vk::AccessFlagBits2::eMemoryWrite)
                .setDstStageMask(vk::PipelineStageFlagBits2::eAllCommands)
                .setDstAccessMask(vk::AccessFlagBits2::eMemoryWrite | vk::AccessFlagBits2::eMemoryRead)
                .setOldLayout(cur_layout)
                .setNewLayout(new_layout)
                .setSubresourceRange(vk::ImageSubresourceRange(aspect_mask, 0, vk::RemainingMipLevels, 0, vk::RemainingArrayLayers))
                .setImage(image);

        auto dependency_info =
            vk::DependencyInfo()
                .setImageMemoryBarriers(image_barrier);

        cmd_buffer.pipelineBarrier2(dependency_info);
    }

    void VulkanRHI::CopyImageToImage(vk::CommandBuffer cmd_buffer, vk::Image source, vk::Image destination, vk::Extent2D src_size, vk::Extent2D dst_size)
    {
        auto blit_region =
            vk::ImageBlit2()
                .setSrcOffsets({vk::Offset3D(),
                                vk::Offset3D(src_size.width, src_size.height, 1)})
                .setDstOffsets({vk::Offset3D(),
                                vk::Offset3D(dst_size.width, dst_size.height, 1)})
                .setSrcSubresource(vk::ImageSubresourceLayers(vk::ImageAspectFlagBits::eColor, 0, 0, 1))
                .setDstSubresource(vk::ImageSubresourceLayers(vk::ImageAspectFlagBits::eColor, 0, 0, 1));

        auto blit_image_info =
            vk::BlitImageInfo2()
                .setSrcImage(source)
                .setSrcImageLayout(vk::ImageLayout::eTransferSrcOptimal)
                .setDstImage(destination)
                .setDstImageLayout(vk::ImageLayout::eTransferDstOptimal)
                .setFilter(vk::Filter::eLinear)
                .setRegions(blit_region);

        cmd_buffer.blitImage2(blit_image_info);
    }

    void VulkanRHI::GenerateMipmaps(vk::CommandBuffer cmd_buffer, vk::Image image, vk::Extent2D image_size)
    {
        uint32_t mip_levels = static_cast<uint32_t>(std::floor(std::log2(std::max(image_size.width, image_size.height)))) + 1;

        for (int mip = 0; mip < mip_levels; mip++)
        {
            vk::Extent2D half_size = image_size;
            half_size.width /= 2;
            half_size.height /= 2;

            auto image_barrier =
                vk::ImageMemoryBarrier2()
                    .setSrcStageMask(vk::PipelineStageFlagBits2::eAllCommands)
                    .setSrcAccessMask(vk::AccessFlagBits2::eMemoryWrite)
                    .setDstStageMask(vk::PipelineStageFlagBits2::eAllCommands)
                    .setDstAccessMask(vk::AccessFlagBits2::eMemoryWrite | vk::AccessFlagBits2::eMemoryRead)
                    .setOldLayout(vk::ImageLayout::eTransferDstOptimal)
                    .setNewLayout(vk::ImageLayout::eTransferSrcOptimal)
                    .setSubresourceRange(vk::ImageSubresourceRange(vk::ImageAspectFlagBits::eColor, mip, 1, 0, 1))
                    .setImage(image);

            auto dependency_info =
                vk::DependencyInfo()
                    .setImageMemoryBarriers(image_barrier);

            cmd_buffer.pipelineBarrier2(dependency_info);

            if (mip < mip_levels - 1)
            {
                auto blit_region =
                    vk::ImageBlit2()
                        .setSrcOffsets({vk::Offset3D(),
                                        vk::Offset3D(image_size.width, image_size.height, 1)})
                        .setDstOffsets({vk::Offset3D(),
                                        vk::Offset3D(half_size.width, half_size.height, 1)})
                        .setSrcSubresource(vk::ImageSubresourceLayers(vk::ImageAspectFlagBits::eColor, mip, 0, 1))
                        .setDstSubresource(vk::ImageSubresourceLayers(vk::ImageAspectFlagBits::eColor, mip + 1, 0, 1));

                auto blit_image_info =
                    vk::BlitImageInfo2()
                        .setSrcImage(image)
                        .setSrcImageLayout(vk::ImageLayout::eTransferSrcOptimal)
                        .setDstImage(image)
                        .setDstImageLayout(vk::ImageLayout::eTransferDstOptimal)
                        .setFilter(vk::Filter::eLinear)
                        .setRegions(blit_region);

                cmd_buffer.blitImage2(blit_image_info);

                image_size = half_size;
            }
        }

        // 转换布局
        TransitionImage(cmd_buffer,
                        image,
                        vk::ImageLayout::eTransferSrcOptimal,
                        vk::ImageLayout::eShaderReadOnlyOptimal);
    }

    AllocatedImage VulkanRHI::CreateImage(vk::Extent3D extent, vk::Format format, vk::ImageUsageFlags usage, bool mipmapped, uint32_t array_layers)
    {
        AllocatedImage new_image;
        new_image.format = format;
        new_image.extent = extent;

        auto image_ci =
            vk::ImageCreateInfo()
                .setImageType(vk::ImageType::e2D)
                .setFormat(format)
                .setExtent(extent)
                .setMipLevels(1)
                .setArrayLayers(array_layers)
                .setSamples(vk::SampleCountFlagBits::e1)
                .setTiling(vk::ImageTiling::eOptimal)
                .setUsage(usage);
        if (mipmapped)
        {
            new_image.mipmapped = true;
            image_ci.setMipLevels(static_cast<uint32_t>(std::floor(std::log2(std::max(extent.width, extent.height)))) + 1);
        }

        // 将图片保存至GPU上
        auto image_ai =
            vma::AllocationCreateInfo()
                .setUsage(vma::MemoryUsage::eGpuOnly)
                .setRequiredFlags(vk::MemoryPropertyFlagBits::eDeviceLocal);

        std::tie(new_image.image, new_image.allocation) = m_allocator.createImage(image_ci, image_ai);

        //---------创建图像视图-----------
        auto subresource_range =
            vk::ImageSubresourceRange()
                .setBaseMipLevel(0)
                .setLevelCount(1)
                .setBaseArrayLayer(0)
                .setLayerCount(array_layers);

        switch (format)
        {
        case vk::Format::eD16Unorm:
        case vk::Format::eD24UnormS8Uint:
        case vk::Format::eD32Sfloat:
        case vk::Format::eD32SfloatS8Uint:
            subresource_range.setAspectMask(vk::ImageAspectFlagBits::eDepth);
            break;
        default:
            subresource_range.setAspectMask(vk::ImageAspectFlagBits::eColor);
            break;
        }

        auto view_ci =
            vk::ImageViewCreateInfo()
                .setImage(new_image.image)
                .setFormat(format)
                .setSubresourceRange(subresource_range);

        if (array_layers == 1)
        {
            view_ci.setViewType(vk::ImageViewType::e2D);
        }
        else
        {
            view_ci.setViewType(vk::ImageViewType::e2DArray);
        }

        new_image.image_view = m_device.createImageView(view_ci);

        m_main_deletion_queue.PushFunction(
            [=]()
            {
                m_device.destroyImageView(new_image.image_view);
                m_allocator.destroyImage(new_image.image, new_image.allocation);
            });

        return new_image;
    }

    vk::ImageView VulkanRHI::CreateImageView(const vk::ImageViewCreateInfo& info)
    {
        vk::ImageView new_view = m_device.createImageView(info);

        m_main_deletion_queue.PushFunction(
            [=]()
            {
                m_device.destroyImageView(new_view);
            });

        return new_view;
    }

    AllocatedImage VulkanRHI::CreateCubeMap(vk::Extent3D extent, vk::Format format, vk::ImageUsageFlags usage, bool mipmapped)
    {
        AllocatedImage new_image;
        new_image.format = format;
        new_image.extent = extent;

        auto image_ci =
            vk::ImageCreateInfo()
                .setImageType(vk::ImageType::e2D)
                .setFlags(vk::ImageCreateFlagBits::eCubeCompatible)
                .setFormat(format)
                .setExtent(extent)
                .setMipLevels(1)
                .setArrayLayers(6)
                .setSamples(vk::SampleCountFlagBits::e1)
                .setTiling(vk::ImageTiling::eOptimal)
                .setUsage(usage);
        if (mipmapped)
        {
            new_image.mipmapped = true;
            image_ci.setMipLevels(static_cast<uint32_t>(std::floor(std::log2(std::max(extent.width, extent.height)))) + 1);
        }

        // 将图片保存至GPU上
        auto image_ai =
            vma::AllocationCreateInfo()
                .setUsage(vma::MemoryUsage::eGpuOnly)
                .setRequiredFlags(vk::MemoryPropertyFlagBits::eDeviceLocal);

        std::tie(new_image.image, new_image.allocation) = m_allocator.createImage(image_ci, image_ai);

        //---------创建图像视图-----------
        auto subresource_range =
            vk::ImageSubresourceRange()
                .setBaseMipLevel(0)
                .setLevelCount(1)
                .setBaseArrayLayer(0)
                .setLayerCount(6);

        switch (format)
        {
        case vk::Format::eD16Unorm:
        case vk::Format::eD24UnormS8Uint:
        case vk::Format::eD32Sfloat:
        case vk::Format::eD32SfloatS8Uint:
            subresource_range.setAspectMask(vk::ImageAspectFlagBits::eDepth);
            break;
        default:
            subresource_range.setAspectMask(vk::ImageAspectFlagBits::eColor);
            break;
        }

        auto view_ci =
            vk::ImageViewCreateInfo()
                .setImage(new_image.image)
                .setFormat(format)
                .setViewType(vk::ImageViewType::eCube)
                .setSubresourceRange(subresource_range);

        new_image.image_view = m_device.createImageView(view_ci);

        m_main_deletion_queue.PushFunction(
            [=]()
            {
                m_device.destroyImageView(new_image.image_view);
                m_allocator.destroyImage(new_image.image, new_image.allocation);
            });

        return new_image;
    }

    void VulkanRHI::UploadImageData(void* data, AllocatedImage& image)
    {
        size_t DATA_SIZE;
        if (image.format == vk::Format::eR32G32B32A32Sfloat)
        {
            DATA_SIZE = image.extent.width * image.extent.height * image.extent.depth * 4 * 4;
        }
        else
        {
            DATA_SIZE = image.extent.width * image.extent.height * image.extent.depth * 4;
        }

        // CPU_TO_GPU内存有时可能会不够大
        AllocatedBuffer staging_buffer =
            CreateBuffer(DATA_SIZE,
                         vk::BufferUsageFlagBits::eTransferSrc,
                         vma::MemoryUsage::eCpuOnly,
                         false);

        // 将图像数据存入暂存缓冲区
        memcpy(staging_buffer.info.pMappedData, data, DATA_SIZE);

        // 转换格式
        vk::CommandBuffer cmd_buffer = BeginSingleTimeCommands();
        {
            // 转换布局
            TransitionImage(cmd_buffer,
                            image.image,
                            vk::ImageLayout::eUndefined,
                            vk::ImageLayout::eTransferDstOptimal);

            // 图像拷贝到GPU
            auto image_subresource =
                vk::ImageSubresourceLayers()
                    .setAspectMask(vk::ImageAspectFlagBits::eColor)
                    .setMipLevel(0)
                    .setBaseArrayLayer(0)
                    .setLayerCount(1);

            auto copy_region =
                vk::BufferImageCopy()
                    .setBufferOffset(0)
                    .setBufferRowLength(0)
                    .setBufferImageHeight(0)
                    .setImageExtent(image.extent)
                    .setImageSubresource(image_subresource);

            cmd_buffer.copyBufferToImage(staging_buffer.buffer, image.image, vk::ImageLayout::eTransferDstOptimal, copy_region);

            // 生成mipmap
            if (image.mipmapped)
            {
                GenerateMipmaps(cmd_buffer, image.image, {image.extent.width, image.extent.height});
            }
            else
            {
                TransitionImage(cmd_buffer,
                                image.image,
                                vk::ImageLayout::eTransferDstOptimal,
                                vk::ImageLayout::eShaderReadOnlyOptimal);
            }
        }
        EndSingleTimeCommands(cmd_buffer);

        DestroyBuffer(staging_buffer);
    }

    void VulkanRHI::UploadCubeMapData(std::array<void*, 6> data, AllocatedImage& image)
    {
        const size_t DATA_SIZE      = image.extent.width * image.extent.height * 4 * 4;
        const size_t CUBE_DATA_SIZE = DATA_SIZE * 6;

        // CPU_TO_GPU内存有时可能会不够大
        AllocatedBuffer staging_buffer =
            CreateBuffer(CUBE_DATA_SIZE,
                         vk::BufferUsageFlagBits::eTransferSrc,
                         vma::MemoryUsage::eCpuOnly,
                         false);

        // 将图像数据存入暂存缓冲区
        for (int i = 0; i < 6; i++)
        {
            memcpy((void*)(static_cast<char*>(staging_buffer.info.pMappedData) + DATA_SIZE * i), data[i], DATA_SIZE);
        }
        // memcpy(staging_buffer.info.pMappedData, data.data(), CUBE_DATA_SIZE);

        // 转换格式
        vk::CommandBuffer cmd_buffer = BeginSingleTimeCommands();
        {
            // 转换布局
            TransitionImage(cmd_buffer,
                            image.image,
                            vk::ImageLayout::eUndefined,
                            vk::ImageLayout::eTransferDstOptimal);

            // 图像拷贝到GPU
            auto image_subresource =
                vk::ImageSubresourceLayers()
                    .setAspectMask(vk::ImageAspectFlagBits::eColor)
                    .setMipLevel(0)
                    .setBaseArrayLayer(0)
                    .setLayerCount(6);

            auto copy_region =
                vk::BufferImageCopy()
                    .setBufferOffset(0)
                    .setBufferRowLength(0)
                    .setBufferImageHeight(0)
                    .setImageExtent(image.extent)
                    .setImageSubresource(image_subresource);

            cmd_buffer.copyBufferToImage(staging_buffer.buffer, image.image, vk::ImageLayout::eTransferDstOptimal, copy_region);

            TransitionImage(cmd_buffer,
                            image.image,
                            vk::ImageLayout::eTransferDstOptimal,
                            vk::ImageLayout::eShaderReadOnlyOptimal);
        }
        EndSingleTimeCommands(cmd_buffer);

        DestroyBuffer(staging_buffer);
    }

    AllocatedBuffer VulkanRHI::CreateBuffer(size_t alloc_size, vk::BufferUsageFlags buffer_usage, vma::MemoryUsage memory_usage, bool auto_destroy)
    {
        auto buffer_ci =
            vk::BufferCreateInfo()
                .setSize(alloc_size)
                .setUsage(buffer_usage);

        auto allocation_ci =
            vma::AllocationCreateInfo()
                .setUsage(memory_usage)
                .setFlags(vma::AllocationCreateFlagBits::eMapped);

        AllocatedBuffer new_buffer;
        YUTREL_ASSERT(m_allocator.createBuffer(&buffer_ci, &allocation_ci, &new_buffer.buffer, &new_buffer.allocation, &new_buffer.info) == vk::Result::eSuccess, "Failed to create buffer");

        if (auto_destroy)
        {
            m_main_deletion_queue.PushFunction(
                [=]()
                {
                    m_allocator.destroyBuffer(new_buffer.buffer, new_buffer.allocation);
                });
        }

        return new_buffer;
    }

    void VulkanRHI::DestroyBuffer(const AllocatedBuffer& buffer)
    {
        m_allocator.destroyBuffer(buffer.buffer, buffer.allocation);
    }

    vk::ShaderModule VulkanRHI::CreateShaderModule(const std::vector<unsigned char>& shader_code)
    {
        VkShaderModuleCreateInfo create_info{};
        create_info.sType    = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        create_info.pNext    = nullptr;
        create_info.codeSize = shader_code.size();
        create_info.pCode    = reinterpret_cast<const uint32_t*>(shader_code.data());

        auto shader_ci =
            vk::ShaderModuleCreateInfo()
                .setCodeSize(shader_code.size())
                .setPCode(reinterpret_cast<const uint32_t*>(shader_code.data()));

        VkShaderModule shader_module = m_device.createShaderModule(shader_ci);

        return shader_module;
    }

    void VulkanRHI::DestroyShaderModule(vk::ShaderModule shader)
    {
        m_device.destroyShaderModule(shader);
    }

    vk::PipelineLayout VulkanRHI::CreatePipelineLayout(const vk::PipelineLayoutCreateInfo& info)
    {
        vk::PipelineLayout layout = m_device.createPipelineLayout(info);

        m_main_deletion_queue.PushFunction(
            [=]()
            {
                m_device.destroyPipelineLayout(layout);
            });

        return layout;
    }

    vk::Pipeline VulkanRHI::CreateRenderPipeline(const RenderPipelineCreateInfo& info)
    {
        // 视口和裁剪
        auto viewport_state_ci =
            vk::PipelineViewportStateCreateInfo()
                .setViewportCount(1)
                .setScissorCount(1);

        // 颜色混合
        std::vector<vk::PipelineColorBlendAttachmentState> color_blend_attachments;
        for (size_t i = 0; i < info.render_info.colorAttachmentCount; i++)
        {
            color_blend_attachments.push_back(info.color_blend_attachment);
        }

        auto color_blend_state_ci =
            vk::PipelineColorBlendStateCreateInfo()
                .setLogicOpEnable(vk::False)
                .setLogicOp(vk::LogicOp::eCopy)
                .setAttachments(color_blend_attachments);

        // 顶点阶段
        // 不需要
        auto vertex_input_state_ci = vk::PipelineVertexInputStateCreateInfo();

        // 动态状态
        std::array<vk::DynamicState, 2> states =
            {
                vk::DynamicState::eViewport,
                vk::DynamicState::eScissor,
            };
        auto dynamic_state_ci =
            vk::PipelineDynamicStateCreateInfo()
                .setDynamicStates(states);

        //---------管线创建信息-------------
        auto pipeline_ci =
            vk::GraphicsPipelineCreateInfo()
                .setPNext(&info.render_info)
                .setStages(info.shader_stages)
                .setPVertexInputState(&vertex_input_state_ci)
                .setPInputAssemblyState(&info.input_assembly)
                .setPViewportState(&viewport_state_ci)
                .setPRasterizationState(&info.rasterizer)
                .setPMultisampleState(&info.multisampling)
                .setPColorBlendState(&color_blend_state_ci)
                .setPDepthStencilState(&info.depth_stencil)
                .setPDynamicState(&dynamic_state_ci)
                .setLayout(info.pipeline_layout);

        auto result = m_device.createGraphicsPipeline(nullptr, pipeline_ci);
        YUTREL_ASSERT(result.result == vk::Result::eSuccess, "Failed to create pipelines");

        m_main_deletion_queue.PushFunction(
            [=]()
            {
                m_device.destroyPipeline(result.value);
            });

        return result.value;
    }

    vk::DescriptorSetLayout VulkanRHI::CreateDescriptorSetLayout(DescriptorSetLayoutCreateInfo& info)
    {
        for (auto& b : info.bindings)
        {
            b.stageFlags |= info.shader_stages;
        }

        auto layout_ci =
            vk::DescriptorSetLayoutCreateInfo()
                .setBindings(info.bindings)
                .setFlags({});

        vk::DescriptorSetLayout layout = m_device.createDescriptorSetLayout(layout_ci);

        m_main_deletion_queue.PushFunction(
            [=]()
            {
                m_device.destroyDescriptorSetLayout(layout);
            });

        return layout;
    }

    vk::DescriptorSet VulkanRHI::AllocateDescriptorSets(vk::DescriptorSetLayout layout)
    {
        auto set_ai =
            vk::DescriptorSetAllocateInfo()
                .setDescriptorPool(m_descriptor_pool)
                .setDescriptorSetCount(1)
                .setSetLayouts(layout);

        vk::DescriptorSet set = m_device.allocateDescriptorSets(set_ai).front();

        return set;
    }

    void VulkanRHI::UpdateDescriptorSets(DescriptorWriter& writer, vk::DescriptorSet set)
    {
        for (auto& write : writer.writes)
        {
            write.setDstSet(set);
        }
        m_device.updateDescriptorSets(writer.writes, {});
    }

    vk::DeviceAddress VulkanRHI::GetBufferDeviceAddress(const vk::BufferDeviceAddressInfo& info)
    {
        return m_device.getBufferAddress(info);
    }

    vk::Sampler VulkanRHI::CreateSampler(const vk::SamplerCreateInfo& info)
    {
        vk::Sampler sampler = m_device.createSampler(info);

        m_main_deletion_queue.PushFunction(
            [=]()
            {
                m_device.destroySampler(sampler);
            });

        return sampler;
    }

} // namespace Yutrel