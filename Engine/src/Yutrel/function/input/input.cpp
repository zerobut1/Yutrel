#include "yutrel_pch.hpp"

#include "input.hpp"

#include "Yutrel/platform/Windows/windows_input.hpp"

namespace Yutrel
{
    Input* Input::Create()
    {
        return new WindowsInput();
    }
} // namespace Yutrel