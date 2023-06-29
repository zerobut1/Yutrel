#pragma once

#include "Yutrel/function/render/camera.hpp"

#include <memory>

namespace Yutrel
{
    /**
     *cameracontroller负责camera与其它系统的交互以及控制
     */
    class CameraController
    {
    public:
        static CameraController* Create(float aspectRatio, glm::vec3 positon, float yaw = -90.0f, float pitch = 0.0f);

        CameraController(float aspectRatio, glm::vec3 positon, float yaw, float pitch);

        void Tick(double delta_time);

        void Resize(float aspectRatio);

        Camera& GetCamera()
        {
            return m_camera;
        }
        const Camera& GetCamera() const { return m_camera; }

        float GetZoomLevel() const { return m_zoom_level; }
        void SetZoomLevel(float level) { m_zoom_level = level; }

        void OnMouseScrolled(double xoffset, double yoffset);
        void OnCursorPos(double xpos, double ypos);

    private:
        void ProcessMouseMovement(float xoffset, float yoffset);
        void UpdateCameraVectors();

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

        glm::vec3 m_world_up       = {0.0f, 1.0f, 0.0f};
        float m_sensitivity_mouse  = 0.1f;
        float m_sensitivity_scroll = 2.0f;
        float m_camera_speed       = 5.0f;
    };

} // namespace Yutrel