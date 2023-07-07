
#include "glm/ext/matrix_clip_space.hpp"
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

        RenderData render_data;
        render_data.plane_model   = Model::Create("../resource/object/plane/plane.obj");
        render_data.cube_model    = Model::Create("../resource/object/cube/cube.obj");
        render_data.skybox_shader = Shader::Create("../Engine/asset/shader/skybox.vert", "../Engine/asset/shader/skybox.frag");

        Application::Get().GetWorld().SetResource(RenderData{render_data});
    }

    /*
     * 固定的渲染管线
     * 但是说实话没有什么卵用
     * 主要的渲染还是放到sandbox里吧
     */
    void OpenGLRenderer::RenderScene(Entity shader_entity, Entity camera_controller_entity)
    {
        auto querier  = Querier(Application::Get().GetWorld());
        auto resource = Resources(Application::Get().GetWorld());

        // 摄像机
        auto camera          = querier.Get<CameraController*>(camera_controller_entity)[0].GetCamera();
        glm::mat4 view       = camera.getViewMatrix();
        glm::mat4 projection = camera.getProjectionMatrix();

        //-----------------------------
        // 渲染场景
        //-----------------------------

        auto shader = querier.Get<Shader*>(shader_entity);
        shader->Use();
        shader->setMat4("u_view", view);
        shader->setMat4("u_projection", projection);

        auto scene_entities = querier.Query<Scene>();
        for (auto entity : scene_entities)
        {
            if (querier.Has<Model*>(entity))
            {
                auto model        = querier.Get<Model*>(entity);
                auto model_matrix = querier.Get<Transform>(entity).model_matrix;
                shader->setMat4("u_model", model_matrix);
                model->Draw(shader);
            }
        }
    }

    void OpenGLRenderer::RenderSkybox(Entity skybox, Entity camera_controller_entity)
    {
        auto querier = Querier(Application::Get().GetWorld());
        auto data    = Application::Get().GetWorld().GetResource<RenderData>();
        auto texture = querier.Get<SkyBox>(skybox).texture;
        auto shader  = data->skybox_shader;
        auto model   = data->cube_model;
        auto camera  = querier.Get<CameraController*>(camera_controller_entity)->GetCamera();

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

    void OpenGLRenderer::RenderUI(WindowUI* ui)
    {
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        ui->RenderUI();
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }

    void OpenGLRenderer::InitializeUIRender(WindowUI* window_ui)
    {
        Resources resources = Resources(Application::Get().GetWorld());
        ImGui_ImplGlfw_InitForOpenGL(static_cast<GLFWwindow*>(resources.Get<Window*>()->GetWindow()), true);
        ImGui_ImplOpenGL3_Init("#version 460");
    }

    void OpenGLRenderer::ClearUIRender(WindowUI* window_ui)
    {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
    }

} // namespace Yutrel