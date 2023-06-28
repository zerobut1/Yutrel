
#include "yutrel_pch.hpp"

#include "opengl_renderer.hpp"

#include "Yutrel/function/window/window.hpp"

#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <glm/ext/matrix_float4x4.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/quaternion_trigonometric.hpp>
#include <glm/trigonometric.hpp>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <imgui_internal.h>

#include <stdint.h>

namespace Yutrel
{
    OpenGLRenderer::OpenGLRenderer(Window* window)
    {
        Initialize(window);
    }

    OpenGLRenderer::~OpenGLRenderer()
    {
        Clear();
    }

    void OpenGLRenderer::Initialize(Window* window)
    {
        LOG_INFO("Initlize OpenGL Renderer!");
        // 窗口
        glfwMakeContextCurrent(static_cast<GLFWwindow*>(window->GetWindow()));

        // 初始化GLAD
        gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
    }

    void OpenGLRenderer::RenderUI(std::shared_ptr<WindowUI> ui)
    {
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        ui->preRender();
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }

    void OpenGLRenderer::InitializeUIRender(WindowUI* window_ui)
    {
        // ImGui_ImplGlfw_InitForOpenGL(static_cast<GLFWwindow *>(g_runtime_global_context.m_window_system->getWindow()), true);
        ImGui_ImplOpenGL3_Init("#version 460");
    }

    void OpenGLRenderer::ClearUIRender(WindowUI* window_ui)
    {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
    }

} // namespace Yutrel