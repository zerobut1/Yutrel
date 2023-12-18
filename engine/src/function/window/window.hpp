/**
 * 窗口
 * 与平台有关
 * 默认资源之一
 * 提供窗口的创建以及更新
 * 还有事件机制
 * todo 优化事件机制
 * todo 多窗口？
 */
#pragma once

#include "core/macro.hpp"

#include <gecs/gecs.hpp>

#include <functional>
#include <string>

namespace Yutrel
{
    class Window
    {
    public:
        virtual void Tick() const                       = 0;
        virtual void SetTitle(const std::string& title) = 0;
        virtual bool ShouldClose() const                = 0;
        virtual void* GetRaw() const                    = 0;
        virtual uint32_t Getwidth() const               = 0;
        virtual uint32_t GetHeight() const              = 0;

        /**
         *  Event也一并写到了window类中
         */
    public:
        typedef std::function<void()> OnResetFunc;
        typedef std::function<void(int, int, int, int)> OnKeyFunc;
        typedef std::function<void(unsigned int)> OnCharFunc;
        typedef std::function<void(int, unsigned int)> OnCharModsFunc;
        typedef std::function<void(int, int, int)> OnMouseButtonFunc;
        typedef std::function<void(double, double)> OnCursorPosFunc;
        typedef std::function<void(int)> OnCursorEnterFunc;
        typedef std::function<void(double, double)> OnScrollFunc;
        typedef std::function<void(int, const char**)> OnDropFunc;
        typedef std::function<void(int, int)> OnWindowSizeFunc;
        typedef std::function<void()> OnWindowCloseFunc;

        void RegisterOnResetFunc(OnResetFunc func) { m_OnResetFunc.push_back(func); }
        void RegisterOnKeyFunc(OnKeyFunc func) { m_OnKeyFunc.push_back(func); }
        void RegisterOnCharFunc(OnCharFunc func) { m_OnCharFunc.push_back(func); }
        void RegisterOnCharModsFunc(OnCharModsFunc func) { m_OnCharModsFunc.push_back(func); }
        void RegisterOnMouseButtonFunc(OnMouseButtonFunc func) { m_OnMouseButtonFunc.push_back(func); }
        void RegisterOnCursorPosFunc(OnCursorPosFunc func) { m_OnCursorPosFunc.push_back(func); }
        void RegisterOnCursorEnterFunc(OnCursorEnterFunc func) { m_OnCursorEnterFunc.push_back(func); }
        void RegisterOnScrollFunc(OnScrollFunc func) { m_OnScrollFunc.push_back(func); }
        void RegisterOnDropFunc(OnDropFunc func) { m_OnDropFunc.push_back(func); }
        void RegisterOnWindowSizeFunc(OnWindowSizeFunc func) { m_OnWindowSizeFunc.push_back(func); }
        void RegisterOnWindowCloseFunc(OnWindowCloseFunc func) { m_OnWindowCloseFunc.push_back(func); }

    protected:
        void OnReset()
        {
            for (auto& func : m_OnResetFunc)
                func();
        }
        void OnKey(int key, int scancode, int action, int mods)
        {
            for (auto& func : m_OnKeyFunc)
                func(key, scancode, action, mods);
        }
        void OnChar(unsigned int codepoint)
        {
            for (auto& func : m_OnCharFunc)
                func(codepoint);
        }
        void OnCharMods(int codepoint, unsigned int mods)
        {
            for (auto& func : m_OnCharModsFunc)
                func(codepoint, mods);
        }
        void OnMouseButton(int button, int action, int mods)
        {
            for (auto& func : m_OnMouseButtonFunc)
                func(button, action, mods);
        }
        void OnCursorPos(double xpos, double ypos)
        {
            for (auto& func : m_OnCursorPosFunc)
                func(xpos, ypos);
        }
        void OnCursorEnter(int entered)
        {
            for (auto& func : m_OnCursorEnterFunc)
                func(entered);
        }
        void OnScroll(double xoffset, double yoffset)
        {
            for (auto& func : m_OnScrollFunc)
                func(xoffset, yoffset);
        }
        void OnDrop(int count, const char** paths)
        {
            for (auto& func : m_OnDropFunc)
                func(count, paths);
        }
        void OnWindowSize(int width, int height)
        {
            for (auto& func : m_OnWindowSizeFunc)
                func(width, height);
        }

    protected:
        std::vector<OnResetFunc> m_OnResetFunc;
        std::vector<OnKeyFunc> m_OnKeyFunc;
        std::vector<OnCharFunc> m_OnCharFunc;
        std::vector<OnCharModsFunc> m_OnCharModsFunc;
        std::vector<OnMouseButtonFunc> m_OnMouseButtonFunc;
        std::vector<OnCursorPosFunc> m_OnCursorPosFunc;
        std::vector<OnCursorEnterFunc> m_OnCursorEnterFunc;
        std::vector<OnScrollFunc> m_OnScrollFunc;
        std::vector<OnDropFunc> m_OnDropFunc;
        std::vector<OnWindowSizeFunc> m_OnWindowSizeFunc;
        std::vector<OnWindowCloseFunc> m_OnWindowCloseFunc;
    };

    /*
     * 窗口组件
     * 避免将抽象类放入资源中
     */
    class WindowResource final
    {
    public:
        static void Update(gecs::resource<WindowResource> window);

    public:
        WindowResource(const std::string& title, uint32_t width, uint32_t height);
        ~WindowResource();

        Ref<Window> GetWindow() const { return m_window; }

    private:
        Ref<Window> m_window;
    };
} // namespace Yutrel