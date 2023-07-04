
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
    // extern RenderData

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

        // 设置全局通用的数据
        Application::Get().GetWorld().SetResource(
            RenderData{
                Model::Create("../resource/object/cube/cube.obj"),
                Shader::Create("../Engine/asset/shader/skybox.vert", "../Engine/asset/shader/skybox.frag")});
    }

    // void OpenGLRenderer::RenderModel(const Model* model)
    // {
    //     for (auto mesh : model->GetMeshes())
    //     {

    //     }
    // }

    void OpenGLRenderer::RenderSkybox(Entity skybox, Entity camera_controller)
    {
        auto querier = Querier(Application::Get().GetWorld());
        auto data    = Application::Get().GetWorld().GetResource<RenderData>();
        auto texture = querier.Get<SkyBox>(skybox).texture;
        auto shader  = data->skybox_shader;
        auto model   = data->skybox_model;
        auto camera  = querier.Get<CameraController*>(camera_controller)->GetCamera();

        glDepthFunc(GL_LEQUAL);
        shader->Use();
        shader->setMat4("view", camera.getViewMatrix());
        shader->setMat4("projection", camera.getProjectionMatrix());
        texture->Bind();
        model->Draw(shader);
        glDepthFunc(GL_LESS);
    }

    void OpenGLRenderer::DrawIndexed(const VertexArray* vertexArray)
    {
        vertexArray->Bind();
        glDrawElements(GL_TRIANGLES, vertexArray->GetIndexBuffer()->GetCount(), GL_UNSIGNED_INT, nullptr);
        vertexArray->Unbind();
    }

    // void OpenGLRenderer::RenderUI(std::shared_ptr<WindowUI> ui)
    // {
    //     ImGui_ImplOpenGL3_NewFrame();
    //     ImGui_ImplGlfw_NewFrame();
    //     ImGui::NewFrame();
    //     ui->preRender();
    //     ImGui::Render();
    //     ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    // }

    // void OpenGLRenderer::InitializeUIRender(WindowUI* window_ui)
    // {
    //     // ImGui_ImplGlfw_InitForOpenGL(static_cast<GLFWwindow *>(g_runtime_global_context.m_window_system->getWindow()), true);
    //     ImGui_ImplOpenGL3_Init("#version 460");
    // }

    // void OpenGLRenderer::ClearUIRender(WindowUI* window_ui)
    // {
    //     ImGui_ImplOpenGL3_Shutdown();
    //     ImGui_ImplGlfw_Shutdown();
    //     ImGui::DestroyContext();
    // }

} // namespace Yutrel