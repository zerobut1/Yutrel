#include "camera_controller.h"

#include "glm/fwd.hpp"
#include "runtime/function/global/global_context.h"

#include <functional>
#include <memory>

namespace Yutrel
{
    std::shared_ptr<CameraController> CameraController::create(float aspectRatio, glm::vec3 positon, float yaw, float pitch)
    {
        return std::make_shared<CameraController>(aspectRatio, positon, yaw, pitch);
    }

    CameraController::CameraController(float aspectRatio, glm::vec3 positon, float yaw, float pitch)
        : m_camera(),
          m_aspect_ratio(aspectRatio),
          m_camera_position(positon),
          m_yaw(yaw),
          m_pitch(pitch)
    {
        updateCameraVectors();
        m_camera.setPosition(m_camera_position);
        m_camera.setDirection(m_camera_direction);
        m_camera.setUp(m_camera_up);
        m_camera.setProjection(m_aspect_ratio, m_zoom_level);

        g_runtime_global_context.m_window_system->registerOnWindowSizeFunc(std::bind(&CameraController::onWindowResized, this, std::placeholders::_1, std::placeholders::_2));
        g_runtime_global_context.m_window_system->registerOnScrollFunc(std::bind(&CameraController::onMouseScrolled, this, std::placeholders::_1, std::placeholders::_2));
        g_runtime_global_context.m_window_system->registerOnCursorPosFunc(std::bind(&CameraController::onCursorPos, this, std::placeholders::_1, std::placeholders::_2));
    }

    void CameraController::tick(double delta_time)
    {
        auto input     = g_runtime_global_context.m_input_system;
        float velocity = m_camera_speed * delta_time;

        if (input->IsKeyPressed(Key::D))
        {
            m_camera_position += m_camera_right * velocity;
        }
        else if (input->IsKeyPressed(Key::A))
        {
            m_camera_position -= m_camera_right * velocity;
        }
        if (input->IsKeyPressed(Key::W))
        {
            m_camera_position += m_camera_direction * velocity;
        }
        else if (input->IsKeyPressed(Key::S))
        {
            m_camera_position -= m_camera_direction * velocity;
        }
        if (input->IsKeyPressed(Key::Space))
        {
            m_camera_position += glm::vec3(0.0, 1.0, 0.0) * velocity;
        }
        else if (input->IsKeyPressed(Key::LeftShift))
        {
            m_camera_position -= glm::vec3(0.0, 1.0, 0.0) * velocity;
        }
        
        m_camera.setPosition(m_camera_position);
    }

    void CameraController::onWindowResized(int width, int height)
    {
        m_aspect_ratio = (float)width / (float)height;
        m_camera.setProjection(m_aspect_ratio, m_zoom_level);
    }

    void CameraController::onMouseScrolled(double xoffset, double yoffset)
    {
        m_zoom_level -= yoffset;
        m_zoom_level = std::max(m_zoom_level, 1.0f);
        m_zoom_level = std::min(m_zoom_level, 45.0f);
        m_camera.setProjection(m_aspect_ratio, m_zoom_level);
    }

    void CameraController::onCursorPos(double xpos, double ypos)
    {
        auto input = g_runtime_global_context.m_input_system;
        if (input->IsMouseButtonPressed(Mouse::ButtonRight))
        {
            // glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

            float xpos_cur = static_cast<float>(xpos);
            float ypos_cur = static_cast<float>(ypos);

            if (firstMouse)
            {
                lastX      = xpos_cur;
                lastY      = ypos_cur;
                firstMouse = false;
            }

            float xoffset = xpos_cur - lastX;
            float yoffset = lastY - ypos_cur;

            lastX = xpos_cur;
            lastY = ypos_cur;

            ProcessMouseMovement(xoffset, yoffset);
        }
        else
        {
            // glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            firstMouse = true;
        }
    }

    void CameraController::ProcessMouseMovement(float xoffset, float yoffset)
    {
        xoffset *= m_sensitivity;
        yoffset *= m_sensitivity;

        m_yaw += xoffset;
        m_pitch += yoffset;

        // make sure that when pitch is out of bounds, screen doesn't get flipped
        if (m_pitch > 89.0f)
            m_pitch = 89.0f;
        if (m_pitch < -89.0f)
            m_pitch = -89.0f;

        // update Front, Right and Up Vectors using the updated Euler angles
        updateCameraVectors();
    }

    void CameraController::updateCameraVectors()
    {
        glm::vec3 front;
        front.x = cos(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
        front.y = sin(glm::radians(m_pitch));
        front.z = sin(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));

        m_camera_direction = glm::normalize(front);
        m_camera_right     = glm::normalize(glm::cross(m_camera_direction, m_world_up));
        m_camera_up        = glm::normalize(glm::cross(m_camera_right, front));
        m_camera.setDirection(m_camera_direction);
        m_camera.setUp(m_camera_up);
    }
} // namespace Yutrel