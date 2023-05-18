/*
#include "camera_controller.h"

#include "runtime/function/global/global_context.h"

#include <memory>

namespace Yutrel
{
    std::shared_ptr<CameraController> CameraController::create(float aspectRatio, bool rotation)
    {
        return std::make_shared<CameraController>(aspectRatio, rotation);
    }

    CameraController::CameraController(float aspectRatio, bool rotation)
        : m_aspect_ratio(aspectRatio),
          m_camera(-m_aspect_ratio * m_zoom_level, m_aspect_ratio * m_zoom_level, -m_zoom_level, m_zoom_level),
          m_enable_rotation(rotation)
    {
        g_runtime_global_context.m_window_system->registerOnWindowSizeFunc(std::bind(&CameraController::onWindowResized, this, std::placeholders::_1, std::placeholders::_2));
        g_runtime_global_context.m_window_system->registerOnScrollFunc(std::bind(&CameraController::onMouseScrolled, this, std::placeholders::_1, std::placeholders::_2));
    }

    void CameraController::tick(double delta_time)
    {
        auto input = g_runtime_global_context.m_input_system;

        if (input->IsKeyPressed(Key::A))
        {
            m_camera_position.x -= cos(glm::radians(m_camera_rotation)) * m_camera_translation_speed * delta_time;
            m_camera_position.y -= sin(glm::radians(m_camera_rotation)) * m_camera_translation_speed * delta_time;
        }
        else if (input->IsKeyPressed(Key::D))
        {
            m_camera_position.x += cos(glm::radians(m_camera_rotation)) * m_camera_translation_speed * delta_time;
            m_camera_position.y += sin(glm::radians(m_camera_rotation)) * m_camera_translation_speed * delta_time;
        }

        if (input->IsKeyPressed(Key::W))
        {
            m_camera_position.x += -sin(glm::radians(m_camera_rotation)) * m_camera_translation_speed * delta_time;
            m_camera_position.y += cos(glm::radians(m_camera_rotation)) * m_camera_translation_speed * delta_time;
        }
        else if (input->IsKeyPressed(Key::S))
        {
            m_camera_position.x -= -sin(glm::radians(m_camera_rotation)) * m_camera_translation_speed * delta_time;
            m_camera_position.y -= cos(glm::radians(m_camera_rotation)) * m_camera_translation_speed * delta_time;
        }

        if (m_enable_rotation)
        {
            if (input->IsKeyPressed(Key::Q))
                m_camera_rotation += m_camera_rotation_speed * delta_time;
            if (input->IsKeyPressed(Key::E))
                m_camera_rotation -= m_camera_rotation_speed * delta_time;

            if (m_camera_rotation > 180.0f)
                m_camera_rotation -= 360.0f;
            else if (m_camera_rotation <= -180.0f)
                m_camera_rotation += 360.0f;

            m_camera.setRotation(m_camera_rotation);
        }

        m_camera.setPosition(m_camera_position);

        m_camera_translation_speed = m_zoom_level;
    }

    void CameraController::onWindowResized(int width, int height)
    {
        m_aspect_ratio = (float)width / (float)height;
        m_camera.setProjection(-m_aspect_ratio * m_zoom_level, m_aspect_ratio * m_zoom_level, -m_zoom_level, m_zoom_level);
    }

    void CameraController::onMouseScrolled(double xoffset, double yoffset)
    {
        m_zoom_level -= yoffset * 0.25f;
        m_zoom_level = std::max(m_zoom_level, 0.25f);
        m_camera.setProjection(-m_aspect_ratio * m_zoom_level, m_aspect_ratio * m_zoom_level, -m_zoom_level, m_zoom_level);
    }

} // namespace Yutrel
*/