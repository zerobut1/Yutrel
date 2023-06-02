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
    // clang-format off
    float vertices_skybox[] = {
        // positions          
        -1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,
        1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,
        
        1.0f, -1.0f, -1.0f,
        1.0f, -1.0f,  1.0f,
        1.0f,  1.0f,  1.0f,
        1.0f,  1.0f,  1.0f,
        1.0f,  1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        1.0f,  1.0f,  1.0f,
        1.0f,  1.0f,  1.0f,
        1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

        -1.0f,  1.0f, -1.0f,
        1.0f,  1.0f, -1.0f,
        1.0f,  1.0f,  1.0f,
        1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
        1.0f, -1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
        1.0f, -1.0f,  1.0f
    };

    float vertices_plane[] = {
        5.0f,  -0.5f,  5.0f, 2.0f, 0.0f,
        -5.0f, -0.5f,  5.0f, 0.0f, 0.0f,
        -5.0f, -0.5f, -5.0f, 0.0f, 2.0f,
        5.0f, -0.5f,  5.0f, 2.0f, 0.0f,
        -5.0f, -0.5f, -5.0f, 0.0f, 2.0f, 
        5.0f,  -0.5f, -5.0f, 2.0f, 2.0f
    };

    float vertices_qurd[] = {
        // positions        // texture Coords
        -1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
        -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
        1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
        1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
    };
    // clang-format on
    constexpr uint32_t SHADOWMAP_SIZE = 1024;

    OpenGLRenderSystem::~OpenGLRenderSystem()
    {
        clear();
    }

    void OpenGLRenderSystem::initialize(RenderSystemInitInfo render_init_info)
    {
        // 窗口
        m_window = render_init_info.window_system->getglfwWindow();
        glfwMakeContextCurrent(m_window);
        m_viewport.width  = render_init_info.window_system->getWindowSize()[0];
        m_viewport.height = render_init_info.window_system->getWindowSize()[1];

        // 初始化GLAD
        assert(gladLoadGLLoader((GLADloadproc)glfwGetProcAddress));

        //------------camera---------
        m_camera_controller = CameraController::create(m_viewport.width / m_viewport.height, glm::vec3{0.0f, 1.0f, 4.0f});

        //---------skybox-----------
        m_skybox_VAO                             = VertexArray::create();
        std::shared_ptr<VertexBuffer> skybox_VBO = VertexBuffer::create(vertices_skybox, sizeof(vertices_skybox));
        skybox_VBO->setLayout({{Yutrel::ShaderDataType::Float3, "a_Pos"}});
        m_skybox_VAO->addVertexBuffer(skybox_VBO);

        m_skybox_shader = Shader::create("../Engine/asset/shader/skybox.vert", "../Engine/asset/shader/skybox.frag");

        m_skybox_texture = TextureCubemaps::create(
            {"../resource/texture/skybox/right.jpg",
             "../resource/texture/skybox/left.jpg",
             "../resource/texture/skybox/top.jpg",
             "../resource/texture/skybox/bottom.jpg",
             "../resource/texture/skybox/front.jpg",
             "../resource/texture/skybox/back.jpg"});

        //----------plane-------------------
        m_plane_VAO                             = VertexArray::create();
        std::shared_ptr<VertexBuffer> plane_VBO = VertexBuffer::create(vertices_plane, sizeof(vertices_plane));
        plane_VBO->setLayout({{Yutrel::ShaderDataType::Float3, "a_Pos"},
                              {Yutrel::ShaderDataType::Float2, "a_Pos"}});
        m_plane_VAO->addVertexBuffer(plane_VBO);

        m_plane_shader = Shader::create("../Engine/asset/shader/plane.vert", "../Engine/asset/shader/plane.frag");

        m_plane_texture = Texture2D::create("../resource/texture/marble.jpg");

        //------------bunny-----------
        m_bunny_model  = Model::create("../resource/object/bunny/bunny_iH.ply");
        m_bunny_shader = Shader::create("../Engine/asset/shader/bunny.vert", "../Engine/asset/shader/bunny.frag");

        //---------backpack------------
        // m_backpack_model = Model::create("../Engine/asset/object/backpack/backpack.obj");
        // m_bunny_model = Model::create("../Engine/asset/object/nanosuit/nanosuit.obj");

        //-------------shadowmap------------
        glGenFramebuffers(1, &m_shadowmap_framebuffer);
        // m_shadowmap_texture = Texture2D::create(SHADOWMAP_SIZE, SHADOWMAP_SIZE);

        glGenTextures(1, &m_shadowmap_texture);
        glBindTexture(GL_TEXTURE_2D, m_shadowmap_texture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOWMAP_SIZE, SHADOWMAP_SIZE, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        // glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_shadowmap_texture->getRnedererID(), 0);
        glBindFramebuffer(GL_FRAMEBUFFER, m_shadowmap_framebuffer);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_shadowmap_texture, 0);
        glDrawBuffer(GL_NONE);
        glReadBuffer(GL_NONE);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        m_shadowmap_shader = Shader::create("../Engine/asset/shader/shadowmap.vert", "../Engine/asset/shader/shadowmap.frag");

        //--------quad----------
    }

    void OpenGLRenderSystem::tick(float delta_time)
    {
        refreshOutputFrameBuffer();

        m_camera_controller->tick(delta_time, m_viewport.width / m_viewport.height);

        glm::vec3 lightPos(-2.0f, 2.0f, -1.0f);

        //-----------------------------
        // 渲染shadowmap
        //-----------------------------
        glBindFramebuffer(GL_FRAMEBUFFER, m_shadowmap_framebuffer);
        glViewport(0, 0, SHADOWMAP_SIZE, SHADOWMAP_SIZE);
        glClear(GL_DEPTH_BUFFER_BIT);

        glm::mat4 lightProjection  = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, 1.0f, 7.5f);
        glm::mat4 lightView        = glm::lookAt(lightPos, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        glm::mat4 lightSpaceMatrix = lightProjection * lightView;

        m_shadowmap_shader->Use();
        m_shadowmap_shader->setMat4("lightSpaceMatrix", lightSpaceMatrix);

        drawScene(m_shadowmap_shader);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        //-----------------------------
        // 渲染场景
        //-----------------------------
        glBindFramebuffer(GL_FRAMEBUFFER, m_output_framebuffer);
        glViewport(0, 0, m_viewport.width, m_viewport.height);
        // m_renderer_output->setSize(m_viewport.width, m_viewport.height);
        glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glEnable(GL_DEPTH_TEST);

        glm::mat4 model      = glm::mat4(1.0f);
        glm::mat4 view       = m_camera_controller->getCamera().getViewMatrix();
        glm::mat4 projection = m_camera_controller->getCamera().getProjectionMatrix();

        //-----------plane--------------
        m_plane_VAO->Bind();
        m_plane_shader->Use();
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, -0.5f, 0.0f));
        m_plane_shader->setMat4("model", model);
        m_plane_shader->setMat4("view", view);
        m_plane_shader->setMat4("projection", projection);
        m_plane_shader->setMat4("lightSpaceMatrix", lightSpaceMatrix);
        m_plane_shader->setFloat3("viewPos", m_camera_controller->getCamera().getPosition());
        m_plane_shader->setFloat3("lightPos", lightPos);
        m_plane_texture->Bind(0);
        // m_shadowmap_texture->Bind(1);
        glBindTextureUnit(1, m_shadowmap_texture);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        m_plane_shader->unUse();
        m_plane_VAO->Unbind();

        //------------bunny--------------
        m_bunny_shader->Use();
        model = glm::mat4(1.0f);
        model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));
        m_bunny_shader->setMat4("model", model);
        m_bunny_shader->setMat4("view", view);
        m_bunny_shader->setMat4("projection", projection);
        m_bunny_shader->setMat4("lightSpaceMatrix", lightSpaceMatrix);
        m_bunny_shader->setFloat3("viewPos", m_camera_controller->getCamera().getPosition());
        m_bunny_shader->setFloat3("lightPos", lightPos);
        glBindTextureUnit(0, m_shadowmap_texture);
        // m_shadowmap_texture->Bind();
        m_bunny_model->Draw();
        m_bunny_shader->unUse();

        //---------skybox--------------
        glDepthFunc(GL_LEQUAL);
        m_skybox_VAO->Bind();
        m_skybox_shader->Use();
        view = glm::mat4(glm::mat3(m_camera_controller->getCamera().getViewMatrix()));
        m_skybox_shader->setMat4("view", view);
        m_skybox_shader->setMat4("projection", projection);
        m_skybox_texture->Bind();
        glDrawArrays(GL_TRIANGLES, 0, 36);
        m_skybox_shader->unUse();
        m_skybox_VAO->Unbind();
        glDepthFunc(GL_LESS);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        if (m_ui)
        {
            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();
            m_ui->preRender();
            ImGui::Render();
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        }

        glfwSwapBuffers(m_window);
    }

    void OpenGLRenderSystem::drawScene(std::shared_ptr<Shader> &shader)
    {
        glm::mat4 model = glm::mat4(1.0f);
        shader->Use();
        //-----------plane--------------
        m_plane_VAO->Bind();
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, -0.5f, 0.0f));
        shader->setMat4("model", model);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        m_plane_VAO->Unbind();

        //------------bunny--------------
        model = glm::mat4(1.0f);
        model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));
        shader->setMat4("model", model);
        m_bunny_model->Draw();

        shader->unUse();
    }

    void OpenGLRenderSystem::refreshOutputFrameBuffer()
    {
        if (m_output_framebuffer)
        {
            glDeleteFramebuffers(1, &m_output_framebuffer);
            m_renderer_output.reset();
            glDeleteRenderbuffers(1, &m_output_renderbuffer);
        }

        glGenFramebuffers(1, &m_output_framebuffer);
        glBindFramebuffer(GL_FRAMEBUFFER, m_output_framebuffer);
        m_renderer_output = Texture2D::create(m_viewport.width, m_viewport.height);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_renderer_output->getRnedererID(), 0);

        glGenRenderbuffers(1, &m_output_renderbuffer);
        glBindRenderbuffer(GL_RENDERBUFFER, m_output_renderbuffer);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, m_viewport.width, m_viewport.height);
        glBindRenderbuffer(GL_RENDERBUFFER, 0);

        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_output_renderbuffer);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void OpenGLRenderSystem::initializeUIRenderBackend(WindowUI *window_ui)
    {
        m_ui = window_ui;

        ImGui_ImplGlfw_InitForOpenGL(m_window, true);
        ImGui_ImplOpenGL3_Init("#version 460");
    }

    void OpenGLRenderSystem::setEngineContentViewport(float offset_x, float offset_y, float width, float height)
    {
        m_viewport.x      = offset_x;
        m_viewport.y      = offset_y;
        m_viewport.width  = width;
        m_viewport.height = height;
    }

    EngineContentViewport OpenGLRenderSystem::getEngineContentViewport() const
    {
        return {m_viewport.x, m_viewport.y, m_viewport.width, m_viewport.height};
    }

    void OpenGLRenderSystem::clear()
    {
    }

} // namespace Yutrel