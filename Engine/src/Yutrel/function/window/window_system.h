#pragma once

#include <functional>
#include <memory>
#include <vector>

struct GLFWwindow;

namespace Yutrel
{
    struct WindowCreateInfo
    {
        int width;
        int height;
        const char *title;
        bool is_fullscreen = false;
    };

    class WindowSystem
    {
    public:
        static std::shared_ptr<WindowSystem> Create();

        virtual void initialize(WindowCreateInfo create_info) = 0;
        virtual void tick() const                             = 0;
        virtual void setTitle(const char *title)              = 0;
        virtual bool shouldClose() const                      = 0;
        virtual void *getWindow() const                       = 0;
        virtual std::array<int, 2> getWindowSize() const      = 0;

    protected:
        int m_width;
        int m_height;

        //  Event也一并写到了window类中
    public:
        typedef std::function<void()> onResetFunc;
        typedef std::function<void(int, int, int, int)> onKeyFunc;
        typedef std::function<void(unsigned int)> onCharFunc;
        typedef std::function<void(int, unsigned int)> onCharModsFunc;
        typedef std::function<void(int, int, int)> onMouseButtonFunc;
        typedef std::function<void(double, double)> onCursorPosFunc;
        typedef std::function<void(int)> onCursorEnterFunc;
        typedef std::function<void(double, double)> onScrollFunc;
        typedef std::function<void(int, const char **)> onDropFunc;
        typedef std::function<void(int, int)> onWindowSizeFunc;
        typedef std::function<void()> onWindowCloseFunc;

        std::vector<onResetFunc> m_onResetFunc;
        std::vector<onKeyFunc> m_onKeyFunc;
        std::vector<onCharFunc> m_onCharFunc;
        std::vector<onCharModsFunc> m_onCharModsFunc;
        std::vector<onMouseButtonFunc> m_onMouseButtonFunc;
        std::vector<onCursorPosFunc> m_onCursorPosFunc;
        std::vector<onCursorEnterFunc> m_onCursorEnterFunc;
        std::vector<onScrollFunc> m_onScrollFunc;
        std::vector<onDropFunc> m_onDropFunc;
        std::vector<onWindowSizeFunc> m_onWindowSizeFunc;
        std::vector<onWindowCloseFunc> m_onWindowCloseFunc;

        void registerOnResetFunc(onResetFunc func) { m_onResetFunc.push_back(func); }
        void registerOnKeyFunc(onKeyFunc func) { m_onKeyFunc.push_back(func); }
        void registerOnCharFunc(onCharFunc func) { m_onCharFunc.push_back(func); }
        void registerOnCharModsFunc(onCharModsFunc func) { m_onCharModsFunc.push_back(func); }
        void registerOnMouseButtonFunc(onMouseButtonFunc func) { m_onMouseButtonFunc.push_back(func); }
        void registerOnCursorPosFunc(onCursorPosFunc func) { m_onCursorPosFunc.push_back(func); }
        void registerOnCursorEnterFunc(onCursorEnterFunc func) { m_onCursorEnterFunc.push_back(func); }
        void registerOnScrollFunc(onScrollFunc func) { m_onScrollFunc.push_back(func); }
        void registerOnDropFunc(onDropFunc func) { m_onDropFunc.push_back(func); }
        void registerOnWindowSizeFunc(onWindowSizeFunc func) { m_onWindowSizeFunc.push_back(func); }
        void registerOnWindowCloseFunc(onWindowCloseFunc func) { m_onWindowCloseFunc.push_back(func); }

    protected:
        void onReset()
        {
            for (auto &func : m_onResetFunc)
                func();
        }
        void onKey(int key, int scancode, int action, int mods)
        {
            for (auto &func : m_onKeyFunc)
                func(key, scancode, action, mods);
        }
        void onChar(unsigned int codepoint)
        {
            for (auto &func : m_onCharFunc)
                func(codepoint);
        }
        void onCharMods(int codepoint, unsigned int mods)
        {
            for (auto &func : m_onCharModsFunc)
                func(codepoint, mods);
        }
        void onMouseButton(int button, int action, int mods)
        {
            for (auto &func : m_onMouseButtonFunc)
                func(button, action, mods);
        }
        void onCursorPos(double xpos, double ypos)
        {
            for (auto &func : m_onCursorPosFunc)
                func(xpos, ypos);
        }
        void onCursorEnter(int entered)
        {
            for (auto &func : m_onCursorEnterFunc)
                func(entered);
        }
        void onScroll(double xoffset, double yoffset)
        {
            for (auto &func : m_onScrollFunc)
                func(xoffset, yoffset);
        }
        void onDrop(int count, const char **paths)
        {
            for (auto &func : m_onDropFunc)
                func(count, paths);
        }
        void onWindowSize(int width, int height)
        {
            for (auto &func : m_onWindowSizeFunc)
                func(width, height);
        }
    };
} // namespace Yutrel