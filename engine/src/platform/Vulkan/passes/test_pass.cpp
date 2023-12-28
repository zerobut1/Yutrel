#include "yutrel_pch.hpp"

#include "test_pass.hpp"

#include "platform/Vulkan/initializers/initializers.hpp"
#include "platform/Vulkan/rhi/vulkan_rhi.hpp"

namespace Yutrel
{
    void TestPass::Init(RenderPassInitInfo* info)
    {
    }

    void TestPass::PreparePassData(Ref<struct RenderData> render_data)
    {
        m_render_data = render_data;
    }

    void TestPass::DrawForward()
    {
        //--------清除图像------------
        VkClearColorValue clear_value;
        float flash = abs(sin(m_rhi->GetCurrentFrameCount() / 10000.f));
        clear_value = {{0.0f, 0.0f, flash, 1.0f}};

        VkImageSubresourceRange clear_range = vkinit::ImageSubresourceRange(VK_IMAGE_ASPECT_COLOR_BIT);

        vkCmdClearColorImage(m_rhi->GetCurrentCommandBuffer(), m_rhi->GetCurrentSwapchainImage(), VK_IMAGE_LAYOUT_GENERAL, &clear_value, 1, &clear_range);
    }

} // namespace Yutrel