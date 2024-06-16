#include "application.h"

#include "Core/log.h"
#include "Function/Render/renderer.h"
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

    void Application::init(const ApplicationCreateInfo& info)
    {
        //----------窗口----------
        WindowCreateInfo window_ci;
        window_ci.title  = info.name;
        window_ci.width  = info.width;
        window_ci.height = info.height;

        m_window = std::make_shared<Window>(window_ci);

        //---------renderer---------
        RendererCreateInfo renderer_ci;
        renderer_ci.window             = m_window->getWindow();
        renderer_ci.width              = info.width;
        renderer_ci.height             = info.height;
        renderer_ci.device_features    = info.device_features;
        renderer_ci.device_features_12 = info.device_features_12;
        renderer_ci.device_features_13 = info.device_features_13;

        m_renderer = std::make_shared<Renderer>(renderer_ci);
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

} // namespace Yutrel