#include "Application.h"

#include "Frame.h"
#include "Gui.h"
#include "Log.h"
#include "Renderer.h"
#include "Swapchain.h"
#include "Window.h"

#include <string>

namespace Yutrel
{
    Application::Application(const ApplicationCreateInfo& info)
    {
        init(info);
    }

    Application::~Application()
    {
        shutdown();
    }

    void Application::init(const ApplicationCreateInfo& info)
    {
        LOG_INFO("Yutrel init {}", info.name);

        //---------renderer---------
        Renderer::CreateInfo renderer_ci{};
        renderer_ci.device_features.samplerAnisotropy   = vk::True;
        renderer_ci.device_features_13.synchronization2 = vk::True;
        renderer_ci.device_features_13.dynamicRendering = vk::True;
        m_renderer                                      = std::make_unique<Renderer>(renderer_ci);

        //----------窗口----------
        Window::CreateInfo window_ci{
            .width     = info.width,
            .height    = info.height,
            .title     = info.name,
            .callbacks = this,
        };

        m_window = std::make_unique<Window>(window_ci);

        //----------交换链------------
        Swapchain::CreateInfo swapchain_ci{
            .renderer = getRenderer(),
            .window   = getWindow(),
        };

        m_swapchain = std::make_unique<Swapchain>(swapchain_ci);

        //---------Gui--------------
        Gui::CreateInfo gui_ci{
            .renderer  = getRenderer(),
            .window    = getWindow(),
            .swapchain = getSwapchain(),
        };

        m_gui = std::make_unique<Gui>(gui_ci);
    }

    void Application::shutdown()
    {
        // 释放所有组件
        for (auto& c : m_components)
        {
            c->onDetach();
        }

        m_gui.release();
        m_swapchain.release();
        m_renderer.release();
        m_window.release();
    }

    void Application::run()
    {
        // Main Loop
        while (!m_window->shouldClose())
        {
            m_window->pollEvents();

            // 计算FPS
            {
                static double previous_seconds = getTime();
                static int frame_count         = 0;
                double current_seconds         = getTime();
                double elapsed_seconds         = current_seconds - previous_seconds;

                if (elapsed_seconds > 0.25)
                {
                    previous_seconds    = current_seconds;
                    double fps          = static_cast<double>(frame_count) / elapsed_seconds;
                    double ms_per_frame = 1000.0 / fps;

                    m_window->setTitle(std::format(" - {:.2f} ms/frame ({:.1f} FPS)", ms_per_frame, fps));

                    frame_count = 0;
                }

                frame_count++;
            }

            // todo resize

            // 重建渲染视口大小
            {
                const bool b_need_resize = (m_window->getWidth() != m_viewport_width) || (m_window->getHeight() != m_viewport_height);
                if (b_need_resize)
                {
                    m_viewport_width  = m_window->getWidth();
                    m_viewport_height = m_window->getHeight();
                    for (auto& c : m_components)
                    {
                        c->onResize(m_viewport_width, m_viewport_height);
                    }
                }
            }

            // UI更新
            {
                for (auto& c : m_components)
                {
                    if (auto c_ui = dynamic_cast<ComponentWithUIBase*>(c.get()))
                    {
                        m_gui->updateUI(
                            [c_ui]()
                            {
                                c_ui->onUIUpdate();
                            });
                    }
                }
            }

            // 渲染一帧
            {
                auto cur_frame = m_renderer->prepareBeforeRender();

                m_swapchain->acquireNextImage(cur_frame->getAvailableForRenderSemaphore());

                auto cmd_buffer = cur_frame->beginCommandBuffer();

                for (auto& c : m_components)
                {
                    c->onRender(cmd_buffer);
                }

                m_gui->drawUI(cmd_buffer);

                m_renderer->submitRendering(cur_frame);

                m_swapchain->present(cur_frame->getFinishedForPresentationSemaphore());
            }
        }
    }

    double Application::getTime() const
    {
        return m_window->getTime();
    }

    void Application::addComponent(std::unique_ptr<ComponentBase> component)
    {
        component->onAttach(this);
        m_components.emplace_back(std::move(component));
    }

    void Application::handleWindowSizeChange()
    {
        // YUTREL_ASSERT(m_renderer && m_window && m_swapchain, "");
    }

} // namespace Yutrel