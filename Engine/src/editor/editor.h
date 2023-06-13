#pragma once

#include "editor/ui/editor_ui.h"

#include <Yutrel/Yutrel.h>

#include <memory>

namespace Yutrel
{
    class YutrelEditor : public Application
    {
    public:
        YutrelEditor(YutrelEngine *engine);
        virtual ~YutrelEditor() = default;

        virtual void initialize() override;
        virtual void tick(float delta_time) override;
        virtual void clear() override;

        static YutrelEditor &get() { return *s_instance; }

        // tmp
        void drawScene(std::shared_ptr<Shader> &shader);

    private:
        friend class EditorUI;

    private:
        static YutrelEditor *s_instance;

        std::shared_ptr<CameraController> m_camera_controller;

        std::shared_ptr<Framebuffer> m_viewport_framebuffer;
        glm::vec2 m_viewport_size = {1920.0f, 1080.0f};

        // 点光源
        glm::vec3 lightPos = {-2.0f, 2.0f, -1.0f};
        std::shared_ptr<Model> m_lightcube_model;
        std::shared_ptr<Shader> m_lightcube_shader;

        // shadowmap
        std::shared_ptr<Framebuffer> m_shadowmap_framebuffer;
        std::shared_ptr<Shader> m_shadowmap_shader;
        std::shared_ptr<Shader> m_shadow_shader;

        // skybox
        std::shared_ptr<Model> m_skybox_model;
        std::shared_ptr<Shader> m_skybox_shader;
        std::shared_ptr<TextureCubemaps> m_skybox_texture;

        // plane
        std::shared_ptr<Model> m_plane_model;
        // std::shared_ptr<Shader> m_plane_shader;
        std::shared_ptr<Texture2D> m_plane_texture;

        // bunny
        std::shared_ptr<Model> m_bunny_model;
        // std::shared_ptr<Shader> m_bunny_shader;
    };
} // namespace Yutrel