#include "yutrel_pch.hpp"

#include "camera.hpp"

#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/transform.hpp>

namespace Yutrel
{
    void Camera::Updata(float delta_time)
    {
        glm::vec3 front;
        front.x     = sin(m_yaw) * cos(m_pitch);
        front.y     = sin(m_pitch);
        front.z     = -1.0f * cos(m_yaw) * cos(m_pitch);
        m_direction = glm::normalize(front);
        m_right     = glm::normalize(glm::cross(m_direction, glm::vec3{0.0f, 1.0f, 0.0f}));

        glm::vec3 velocity = m_world_velocity * delta_time * m_speed;
        m_world_velocity   = glm::vec3(0.0f);
        m_world_position += velocity;
    }

    glm::mat4 Camera::GetViewMatrix() const
    {
        glm::mat4 camera_translation = glm::translate(glm::mat4(1.f), m_world_position);
        glm::mat4 camera_rotation    = GetRotationMatrix();
        return glm::inverse(camera_translation * camera_rotation);
    }

    glm::mat4 Camera::GetRotationMatrix() const
    {
        glm::quat pitch_rotation = glm::angleAxis(m_pitch, glm::vec3{1.0f, 0.0f, 0.0f});
        glm::quat yaw_rotation   = glm::angleAxis(m_yaw, glm::vec3{0.0f, -1.0f, 0.0f});

        return glm::toMat4(yaw_rotation) * glm::toMat4(pitch_rotation);
    }

    void Camera::MoveFront()
    {
        m_world_velocity += m_direction;
    }

    void Camera::MoveBack()
    {
        m_world_velocity -= m_direction;
    }

    void Camera::MoveLeft()
    {
        m_world_velocity -= m_right;
    }

    void Camera::MoveRight()
    {
        m_world_velocity += m_right;
    }

    void Camera::MoveUp()
    {
        m_world_velocity += glm::vec3(0.0f, 1.0f, 0.0f);
    }

    void Camera::MoveDown()
    {
        m_world_velocity -= glm::vec3(0.0f, 1.0f, 0.0f);
    }

    void Camera::MoveDirection(float offset_x, float offset_y)
    {
        m_yaw += offset_x * m_mouse_sensitivity;
        m_pitch += offset_y * m_mouse_sensitivity;

        // 上下视角不超过90度
        if (m_pitch > 89.0f)
            m_pitch = 89.0f;
        if (m_pitch < -89.0f)
            m_pitch = -89.0f;
    }

} // namespace Yutrel