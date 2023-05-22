#include "yutrel_pch.h"

#include "opengl_render_system.h"

#include "runtime/function/window/window_system.h"

#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <imgui_internal.h>

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
    // clang-format on

    OpenGLRenderSystem::~OpenGLRenderSystem()
    {
    }

    void OpenGLRenderSystem::initialize(RenderSystemInitInfo render_init_info)
    {
        // 窗口
        m_window = render_init_info.window_system->getglfwWindow();
        glfwMakeContextCurrent(m_window);
        m_viewport.width  = render_init_info.window_system->getWindowSize()[0];
        m_viewport.height = render_init_info.window_system->getWindowSize()[1];

        // 初始化GLAD
        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
        {
            std::cout << "Failed to initialize GLAD" << std::endl;
        }

        //------------camera---------
        m_test_camera_controller = CameraController::create(m_viewport.width / m_viewport.height, glm::vec3{0.0f, 5.0f, 5.0f});

        //---------VAO-----------
        m_skybox_VAO                             = VertexArray::create();
        std::shared_ptr<VertexBuffer> skybox_VBO = VertexBuffer::create(vertices_skybox, sizeof(vertices_skybox));
        skybox_VBO->setLayout({{Yutrel::ShaderDataType::Float3, "a_Pos"}});
        m_skybox_VAO->addVertexBuffer(skybox_VBO);

        //------------shader-----------
        m_skybox_shader = Shader::create("../Engine/asset/shader/skybox.vert", "../Engine/asset/shader/skybox.frag");
        m_model_shader  = Shader::create("../Engine/asset/shader/model.vert", "../Engine/asset/shader/model.frag");

        //-----------texture------------
        m_skybox_texture = TextureCubemaps::create(
            {"../Engine/asset/texture/skybox/right.jpg",
             "../Engine/asset/texture/skybox/left.jpg",
             "../Engine/asset/texture/skybox/top.jpg",
             "../Engine/asset/texture/skybox/bottom.jpg",
             "../Engine/asset/texture/skybox/front.jpg",
             "../Engine/asset/texture/skybox/back.jpg"});

        //----------model----------
        m_test_model = Model::create("../Engine/asset/object/nanosuit/nanosuit.obj");
        // m_test_model = Model::create("../Engine/asset/object/backpack/backpack.obj");
        // m_test_model = Model::create("../Engine/asset/object/bunny/bunny_iH.ply");

        //---------------skybox-----------------
        // m_skybox_VAO =
    }

    void OpenGLRenderSystem::tick(float delta_time)
    {
        refreshFrameBuffer();

        m_test_camera_controller->tick(delta_time, m_viewport.width / m_viewport.height);

        glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
        glViewport(0, 0, m_viewport.width, m_viewport.height);
        glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glEnable(GL_DEPTH_TEST);

        //------------模型--------------
        m_model_shader->Use();
        glm::mat4 projection = m_test_camera_controller->getCamera().getProjectionMatrix();
        glm::mat4 view       = m_test_camera_controller->getCamera().getViewMatrix();
        m_model_shader->setMat4("projection", projection);
        m_model_shader->setMat4("view", view);
        glm::mat4 model = glm::mat4(1.0f);
        model           = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f)); // translate it down so it's at the center of the scene
        model           = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));
        m_model_shader->setMat4("model", model);
        m_test_model->Draw();

        //---------skybox--------------
        glDepthFunc(GL_LEQUAL);
        m_skybox_shader->Use();
        view = glm::mat4(glm::mat3(m_test_camera_controller->getCamera().getViewMatrix()));
        m_skybox_shader->setMat4("view", view);
        m_skybox_shader->setMat4("projection", projection);
        m_skybox_VAO->Bind();
        m_skybox_texture->Bind();
        glDrawArrays(GL_TRIANGLES, 0, 36);
        m_skybox_VAO->Bind();
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

    void OpenGLRenderSystem::refreshFrameBuffer()
    {
        if (framebuffer)
        {
            glDeleteFramebuffers(1, &framebuffer);
            glDeleteTextures(1, &texColorBuffer);
            glDeleteRenderbuffers(1, &texDepthBuffer);
        }
        glGenFramebuffers(1, &framebuffer);
        glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

        glGenTextures(1, &texColorBuffer);
        glBindTexture(GL_TEXTURE_2D, texColorBuffer);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, m_viewport.width, m_viewport.height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glBindTexture(GL_TEXTURE_2D, 0);

        // 将它附加到当前绑定的帧缓冲对象
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texColorBuffer, 0);

        glGenRenderbuffers(1, &texDepthBuffer);
        glBindRenderbuffer(GL_RENDERBUFFER, texDepthBuffer);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, m_viewport.width, m_viewport.height);
        glBindRenderbuffer(GL_RENDERBUFFER, 0);

        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, texDepthBuffer);

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

        // m_render_camera->setAspect(width / height);
        // m_viewer_camera->setAspect(width / height);
    }

    EngineContentViewport OpenGLRenderSystem::getEngineContentViewport() const
    {
        return {m_viewport.x, m_viewport.y, m_viewport.width, m_viewport.height};
    }

    void OpenGLRenderSystem::clear()
    {
        m_model_shader.reset();
        m_test_texture.reset();
        m_test_model.reset();
        m_test_camera_controller.reset();
    }

} // namespace Yutrel