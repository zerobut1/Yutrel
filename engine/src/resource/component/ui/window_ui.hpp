/**
 * UI类
 * 留待具体的app中自己实现不同的UI
 */
#pragma once

#include <memory>

namespace Yutrel
{
    class WindowUI : public std::enable_shared_from_this<WindowUI>
    {
    public:
        virtual void RenderUI() = 0;
    };
} // namespace Yutrel