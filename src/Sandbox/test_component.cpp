#include "test_component.h"

using namespace Yutrel;

void TestComponent::onAttach(Yutrel::Application* app)
{
    m_app = app;
}

void TestComponent::onDetach()
{
}

void TestComponent::onRender(vk::CommandBuffer cmd_buffer)
{
    auto renderer  = m_app->getRenderer();
    auto swapchain = renderer->getSwapchain();

    renderer->transitionImageLayout(cmd_buffer,
                                    swapchain->getCurrentImage(),
                                    vk::ImageLayout::eUndefined,
                                    vk::ImageLayout::ePresentSrcKHR);
}