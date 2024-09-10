#include "triangle.h"

using namespace Yutrel;

void Triangle::onAttach(Yutrel::Application* app)
{
    m_app      = app;
    m_renderer = app->getRenderer();

    createDrawImage();
}

void Triangle::onDetach()
{
}

void Triangle::onRender(vk::CommandBuffer cmd_buffer)
{
    // auto swapchain = m_renderer->getSwapchain();

    // m_renderer->transitionImageLayout(cmd_buffer,
    //                                   swapchain->getCurrentImage(),
    //                                   vk::ImageLayout::eUndefined,
    //                                   vk::ImageLayout::ePresentSrcKHR);
}

void Triangle::createDrawImage()
{

}
