#include "yutrel_pch.hpp"

#include "camera_controller.hpp"

#include "Yutrel/core/application/application.hpp"
#include "Yutrel/function/input/input.hpp"
#include "Yutrel/function/window/window.hpp"

#include "glm/fwd.hpp"

namespace Yutrel
{
    CameraController* CameraController::Create(float aspectRatio, glm::vec3 positon, float yaw, float pitch)
    {
        return new CameraController(aspectRatio, positon, yaw, pitch);
    }

    /**
     *根据相机的初始位置和俯仰角初始化
     */
    CameraController::CameraController(float aspectRatio, glm::vec3 positon, float yaw, float pitch)
        : m_camera(),
          m_aspect_ratio(aspectRatio),
          m_camera_position(positon),
          m_yaw(yaw),
          m_pitch(pitch)
    {
        UpdateCameraVectors();
        m_camera.setPosition(m_camera_position);
        m_camera.setDirection(m_camera_direction);
        m_camera.setUp(m_camera_up);
        m_camera.setProjection(m_aspect_ratio, m_zoom_level);

        // 滚轮缩放视角和右键按住拖动视角
        Window* window = *Application::Get().GetWorld().GetResource<Window*>();
        window->registerOnScrollFunc(std::bind(&CameraController::OnMouseScrolled, this, std::placeholders::_1, std::placeholders::_2));
        window->registerOnCursorPosFunc(std::bind(&CameraController::OnCursorPos, this, std::placeholders::_1, std::placeholders::_2));
    }

    /**
     * 目前的tick主要是用键盘控制摄像机位置
     */
    void CameraController::Tick(double delta_time)
    {
        auto input     = *Application::Get().GetWorld().GetResource<Input*>();
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

    void CameraController::Resize(float aspectRatio)
    {
        m_aspect_ratio = aspectRatio;
        m_camera.setProjection(m_aspect_ratio, m_zoom_level);
    }

    void CameraController::OnMouseScrolled(double xoffset, double yoffset)
    {
        m_zoom_level -= yoffset * m_sensitivity_scroll;
        // 缩放在1到45范围之内;
        m_zoom_level = std::max(m_zoom_level, 1.0f);
        m_zoom_level = std::min(m_zoom_level, 45.0f);
        m_camera.setProjection(m_aspect_ratio, m_zoom_level);
    }

    /**
     *目前的实现是将视角移动写到event中
     *应该会有更科学的写法
     */
    void CameraController::OnCursorPos(double xpos, double ypos)
    {
        auto input = *Application::Get().GetWorld().GetResource<Input*>();

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
        xoffset *= m_sensitivity_mouse;
        yoffset *= m_sensitivity_mouse;

        m_yaw += xoffset;
        m_pitch += yoffset;

        // 上下视角不超过90度
        if (m_pitch > 89.0f)
            m_pitch = 89.0f;
        if (m_pitch < -89.0f)
            m_pitch = -89.0f;

        UpdateCameraVectors();
    }

    void CameraController::UpdateCameraVectors()
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