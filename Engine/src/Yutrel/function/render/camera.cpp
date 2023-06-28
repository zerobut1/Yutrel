#include "yutrel_pch.hpp"

#include "camera.h"

// 暂时使用glm
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/trigonometric.hpp>

namespace Yutrel
{
    void Camera::setProjection(float aspectRatio, float fov)
    {
        m_projection_matrix = glm::perspective(glm::radians(fov), aspectRatio, 0.1f, 100.0f);
    }

    void Camera::recalculateViewMatrix()
    {
        m_view_matrix = glm::lookAt(m_position, m_position + m_direction, m_up);
    }

} // namespace Yutrel