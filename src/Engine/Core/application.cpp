#include "application.h"

#include "Core/log.h"
#include "Function/Render/renderer.h"
#include "Function/Render/swapchain.h"
#include "Function/Window/window.h"

namespace Yutrel
{
    Application::Application(const ApplicationCreateInfo& info)
    {
        try
        {
            init(info);
        }
        catch (const std::exception& e)
        {
            LOG_ERROR("std::exception: {}", e.what());
        };
    }

    Application::~Application()
    {
        try
        {
            shutdown();
        }
        catch (const std::exception& e)
        {
            LOG_ERROR("std::exception: {}", e.what());
        };
    }

    void Application::init(const ApplicationCreateInfo& info)
    {
        LOG_INFO("Yutrel init {}", info.name);

        //---------renderer---------
        Renderer::CreateInfo renderer_ci{};
        // todo 拓展
        m_renderer = std::make_shared<Renderer>(renderer_ci);

        //----------窗口----------
        Window::CreateInfo window_ci{};
        window_ci.title     = info.name;
        window_ci.width     = info.width;
        window_ci.height    = info.height;
        window_ci.callbacks = this;

        m_window = std::make_shared<Window>(window_ci);

        //----------交换链------------
        Swapchain::CreateInfo swapchain_ci{};
        swapchain_ci.renderer = m_renderer;
        swapchain_ci.window   = m_window;

        m_swapchain = std::make_shared<Swapchain>(swapchain_ci);
    }

    void Application::shutdown()
    {
        // 释放所有组件
        for (auto& c : m_components)
        {
            c->onDetach();
        }
        m_components.clear();
    }

    void Application::run()
    {
        try
        {
            while (!m_window->shouldClose())
            {
                m_window->pollEvents();

                auto cmd_buffer = m_renderer->prepareBeforeRender();

                for (auto& c : m_components)
                {
                    c->onRender(cmd_buffer);
                }

                m_renderer->submitRendering();

                m_renderer->framePresent();
            }
        }
        catch (const std::exception& e)
        {
            LOG_ERROR("std::exception: {}", e.what());
        };
    }

    void Application::addComponent(const std::shared_ptr<ComponentBase>& component)
    {
        m_components.emplace_back(component)->onAttach(this);
    }

    void Application::handleWindowSizeChange()
    {
        YUTREL_ASSERT(m_renderer && m_window && m_swapchain, "");
    }

} // namespace Yutrel