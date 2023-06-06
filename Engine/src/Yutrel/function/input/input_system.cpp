#include "yutrel_pch.h"

#include "input_system.h"

#include "Yutrel/platform/Windows/windows_input_system.h"

namespace Yutrel
{
    std::shared_ptr<InputSystem> InputSystem::Create()
    {
        return std::make_shared<WindowsInputSystem>();
    }
} // namespace Yutrel