/*
#pragma once

// 暂时使用glm
#include <glm/glm.hpp>

namespace Yutrel
{
    class Camera
    {
    public:
        Camera(float left, float right, float bottom, float top);

        void setProjection(float left, float right, float bottom, float top);

        const glm::vec3 &getPosition() const { return m_position; }
        void setPosition(const glm::vec3 &position)
        {
            m_position = position;
            recalculateViewMatrix();
        }

        float getRotation() const { return m_rotation; }
        void setRotation(float rotation)
        {
            m_rotation = rotation;
            recalculateViewMatrix();
        }

        const glm::mat4 &getProjectionMatrix() const { return m_projection_matrix; }
        const glm::mat4 &getViewMatrix() const { return m_view_matrix; }

    private:
        void recalculateViewMatrix();

    private:
        glm::mat4 m_projection_matrix;
        glm::mat4 m_view_matrix;

        glm::vec3 m_position = {0.0f, 0.0f, 0.0f};
        float m_rotation     = 0.0f;
    };

} // namespace Yutrel
*/