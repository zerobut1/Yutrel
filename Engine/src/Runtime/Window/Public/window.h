#pragma once

#include <string>

struct GLFWwindow;

namespace Yutrel
{
    class Window final
    {
    public:
        class ICallbacks
        {
        public:
            virtual void handleWindowSizeChange() = 0;
        };

        struct CreateInfo
        {
            uint32_t width{1920};
            uint32_t height{1080};
            std::string title{"Yutrel App"};
            bool is_fullscreen{false};
            ICallbacks* callbacks;
        };

        enum class WindowMode
        {
            Normal,
            Minimized,
            FullScreen,
        };

    public:
        Window() = delete;
        explicit Window(const CreateInfo& info);
        ~Window();

        Window(const Window&)            = delete;
        Window& operator=(const Window&) = delete;

    public:
        void init(const CreateInfo& info);
        void shutdown();

        void pollEvents() const;
        bool shouldClose() const;
        void setTitle(const std::string& title);
        void resize(uint32_t width, uint32_t height);

        GLFWwindow* getWindow() const { return m_GLFW_window; };
        uint32_t getWidth() const { return m_width; };
        uint32_t getHeight() const { return m_height; };

    private:
        void updateWindowSize();
        void setWindowSize(uint32_t width, uint32_t height);

    private:
        static void errorCallback(int error, const char* description);
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

    private:
        GLFWwindow* m_GLFW_window{nullptr};
        uint32_t m_width{0};
        uint32_t m_height{0};
        WindowMode m_window_mode{WindowMode::Normal};
        ICallbacks* m_callbacks{nullptr};
    };
} // namespace Yutrel
