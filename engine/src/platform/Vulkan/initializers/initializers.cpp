#include "yutrel_pch.hpp"

#include "initializers.hpp"

namespace Yutrel
{
    namespace vkinit
    {
        VkCommandPoolCreateInfo CommandPoolCreateInfo(uint32_t queue_family_index, VkCommandPoolCreateFlags flags)
        {
            VkCommandPoolCreateInfo info{};
            info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
            info.pNext = nullptr;

            info.queueFamilyIndex = queue_family_index;
            info.flags            = flags;

            return info;
        }

        VkCommandBufferAllocateInfo CommandBufferAllocateInfo(VkCommandPool pool, uint32_t count, VkCommandBufferLevel level)
        {
            VkCommandBufferAllocateInfo info{};
            info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
            info.pNext = nullptr;

            info.commandPool        = pool;
            info.commandBufferCount = count;
            info.level              = level;

            return info;
        }

        VkFenceCreateInfo FenceCreateInfo(VkFenceCreateFlags flags)
        {
            VkFenceCreateInfo info{};
            info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
            info.pNext = nullptr;

            info.flags = flags;

            return info;
        }

        VkSemaphoreCreateInfo SemaphoreCreateInfo(VkSemaphoreCreateFlags flags)
        {
            VkSemaphoreCreateInfo info{};
            info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
            info.pNext = nullptr;

            info.flags = flags;

            return info;
        }

        VkFramebufferCreateInfo FramebufferCreateInfo(VkRenderPass render_pass, VkExtent2D extent)
        {
            VkFramebufferCreateInfo info{};
            info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
            info.pNext = nullptr;

            info.renderPass      = render_pass;
            info.attachmentCount = 1;
            info.width           = extent.width;
            info.height          = extent.height;
            info.layers          = 1;

            return info;
        }

        VkPipelineLayoutCreateInfo PipelineLayoutCreateInfo()
        {
            VkPipelineLayoutCreateInfo info{};
            info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
            info.pNext = nullptr;

            // 暂时默认设为空值
            info.flags                  = 0;
            info.setLayoutCount         = 0;
            info.pSetLayouts            = nullptr;
            info.pushConstantRangeCount = 0;
            info.pPushConstantRanges    = nullptr;

            return info;
        }

        VkPipelineShaderStageCreateInfo PipelineShaderStageCreateInfo(VkShaderStageFlagBits stage, VkShaderModule shader_module)
        {
            VkPipelineShaderStageCreateInfo info{};
            info.sType  = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
            info.pNext  = nullptr;
            info.stage  = stage;
            info.module = shader_module;
            info.pName  = "main";

            return info;
        }

        VkPipelineVertexInputStateCreateInfo VertexInputStateCreateInfo()
        {
            VkPipelineVertexInputStateCreateInfo info{};
            info.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
            info.pNext = nullptr;

            // 暂时不设置
            info.vertexBindingDescriptionCount   = 0;
            info.vertexAttributeDescriptionCount = 0;

            return info;
        }

        VkPipelineInputAssemblyStateCreateInfo InputAssemblyCreateInfo(VkPrimitiveTopology topology)
        {
            VkPipelineInputAssemblyStateCreateInfo info{};
            info.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
            info.pNext = nullptr;

            info.topology               = topology;
            info.primitiveRestartEnable = VK_FALSE;

            return info;
        }

        VkPipelineRasterizationStateCreateInfo RasterizationStateCreateInfo(VkPolygonMode polygon_mode)
        {
            VkPipelineRasterizationStateCreateInfo info{};
            info.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
            info.pNext = nullptr;

            info.depthClampEnable        = VK_FALSE;
            info.rasterizerDiscardEnable = VK_FALSE;

            // 线框和实体设置
            info.polygonMode = polygon_mode;
            info.lineWidth   = 1.0f;
            // 无面剔除
            info.cullMode  = VK_CULL_MODE_NONE;
            info.frontFace = VK_FRONT_FACE_CLOCKWISE;
            // 无深度偏差
            info.depthBiasEnable         = VK_FALSE;
            info.depthBiasConstantFactor = 0.0f;
            info.depthBiasClamp          = 0.0f;
            info.depthBiasSlopeFactor    = 0.0f;

            return info;
        }

        VkPipelineMultisampleStateCreateInfo MultiSamplingStateCreateInfo()
        {
            VkPipelineMultisampleStateCreateInfo info{};
            info.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
            info.pNext = nullptr;

            // 禁用MSAA
            info.sampleShadingEnable   = VK_FALSE;
            info.rasterizationSamples  = VK_SAMPLE_COUNT_1_BIT;
            info.minSampleShading      = 1.0f;
            info.pSampleMask           = nullptr;
            info.alphaToCoverageEnable = VK_FALSE;
            info.alphaToOneEnable      = VK_FALSE;

            return info;
        }

        VkPipelineColorBlendAttachmentState ColorBlendAttachmentState()
        {
            // 默认不混合
            VkPipelineColorBlendAttachmentState color_blend_attachment{};
            color_blend_attachment.colorWriteMask =
                VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
                VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
            color_blend_attachment.blendEnable = VK_FALSE;

            return color_blend_attachment;
        }

    } // namespace vkinit
} // namespace Yutrel