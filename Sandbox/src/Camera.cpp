#include "Camera.h"

Camera::Camera(uint32_t image_width, uint32_t image_height)
    : m_image_width(image_width),
      m_image_height(image_height)
{
    init();
}

void Camera::init()
{
    m_aspect_ratio = static_cast<float>(m_image_width) / static_cast<float>(m_image_height);
    m_center       = glm::ivec3(0, 0, 0);

    auto focal_length    = 1.0f;
    auto viewport_height = 2.0f;
    auto viewport_width  = m_aspect_ratio * viewport_height;

    auto viewport_u = glm::vec3(viewport_width, 0.0f, 0.0f);
    auto viewport_v = glm::vec3(0.0f, -viewport_height, 0.0f);

    m_pixel_delta_u = viewport_u / static_cast<float>(m_image_width);
    m_pixel_delta_v = viewport_v / static_cast<float>(m_image_height);

    auto viewport_upper_left = m_center - glm::vec3(0, 0, focal_length) - viewport_u / 2.0f - viewport_v / 2.0f;

    m_pixel00_location = viewport_upper_left + 0.5f * m_pixel_delta_u + 0.5f * m_pixel_delta_v;
}