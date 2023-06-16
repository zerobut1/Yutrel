#include "editor.h"
#include "glm/fwd.hpp"

#include <Yutrel/Yutrel.h>

#include <glad/glad.h>
#include <glfw/glfw3.h>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_float4x4.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/quaternion_trigonometric.hpp>
#include <glm/trigonometric.hpp>

namespace Yutrel
{
    YutrelEditor *YutrelEditor::s_instance = nullptr;

    YutrelEditor::YutrelEditor(YutrelEngine *engine)
        : Application(engine)
    {
        s_instance = this;
    }

    void YutrelEditor::initialize()
    {
        // imgui
        m_ui = std::make_shared<EditorUI>();
        m_ui->initialize();

        //-----------camera----------------
        m_camera_controller = CameraController::Create((1920.0f / 1080.0f), glm::vec3(0.0f, 1.0f, 4.0f));

        //-------------viewport framebuffer------------
        FramebufferSpecification fbSpec;
        fbSpec.Attachments     = {FramebufferTextureFormat::RGBA8,
                                  FramebufferTextureFormat::DEPTH24STENCIL8};
        fbSpec.Width           = 1920;
        fbSpec.Height          = 1080;
        m_viewport_framebuffer = Framebuffer::Create(fbSpec);

        //-------------shadowmap------------
        fbSpec.Attachments      = {FramebufferTextureFormat::DEPTH32};
        fbSpec.Width            = 1024;
        fbSpec.Height           = 1024;
        m_shadowmap_framebuffer = Framebuffer::Create(fbSpec);
        m_shadowmap_shader      = Shader::Create("../Engine/asset/shader/shadowmap.vert", "../Engine/asset/shader/shadowmap.frag");
        m_shadow_shader         = Shader::Create("../Engine/asset/shader/shadow.vert", "../Engine/asset/shader/shadow.frag");

        //---------skybox-----------
        m_skybox_model  = Model::Create("../resource/object/cube/cube.obj");
        m_skybox_shader = Shader::Create("../Engine/asset/shader/skybox.vert", "../Engine/asset/shader/skybox.frag");
        // m_skybox_texture = TextureCubemaps::Create(
        //     {"../resource/texture/skybox/right.jpg",
        //      "../resource/texture/skybox/left.jpg",
        //      "../resource/texture/skybox/top.jpg",
        //      "../resource/texture/skysbox/bottom.jpg",
        //      "../resource/texture/skybox/front.jpg",
        //      "../resource/texture/skybox/back.jpg"});
        // m_skybox_texture = TextureCubemaps::Create("../resource/texture/hdr/kloppenheim.hdr");
        m_skybox_texture = TextureCubemaps::Create("../resource/texture/hdr/cayley_interior.hdr");

        //----------plane-------------------
        m_plane_model = Model::Create("../resource/object/plane/plane.obj");
        // m_plane_shader  = Shader::Create("../Engine/asset/shader/plane.vert", "../Engine/asset/shader/plane.frag");
        m_plane_texture = Texture2D::Create("../resource/texture/marble.jpg");

        //----------plane-------------------
        m_lightcube_model  = Model::Create("../resource/object/cube/cube.obj");
        m_lightcube_shader = Shader::Create("../Engine/asset/shader/lightcube.vert", "../Engine/asset/shader/lightcube.frag");
        // m_plane_texture = Texture2D::Create("../resource/texture/marble.jpg");

        //------------bunny-----------
        m_bunny_model = Model::Create("../resource/object/bunny/bunny.obj");
        // m_bunny_shader = Shader::Create("../Engine/asset/shader/bunny.vert", "../Engine/asset/shader/bunny.frag");

        // ---------backpack------------
        m_backpack_model = Model::Create("../resource/object/backpack/backpack.obj");
    }

    void YutrelEditor::tick(float delta_time)
    {
        // viewport有变化则resize
        if (FramebufferSpecification spec = m_viewport_framebuffer->getSpecification();
            m_viewport_size.x > 0.0f && m_viewport_size.y > 0.0f && // zero sized framebuffer is invalid
            (spec.Width != m_viewport_size.x || spec.Height != m_viewport_size.y))
        {
            m_viewport_framebuffer->resize((uint32_t)m_viewport_size.x, (uint32_t)m_viewport_size.y);
            // todo : camera resize
            m_camera_controller->resize(m_viewport_size.x / m_viewport_size.y);
        }

        // camera
        m_camera_controller->tick(delta_time);

        //-----------------------------
        // 渲染shadowmap
        //-----------------------------
        m_shadowmap_framebuffer->Bind();
        glClear(GL_DEPTH_BUFFER_BIT); // todo rendersystem

        m_shadowmap_shader->Use();
        glm::mat4 lightView        = glm::lookAt(lightPos, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        glm::mat4 lightSpaceMatrix = lightProjection * lightView;
        m_shadowmap_shader->setMat4("lightSpaceMatrix", lightSpaceMatrix);

        drawScene(m_shadowmap_shader);

        m_shadowmap_framebuffer->Unbind();

        //-----------------------------
        // 渲染场景
        //-----------------------------
        m_viewport_framebuffer->Bind();
        // todo:put in render system
        glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glEnable(GL_DEPTH_TEST);

        glm::mat4 model      = glm::mat4(1.0f);
        glm::mat4 view       = m_camera_controller->getCamera().getViewMatrix();
        glm::mat4 projection = m_camera_controller->getCamera().getProjectionMatrix();

        m_shadow_shader->Use();
        m_shadow_shader->setMat4("view", view);
        m_shadow_shader->setMat4("projection", projection);
        m_shadow_shader->setMat4("lightSpaceMatrix", lightSpaceMatrix);
        m_shadow_shader->setFloat3("viewPos", m_camera_controller->getCamera().getPosition());
        m_shadow_shader->setFloat3("lightPos", lightPos);

        //-----------plane--------------
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, -1.0f, 0.0f));
        model = glm::scale(model, glm::vec3(10.0f, 10.0f, 10.0f));
        m_shadow_shader->setMat4("model", model);
        glBindTextureUnit(0, m_shadowmap_framebuffer->getColorAttachmentRendererID());
        m_plane_texture->Bind(1);
        m_plane_model->Draw();

        //------------bunny--------------
        // model = glm::mat4(1.0f);
        // model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        // model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));
        // m_shadow_shader->setMat4("model", model);
        // glBindTextureUnit(0, m_shadowmap_framebuffer->getColorAttachmentRendererID());
        // m_bunny_model->Draw();

        //--------------backpack-------------
        m_shadow_shader->Use();
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, 1.0f, 0.0f));
        m_shadow_shader->setMat4("model", model);
        glBindTextureUnit(0, m_shadowmap_framebuffer->getColorAttachmentRendererID());
        m_backpack_model->Draw();

        //------------light cube-----------
        m_lightcube_shader->Use();
        model = glm::mat4(1.0f);
        model = glm::translate(model, lightPos);
        model = glm::scale(model, glm::vec3(0.1f));
        m_lightcube_shader->setMat4("model", model);
        m_lightcube_shader->setMat4("view", view);
        m_lightcube_shader->setMat4("projection", projection);
        m_lightcube_model->Draw();
        // m_lightcube_shader->unUse();

        //---------skybox--------------
        glDepthFunc(GL_LEQUAL);
        m_skybox_shader->Use();
        view = glm::mat4(glm::mat3(m_camera_controller->getCamera().getViewMatrix()));
        m_skybox_shader->setMat4("view", view);
        m_skybox_shader->setMat4("projection", projection);
        m_skybox_texture->Bind();
        m_skybox_model->Draw();
        m_skybox_shader->unUse();
        glDepthFunc(GL_LESS);

        m_viewport_framebuffer->Unbind();
    }

    void YutrelEditor::drawScene(std::shared_ptr<Shader> &shader)
    {
        glm::mat4 model = glm::mat4(1.0f);
        shader->Use();
        //-----------plane--------------
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, -1.0f, 0.0f));
        model = glm::scale(model, glm::vec3(10.0f, 10.0f, 10.0f));
        shader->setMat4("model", model);
        m_plane_model->Draw();

        //------------bunny--------------
        // model = glm::mat4(1.0f);
        // model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        // model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));
        // shader->setMat4("model", model);
        // m_bunny_model->Draw();

        //---------backpack---------------
        model = glm::mat4(1.0f);
        shader->setMat4("model", model);
        m_backpack_model->Draw();

        // shader->unUse();
    }

    void YutrelEditor::clear()
    {
        m_ui->clear();
    }
} // namespace Yutrel