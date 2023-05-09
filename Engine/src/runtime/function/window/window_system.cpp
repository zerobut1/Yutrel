#include "window_system.h"

#include "runtime/platform/Windows/windows_window_system.h"
#include <memory>

namespace Yutrel
{
    std::shared_ptr<WindowSystem> WindowSystem::create()
    {
        // 暂时直接使用windows(glfw)
        return std::make_shared<Windows_WindowSystem>();


        
    }   
} // namespace Yutrel