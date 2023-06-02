/*
#include "camera.h"
#include "glm/ext/matrix_clip_space.hpp"

#include <glm/gtc/matrix_transform.hpp>

namespace Yutrel
{

    Camera::Camera(float left, float right, float bottom, float top)
        : m_projection_matrix(glm::ortho(left, right, bottom, top, -1.0f, 1.0f)), m_view_matrix(1.0f)
    {
    }

    void Camera::setProjection(float left, float right, float bottom, float top)
    {
        m_projection_matrix = glm::ortho(left, right, bottom, top, -1.0f, 1.0f);
        // m_projection_matrix = glm::perspective(T fovy, T aspect, T near, T far)
    }

    void Camera::recalculateViewMatrix()
    {
        glm::mat4 transform = glm::translate(glm::mat4(1.0f), m_position) *
                              glm::rotate(glm::mat4(1.0f), glm::radians(m_rotation), glm::vec3(0, 0, 1));

        m_view_matrix = glm::inverse(transform);
    }

} // namespace Yutrel
*/