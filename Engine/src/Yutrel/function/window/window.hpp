#pragma once

#include "Yutrel/core/ecs/ecs.hpp"

#include <functional>
#include <iostream>
#include <memory>
#include <stdint.h>
#include <string>
#include <utility>
#include <vector>

struct GLFWwindow;

namespace Yutrel
{
    /*
     * 窗口
     */
    class Window
    {
    public:
        static Window* Create(std::string title, uint32_t width, uint32_t height);
        static void UpdateWindow(Commands& cmd, Querier querier, Resources resources, Events& events);

        virtual void Tick() const                             = 0;
        virtual void SetTitle(const char* title)              = 0;
        virtual bool ShouldClose() const                      = 0;
        virtual void* GetWindow() const                       = 0;
        virtual std::pair<float, float> GetWindowSize() const = 0;

        /**
         *  Event也一并写到了window类中
         *  目前还是采用这一套event
         *  ecs的event主要用于app内不同系统的通信
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

        void registerOnResetFunc(OnResetFunc func) { m_OnResetFunc.push_back(func); }
        void registerOnKeyFunc(OnKeyFunc func) { m_OnKeyFunc.push_back(func); }
        void registerOnCharFunc(OnCharFunc func) { m_OnCharFunc.push_back(func); }
        void registerOnCharModsFunc(OnCharModsFunc func) { m_OnCharModsFunc.push_back(func); }
        void registerOnMouseButtonFunc(OnMouseButtonFunc func) { m_OnMouseButtonFunc.push_back(func); }
        void registerOnCursorPosFunc(OnCursorPosFunc func) { m_OnCursorPosFunc.push_back(func); }
        void registerOnCursorEnterFunc(OnCursorEnterFunc func) { m_OnCursorEnterFunc.push_back(func); }
        void registerOnScrollFunc(OnScrollFunc func) { m_OnScrollFunc.push_back(func); }
        void registerOnDropFunc(OnDropFunc func) { m_OnDropFunc.push_back(func); }
        void registerOnWindowSizeFunc(OnWindowSizeFunc func) { m_OnWindowSizeFunc.push_back(func); }
        void registerOnWindowCloseFunc(OnWindowCloseFunc func) { m_OnWindowCloseFunc.push_back(func); }

    protected:
        void onReset()
        {
            for (auto& func : m_OnResetFunc)
                func();
        }
        void onKey(int key, int scancode, int action, int mods)
        {
            for (auto& func : m_OnKeyFunc)
                func(key, scancode, action, mods);
        }
        void onChar(unsigned int codepoint)
        {
            for (auto& func : m_OnCharFunc)
                func(codepoint);
        }
        void onCharMods(int codepoint, unsigned int mods)
        {
            for (auto& func : m_OnCharModsFunc)
                func(codepoint, mods);
        }
        void onMouseButton(int button, int action, int mods)
        {
            for (auto& func : m_OnMouseButtonFunc)
                func(button, action, mods);
        }
        void onCursorPos(double xpos, double ypos)
        {
            for (auto& func : m_OnCursorPosFunc)
                func(xpos, ypos);
        }
        void onCursorEnter(int entered)
        {
            for (auto& func : m_OnCursorEnterFunc)
                func(entered);
        }
        void onScroll(double xoffset, double yoffset)
        {
            for (auto& func : m_OnScrollFunc)
                func(xoffset, yoffset);
        }
        void onDrop(int count, const char** paths)
        {
            for (auto& func : m_OnDropFunc)
                func(count, paths);
        }
        void onWindowSize(int width, int height)
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
} // namespace Yutrel