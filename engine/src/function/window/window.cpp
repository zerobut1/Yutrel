#include "yutrel_pch.hpp"

#include "window.hpp"

#include "platform/Windows/windows_window.hpp"

namespace Yutrel
{
    void WindowResource::Update(gecs::resource<WindowResource> window)
    {
        window->GetWindow()->Tick();
    }

    WindowResource::WindowResource(const std::string& title, uint32_t width, uint32_t height)
    {
        // 暂时直接使用windows
        m_window = CreateRef<WindowsWindow>(title, width, height);
    }

    WindowResource::~WindowResource()
    {
        LOG_INFO("Window Destroyed");
    }
} // namespace Yutrel