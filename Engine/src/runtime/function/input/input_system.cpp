#include "input_system.h"

#include "runtime/platform/Windows/windows_input_system.h"

#include <memory>

namespace Yutrel
{
    std::shared_ptr<InputSystem> InputSystem::create()
    {
        return std::make_shared<WindowsInputSystem>();
    }
} // namespace Yutrel