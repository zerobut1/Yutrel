#pragma once

#include <vk_mem_alloc.h>
#include <vulkan/vk_enum_string_helper.h>
#include <vulkan/vulkan.h>

#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/matrix.hpp>

#include <deque>
#include <string>
#include <vector>

namespace Yutrel
{
    // 删除队列
    struct DeletionQueue
    {
        std::deque<std::function<void()>> deletors;

        void PushFunction(std::function<void()>&& function)
        {
            deletors.push_back(function);
        }

        void flush()
        {
            for (auto it = deletors.rbegin(); it != deletors.rend(); it++)
            {
                // 调用删除函数
                (*it)();
            }

            deletors.clear();
        }
    };

    // 分配缓冲区
    struct AllocatedBuffer
    {
        VkBuffer buffer;
        VmaAllocation allocation;
        VmaAllocationInfo info;
    };

    // 分配图像
    struct AllocatedImage
    {
        VkImage image;
        VkImageView image_view;
        VmaAllocation allocation;
        VkExtent3D image_extent;
        VkFormat image_format;
    };

    // 帧数据
    struct FrameData
    {
        // 指令
        VkCommandPool command_pool;
        VkCommandBuffer main_command_buffer;

        // 同步
        VkSemaphore finished_for_presentation_semaphore, available_for_render_semaphore;
        VkFence render_fence;

        // 删除队列
        DeletionQueue deletion_queue;

        // // 摄像机缓冲
        // AllocatedBuffer camera_buffer;
        // VkDescriptorSet global_descriptor;

        // // 对象缓冲
        // AllocatedBuffer object_buffer;
        // VkDescriptorSet object_descriptor;
    };

    // 交换链信息
    struct RHISwapChainDesc
    {
        VkExtent2D extent;
        VkFormat image_format;
        VkViewport viewport;
        VkRect2D scissor;
        std::vector<VkImageView>* image_views;
    };

    // 图形管线创建信息
    struct RHIGraphicsPipelineCreateInfo
    {
        // 着色器阶段
        std::vector<VkPipelineShaderStageCreateInfo> shader_stages;

        // 渲染管线布局
        VkPipelineLayout pipeline_layout;

        // 图元装配
        VkPipelineInputAssemblyStateCreateInfo input_assembly;

        // 光栅化阶段
        VkPipelineRasterizationStateCreateInfo rasterizer;

        // MSAA
        VkPipelineMultisampleStateCreateInfo multisampling;

        // 颜色混合
        VkPipelineColorBlendStateCreateInfo color_blend;

        // 顶点输入阶段
        VkPipelineVertexInputStateCreateInfo vertex_input;

        // 视口阶段
        VkPipelineViewportStateCreateInfo viewport_state;

        // 渲染pass
        VkRenderPass render_pass;

        // subpass
        uint32_t subpass;
    };

    // 动态渲染管线创建信息
    struct RHIDynamicPipelineCreateInfo
    {
        // 着色器阶段
        std::vector<VkPipelineShaderStageCreateInfo> shader_stages;

        // 渲染管线布局
        VkPipelineLayout pipeline_layout;

        // 图元装配
        VkPipelineInputAssemblyStateCreateInfo input_assembly;

        // 光栅化阶段
        VkPipelineRasterizationStateCreateInfo rasterizer;

        // MSAA
        VkPipelineMultisampleStateCreateInfo multisampling;

        // 颜色混合
        VkPipelineColorBlendAttachmentState color_blend_attachment;

        // 深度模板阶段
        VkPipelineDepthStencilStateCreateInfo depth_stencil;

        // 渲染创建信息
        VkPipelineRenderingCreateInfo render_info;

        // 颜色缓冲格式
        VkFormat color_attachment_format;

        void Clear()
        {
            input_assembly       = {};
            input_assembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;

            rasterizer       = {};
            rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;

            color_blend_attachment = {};

            multisampling       = {};
            multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;

            pipeline_layout = {};

            depth_stencil       = {};
            depth_stencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;

            render_info       = {};
            render_info.sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO;

            shader_stages.clear();
        }

        void SetShaders(VkShaderModule vertex_shader, VkShaderModule fragment_shader)
        {
            shader_stages.clear();

            // 顶点着色器
            VkPipelineShaderStageCreateInfo info{};
            info.sType  = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
            info.pNext  = nullptr;
            info.stage  = VK_SHADER_STAGE_VERTEX_BIT;
            info.module = vertex_shader;
            info.pName  = "main";
            shader_stages.push_back(info);

            // 片段着色器
            info.stage  = VK_SHADER_STAGE_FRAGMENT_BIT;
            info.module = fragment_shader;
            shader_stages.push_back(info);
        }

        void SetInputTopolygy(VkPrimitiveTopology topology)
        {
            input_assembly.topology               = topology;
            input_assembly.primitiveRestartEnable = VK_FALSE;
        }

        void SetPolygonMode(VkPolygonMode mode)
        {
            rasterizer.polygonMode = mode;
            rasterizer.lineWidth   = 1.0f;
        }

        void SetCullMode(VkCullModeFlags cull_mode, VkFrontFace front_face)
        {
            rasterizer.cullMode  = cull_mode;
            rasterizer.frontFace = front_face;
        }

        // 暂时设为默认禁用MSAA
        void SetMultisamplingNone()
        {
            multisampling.sampleShadingEnable   = VK_FALSE;
            multisampling.rasterizationSamples  = VK_SAMPLE_COUNT_1_BIT;
            multisampling.minSampleShading      = 1.0f;
            multisampling.pSampleMask           = nullptr;
            multisampling.alphaToCoverageEnable = VK_FALSE;
            multisampling.alphaToOneEnable      = VK_FALSE;
        }

        void DisableBlending()
        {
            color_blend_attachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
            color_blend_attachment.blendEnable    = VK_FALSE;
        }

        void SetColorAttachmentFormat(VkFormat format)
        {
            color_attachment_format = format;

            render_info.colorAttachmentCount    = 1;
            render_info.pColorAttachmentFormats = &color_attachment_format;
        }

        void SetDepthFormat(VkFormat format)
        {
            render_info.depthAttachmentFormat = format;
        }

        void DisableDepthTest()
        {
            depth_stencil.depthTestEnable       = VK_FALSE;
            depth_stencil.depthWriteEnable      = VK_FALSE;
            depth_stencil.depthCompareOp        = VK_COMPARE_OP_NEVER;
            depth_stencil.depthBoundsTestEnable = VK_FALSE;
            depth_stencil.stencilTestEnable     = VK_FALSE;
            depth_stencil.front                 = {};
            depth_stencil.back                  = {};
            depth_stencil.minDepthBounds        = 0.f;
            depth_stencil.maxDepthBounds        = 1.f;
        }

        void EnableDepthTest(bool depth_write_enable, VkCompareOp op)
        {
            depth_stencil.depthTestEnable       = VK_TRUE;
            depth_stencil.depthWriteEnable      = depth_write_enable;
            depth_stencil.depthCompareOp        = op;
            depth_stencil.depthBoundsTestEnable = VK_FALSE;
            depth_stencil.stencilTestEnable     = VK_FALSE;
            depth_stencil.front                 = {};
            depth_stencil.back                  = {};
            depth_stencil.minDepthBounds        = 0.0f;
            depth_stencil.maxDepthBounds        = 1.0f;
        }
    };

    struct RHIDescriptorLayoutCreateInfo
    {
        std::vector<VkDescriptorSetLayoutBinding> bindings;

        VkShaderStageFlags shader_stages;

        void AddBinding(uint32_t binding, VkDescriptorType type)
        {
            VkDescriptorSetLayoutBinding newbind{};
            newbind.binding         = binding;
            newbind.descriptorCount = 1;
            newbind.descriptorType  = type;

            bindings.push_back(newbind);
        }

        void Clear()
        {
            bindings.clear();
        }
    };

} // namespace Yutrel
