#include "yutrel_pch.hpp"

#include "window.hpp"

#include "Yutrel/platform/Windows/windows_window.hpp"

namespace Yutrel
{
    void UpdateWindow(Commands& cmd, Querier querier, Resources resources, Events& events)
    {
        auto window = resources.Get<Window*>();
        window->Tick();

        // if(window.shouldClose()){
        //     events.Writer<QuitEvent>().Write()
        // }
    }

    Window* Window::Create(std::string title, uint32_t width, uint32_t height)
    {
        // 暂时直接使用windows(glfw)
        return new WindowsWindow(title, width, height);
    }
} // namespace Yutrel