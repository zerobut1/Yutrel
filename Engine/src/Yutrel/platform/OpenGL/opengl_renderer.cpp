
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
        render_data.plane_model      = Model::Create("../resource/object/plane/plane.obj");
        render_data.cube_model       = Model::Create("../resource/object/cube/cube.obj");
        render_data.skybox_shader    = Shader::Create("../Engine/asset/shader/skybox.vert", "../Engine/asset/shader/skybox.frag");
        render_data.shadowmap_shader = Shader::Create("../Engine/asset/shader/shadowmap.vert", "../Engine/asset/shader/shadowmap.frag");
        render_data.shadow_shader    = Shader::Create("../Engine/asset/shader/shadow.vert", "../Engine/asset/shader/shadow.frag");

        FramebufferSpecification fbSpec;
        fbSpec.Attachments                = {FramebufferTextureFormat::DEPTH32};
        fbSpec.Width                      = 1024;
        fbSpec.Height                     = 1024;
        render_data.shadowmap_framebuffer = Framebuffer::Create(fbSpec);
        Application::Get().GetWorld().SetResource(RenderData{render_data});
    }

    void OpenGLRenderer::RenderScene(Entity camera_controller)
    {
        auto querier  = Querier(Application::Get().GetWorld());
        auto resource = Resources(Application::Get().GetWorld());

        //-----------------------------
        // 渲染shadowmap
        //-----------------------------
        auto shadowmap_framebuffer = resource.Get<RenderData>().shadowmap_framebuffer;
        auto shadowmap_shader      = resource.Get<RenderData>().shadowmap_shader;
        shadowmap_framebuffer->Bind();
        glClear(GL_DEPTH_BUFFER_BIT);
        shadowmap_shader->Use();
        auto light_entities          = querier.Query<Light>();
        auto directional_light       = querier.Get<Light>(light_entities[0]);
        glm::mat4 light_view         = glm::lookAt(directional_light.light_pos, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        glm::mat4 light_projection   = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, 1.0f, 100.0f);
        glm::mat4 light_space_matrix = light_projection * light_view;
        shadowmap_shader->setMat4("lightSpaceMatrix", light_space_matrix);

        auto scene_entities = querier.Query<Scene>();
        for (auto entity : scene_entities)
        {
            auto model     = querier.Get<Model*>(entity);
            auto transform = querier.Get<Transform>(entity).model_matrix;

            shadowmap_shader->setMat4("model", transform);
            model->Draw(shadowmap_shader);
        }

        shadowmap_framebuffer->Unbind();

        // camera
        auto camera          = querier.Get<CameraController*>(camera_controller)[0].GetCamera();
        glm::mat4 view       = camera.getViewMatrix();
        glm::mat4 projection = camera.getProjectionMatrix();

        //-----------------------------
        // 渲染场景
        //-----------------------------
        glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glEnable(GL_DEPTH_TEST);
        glViewport(0, 0, 1920, 1080);

        auto shader = resource.Get<RenderData>().shadow_shader;
        shader->Use();
        shader->setMat4("view", view);
        shader->setMat4("projection", projection);
        shader->setMat4("lightSpaceMatrix", light_space_matrix);
        shader->setFloat3("viewPos", camera.getPosition());
        shader->setFloat3("lightPos", directional_light.light_pos);

        // auto scene_entities = querier.Query<Scene>();
        for (auto entity : scene_entities)
        {
            auto model     = querier.Get<Model*>(entity);
            auto transform = querier.Get<Transform>(entity).model_matrix;
            shader->setMat4("model", transform);

            glBindTextureUnit(4, shadowmap_framebuffer->getColorAttachmentRendererID());

            model->Draw(shader);
        }
    }

    void OpenGLRenderer::RenderSkybox(Entity skybox, Entity camera_controller)
    {
        auto querier = Querier(Application::Get().GetWorld());
        auto data    = Application::Get().GetWorld().GetResource<RenderData>();
        auto texture = querier.Get<SkyBox>(skybox).texture;
        auto shader  = data->skybox_shader;
        auto model   = data->cube_model;
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