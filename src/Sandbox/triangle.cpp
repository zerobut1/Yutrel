#include "triangle.h"

using namespace Yutrel;

void Triangle::onAttach(Yutrel::Application* app)
{
    m_app      = app;
    m_renderer = app->getRenderer();

    Yutrel::RenderTarget::CreateInfo rt_ci{};
    rt_ci.extent        = vk::Extent2D{1920, 1080};
    rt_ci.depth_format  = vk::Format::eD32Sfloat;
    rt_ci.color_formats = std::vector<vk::Format>{vk::Format::eR16G16B16A16Sfloat};
    m_main_rt           = Yutrel::RenderTarget::create(m_renderer, rt_ci);
}

void Triangle::onDetach()
{
}

void Triangle::onRender(vk::CommandBuffer cmd_buffer)
{
    auto swapchain = m_app->getSwapchain();

    m_renderer->transitionImageLayout(cmd_buffer,
                                      swapchain->getCurrentImage(),
                                      vk::ImageLayout::eUndefined,
                                      vk::ImageLayout::ePresentSrcKHR);
}

void Triangle::onResize(uint32_t width, uint32_t height)
{
    m_viewport_width  = width;
    m_viewport_height = height;
}
