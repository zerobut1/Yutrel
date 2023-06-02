/*
#pragma once

#include "Yutrel/function/render/camera.h"
#include <memory>

namespace Yutrel
{

    class CameraController
    {
    public:
        CameraController(float aspectRatio, bool rotation);

        void tick(double delta_time);

        Camera &getCamera() { return m_camera; }
        const Camera &getCamera() const { return m_camera; }

        float getZoomLevel() const { return m_zoom_level; }
        void setZoomLevel(float level) { m_zoom_level = level; }

        void onWindowResized(int width, int height);
        void onMouseScrolled(double xoffset, double yoffset);

        static std::shared_ptr<CameraController> create(float aspectRatio, bool rotation = false);

    private:
        float m_aspect_ratio;
        float m_zoom_level = 1.0f;
        Camera m_camera;

        bool m_enable_rotation;

        glm::vec3 m_camera_position      = {0.0f, 0.0f, 0.0f};
        float m_camera_rotation          = 0.0f; // In degrees, in the anti-clockwise direction
        float m_camera_translation_speed = 5.0f;
        float m_camera_rotation_speed    = 180.0f;
    };

} // namespace Yutrel
*/