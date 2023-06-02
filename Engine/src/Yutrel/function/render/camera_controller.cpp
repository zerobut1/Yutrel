#include "yutrel_pch.h"

#include "camera_controller.h"

#include "Yutrel/function/global/global_context.h"

#include "glm/fwd.hpp"

namespace Yutrel
{
    std::shared_ptr<CameraController> CameraController::create(float aspectRatio, glm::vec3 positon, float yaw, float pitch)
    {
        return std::make_shared<CameraController>(aspectRatio, positon, yaw, pitch);
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
        updateCameraVectors();
        m_camera.setPosition(m_camera_position);
        m_camera.setDirection(m_camera_direction);
        m_camera.setUp(m_camera_up);
        m_camera.setProjection(m_aspect_ratio, m_zoom_level);

        // 滚轮缩放视角和右键按住拖动视角
        g_runtime_global_context.m_window_system->registerOnScrollFunc(std::bind(&CameraController::onMouseScrolled, this, std::placeholders::_1, std::placeholders::_2));
        g_runtime_global_context.m_window_system->registerOnCursorPosFunc(std::bind(&CameraController::onCursorPos, this, std::placeholders::_1, std::placeholders::_2));
    }

    /**
     * 目前的tick主要是用键盘控制摄像机位置
     */
    void CameraController::tick(double delta_time, float aspectRatio)
    {
        auto input     = g_runtime_global_context.m_input_system;
        float velocity = m_camera_speed * delta_time;
        m_aspect_ratio = aspectRatio;
        m_camera.setProjection(m_aspect_ratio, m_zoom_level);

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

    void CameraController::onMouseScrolled(double xoffset, double yoffset)
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
        xoffset *= m_sensitivity_mouse;
        yoffset *= m_sensitivity_mouse;

        m_yaw += xoffset;
        m_pitch += yoffset;

        // 上下视角不超过90度
        if (m_pitch > 89.0f)
            m_pitch = 89.0f;
        if (m_pitch < -89.0f)
            m_pitch = -89.0f;

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