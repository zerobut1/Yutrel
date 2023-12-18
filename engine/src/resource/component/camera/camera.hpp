#pragma once

// 暂时使用glm
#include <glm/glm.hpp>

namespace Yutrel
{
    /**
     *camera类主要维护view和projection两个矩阵
     *目前是一个透视投影摄像机
     */
    class Camera
    {
    public:
        Camera() = default;

        void setProjection(float aspectRatio, float fov);

        const glm::vec3& getPosition() const { return m_position; }
        void setPosition(const glm::vec3& position)
        {
            m_position = position;
            recalculateViewMatrix();
        }

        const glm::vec3& getDirection() const { return m_direction; }
        void setDirection(const glm::vec3& direction)
        {
            m_direction = direction;
            recalculateViewMatrix();
        }

        const glm::vec3& getUp() const { return m_up; }
        void setUp(const glm::vec3& up)
        {
            m_up = up;
            recalculateViewMatrix();
        }

        const glm::vec3& GetRight() const { return m_right; }
        void SetRight(const glm::vec3& right)
        {
            m_right = right;
            recalculateViewMatrix();
        }

        const glm::mat4& getProjectionMatrix() const { return m_projection_matrix; }
        const glm::mat4& getViewMatrix() const { return m_view_matrix; }

    private:
        void recalculateViewMatrix();

    private:
        glm::mat4 m_projection_matrix = glm::mat4(1.0f);
        glm::mat4 m_view_matrix       = glm::mat4(1.0f);

        glm::vec3 m_position  = {0.0f, 0.0f, 0.0f};
        glm::vec3 m_direction = {0.0f, 0.0f, -1.0f};
        glm::vec3 m_up        = {0.0f, 1.0f, 0.0f};
        glm::vec3 m_right     = {1.0f, 0.0f, 0.0f};
    };

} // namespace Yutrel
