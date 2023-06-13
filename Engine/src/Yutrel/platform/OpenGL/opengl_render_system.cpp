#include "glm/ext/matrix_float4x4.hpp"
#include "glm/ext/matrix_transform.hpp"
#include "glm/ext/quaternion_trigonometric.hpp"
#include "glm/trigonometric.hpp"
#include "yutrel_pch.h"

#include "opengl_render_system.h"

#include "Yutrel/function/window/window_system.h"

#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <imgui_internal.h>
#include <stdint.h>

namespace Yutrel
{
    OpenGLRenderSystem::~OpenGLRenderSystem()
    {
        clear();
    }

    void OpenGLRenderSystem::initialize(RenderSystemInitInfo render_init_info)
    {
        // 窗口
        glfwMakeContextCurrent(static_cast<GLFWwindow *>(g_runtime_global_context.m_window_system->getWindow()));

        // 初始化GLAD
        gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
    }

    void OpenGLRenderSystem::renderUI(std::shared_ptr<WindowUI> ui)
    {
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        ui->preRender();
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }

    void OpenGLRenderSystem::initializeUIRender(WindowUI *window_ui)
    {
        ImGui_ImplGlfw_InitForOpenGL(static_cast<GLFWwindow *>(g_runtime_global_context.m_window_system->getWindow()), true);
        ImGui_ImplOpenGL3_Init("#version 460");
    }

    void OpenGLRenderSystem::clearUIRender(WindowUI *window_ui)
    {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
    }

} // namespace Yutrel