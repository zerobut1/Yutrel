#include "opengl_render_system.h"

#include "runtime/function/window/window_system.h"

#include <GLFW/glfw3.h>
#include <glad/glad.h>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <imgui_internal.h>

#include <iostream>
#include <memory>

namespace Yutrel
{
    OpenGLRenderSystem::~OpenGLRenderSystem()
    {
    }

    void OpenGLRenderSystem::initialize(RenderSystemInitInfo render_init_info)
    {

        m_window = render_init_info.window_system->getglfwWindow();
        glfwMakeContextCurrent(m_window);
        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
        {
            std::cout << "Failed to initialize GLAD" << std::endl;
        }

        m_test_shader = Shader::create("../Engine/asset/shader/test.vert", "../Engine/asset/shader/test.frag");
        // m_test_shader = Shader::create("../Engine/asset/shader/model.vert", "../Engine/asset/shader/model.frag");

        m_test_texture = Texture2D::create("D:/PROJECT/Yutrel/Engine/asset/texture/marble.jpg");
        m_test_shader->Use();
        m_test_shader->setInt("texture", 0);

        // set up vertex data (and buffer(s)) and configure vertex attributes
        // ------------------------------------------------------------------
        float vertices[] = {
            -0.5f,
            -0.5f,
            0.0f,
            0.0f,
            0.0f, // left
            0.5f,
            -0.5f,
            0.0f,
            1.0f,
            0.0f,
            // right
            0.0f,
            0.5f,
            0.0f,
            0.5f,
            1.0f // top
        };

        m_test_VA                             = VertexArray::create();
        std::shared_ptr<VertexBuffer> test_VB = VertexBuffer::create(vertices, sizeof(vertices));
        test_VB->setLayout({{Yutrel::ShaderDataType::Float3, "a_Pos"},
                            {Yutrel::ShaderDataType::Float2, "a_TexCoord"}});
        m_test_VA->addVertexBuffer(test_VB);


        m_test_model = Model::create("../Engine/asset/object/nanosuit/nanosuit.obj");

    }

    void OpenGLRenderSystem::tick(float delta_time)
    {
        refreshFrameBuffer();

        glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
        glViewport(0, 0, m_viewport.width, m_viewport.height);
        glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // draw our first triangle
        

        // m_test_shader->Use();
        // glm::mat4 projection = glm::perspective(glm::radians(45.0f), 1920.0f/1080.0f, 0.1f, 100.0f);
        // glm::mat4 view       = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));
        // m_test_shader->setMat4("projection", projection);
        // m_test_shader->setMat4("view", view);

        // // render the loaded model
        // glm::mat4 model = glm::mat4(1.0f);
        // model           = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f)); // translate it down so it's at the center of the scene
        // model           = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
        // m_test_shader->setMat4("model", model);
        // m_test_model->Draw();

        m_test_shader->Use();
        m_test_texture->Bind();
        m_test_VA->Bind();
        glDrawArrays(GL_TRIANGLES, 0, 3);

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

    void OpenGLRenderSystem::initializeUIRenderBackend(WindowUI *window_ui)
    {
        m_ui = window_ui;

        ImGui_ImplGlfw_InitForOpenGL(m_window, true);
        ImGui_ImplOpenGL3_Init("#version 460");
    }

    void OpenGLRenderSystem::updateEngineContentViewport(float offset_x, float offset_y, float width, float height)
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
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, m_viewport.width, m_viewport.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glBindTexture(GL_TEXTURE_2D, 0);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texColorBuffer, 0);

        glGenRenderbuffers(1, &texDepthBuffer);
        glBindRenderbuffer(GL_RENDERBUFFER, texDepthBuffer);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, m_viewport.width, m_viewport.height);
        glBindRenderbuffer(GL_RENDERBUFFER, 0);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, texDepthBuffer);

        // if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        //     LOG_WARN("framebuffer is not complete");

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void OpenGLRenderSystem::clear()
    {
        // 暂时为空
    }
} // namespace Yutrel