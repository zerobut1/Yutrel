#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <functional>
#include <string>
#include <vector>

namespace Yutrel
{
    struct WindowCreateInfo
    {
        uint32_t width{1920};
        uint32_t height{1080};
        std::string title{"Yutrel App"};
        bool is_fullscreen{false};
    };

    class Window final
    {
    public:
        Window() = delete;
        explicit Window(const WindowCreateInfo& info);
        ~Window();

        Window(const Window&)            = delete;
        Window& operator=(const Window&) = delete;

        void init(const WindowCreateInfo& info);
        void shutdown();

        void pollEvents() const;
        bool shouldClose() const;
        void setTitle(const std::string& title);

        GLFWwindow* getWindow() const { return m_window; };
        uint32_t getwidth() const { return m_width; };
        uint32_t getHeight() const { return m_height; };

    public:
        // 注册事件
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

    private:
        static void errorCallback(int error, const char* description);

        // window event callbacks
        static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
        static void charCallback(GLFWwindow* window, unsigned int codepoint);
        static void charModsCallback(GLFWwindow* window, unsigned int codepoint, int mods);
        static void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
        static void cursorPosCallback(GLFWwindow* window, double xpos, double ypos);
        static void cursorEnterCallback(GLFWwindow* window, int entered);
        static void scrollCallback(GLFWwindow* window, double xoffset, double yoffset);
        static void dropCallback(GLFWwindow* window, int count, const char** paths);
        static void windowSizeCallback(GLFWwindow* window, int width, int height);
        static void windowCloseCallback(GLFWwindow* window);

        void onKey(int key, int scancode, int action, int mods);
        void onChar(unsigned int codepoint);
        void onCharMods(int codepoint, unsigned int mods);
        void onMouseButton(int button, int action, int mods);
        void onCursorPos(double xpos, double ypos);
        void onCursorEnter(int entered);
        void onScroll(double xoffset, double yoffset);
        void onDrop(int count, const char** paths);
        void onWindowSize(int width, int height);

    private:
        GLFWwindow* m_window{nullptr};
        uint32_t m_width{0};
        uint32_t m_height{0};

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
