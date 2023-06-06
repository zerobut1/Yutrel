#pragma once

#include "Yutrel/function/render/camera.h"
#include <memory>

namespace Yutrel
{
    /**
     *cameracontroller负责camera与其它系统的交互以及控制
     */
    class CameraController
    {
    public:
        static std::shared_ptr<CameraController> Create(float aspectRatio, glm::vec3 positon, float yaw = -90.0f, float pitch = 0.0f);

        CameraController(float aspectRatio, glm::vec3 positon, float yaw, float pitch);

        void tick(double delta_time, float aspectRatio);

        Camera &getCamera() { return m_camera; }
        const Camera &getCamera() const { return m_camera; }

        float getZoomLevel() const { return m_zoom_level; }
        void setZoomLevel(float level) { m_zoom_level = level; }

        void onMouseScrolled(double xoffset, double yoffset);
        void onCursorPos(double xpos, double ypos);

    private:
        void ProcessMouseMovement(float xoffset, float yoffset);
        void updateCameraVectors();

    private:
        Camera m_camera;

        glm::vec3 m_camera_position;
        glm::vec3 m_camera_direction;
        glm::vec3 m_camera_up;
        glm::vec3 m_camera_right;
        float m_aspect_ratio = 1.0f;
        float m_yaw          = -90.0f;
        float m_pitch        = 0.0f;
        float m_zoom_level   = 45.0f;
        float lastX          = 0.0f;
        float lastY          = 0.0f;
        bool firstMouse      = true;

        glm::vec3 m_world_up = {0.0f, 1.0f, 0.0f};
        float m_sensitivity_mouse = 0.1f;
        float m_sensitivity_scroll = 2.0f;
        float m_camera_speed = 5.0f;
    };

} // namespace Yutrel