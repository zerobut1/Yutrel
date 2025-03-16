#include "Triangle.h"

#include "Renderer.h"
#include "Swapchain.h"

using namespace Yutrel;

void Triangle::onAttach(Yutrel::Application* app)
{
    m_app      = app;
    m_renderer = app->getRenderer();

    Yutrel::RenderTarget::CreateInfo rt_ci{};
    rt_ci.extent = vk::Extent2D{1920, 1080};
    rt_ci.format = vk::Format::eR16G16B16A16Sfloat;
    rt_ci.layout = vk::ImageLayout::eGeneral;
    m_main_rt    = Yutrel::RenderTarget::create(m_renderer, rt_ci);
}

void Triangle::onDetach()
{
}

void Triangle::onRender(vk::CommandBuffer cmd_buffer)
{
    auto swapchain = m_app->getSwapchain();

    m_main_rt->transitionImageLayout(cmd_buffer, vk::ImageLayout::eGeneral);

    draw(cmd_buffer);

    m_main_rt->transitionImageLayout(cmd_buffer, vk::ImageLayout::eTransferSrcOptimal);

    m_renderer->transitionImageLayout(cmd_buffer,
                                      swapchain->getCurrentImage(),
                                      vk::ImageLayout::eUndefined,
                                      vk::ImageLayout::eTransferDstOptimal);

    m_renderer->copyImageToImage(cmd_buffer,
                                 m_main_rt->getImage(),
                                 swapchain->getCurrentImage(),
                                 m_main_rt->getExtent(),
                                 swapchain->getExtent());

    m_renderer->transitionImageLayout(cmd_buffer,
                                      swapchain->getCurrentImage(),
                                      vk::ImageLayout::eTransferDstOptimal,
                                      vk::ImageLayout::ePresentSrcKHR);
}

void Triangle::onResize(uint32_t width, uint32_t height)
{
    m_viewport_width  = width;
    m_viewport_height = height;
}

void Triangle::draw(vk::CommandBuffer cmd_buffer)
{
    auto clear_value =
        vk::ClearColorValue()
            .setFloat32({0.4, 0.8, 1.0, 0.0});

    auto clear_range = vk::ImageSubresourceRange{vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1};

    cmd_buffer.clearColorImage(m_main_rt->getImage(), m_main_rt->getLayout(), clear_value, clear_range);
}