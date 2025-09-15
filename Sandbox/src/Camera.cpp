#include "Camera.h"

Camera::Camera(uint32_t image_width, uint32_t image_height)
    : m_image_width(image_width),
      m_image_height(image_height)
{
    init();
}

void Camera::resize(uint32_t image_width, uint32_t image_height)
{
    m_image_width  = image_width;
    m_image_height = image_height;
    init();
}

void Camera::init()
{
    m_aspect_ratio = static_cast<float>(m_image_width) / static_cast<float>(m_image_height);
    m_center       = look_from;

    auto theta           = glm::radians(vfov);
    auto h               = std::tan(theta / 2.0f);
    auto viewport_height = 2.0f * h * focus_distance;
    auto viewport_width  = m_aspect_ratio * viewport_height;

    glm::vec3 w = glm::normalize(look_from - look_at);
    glm::vec3 u = glm::normalize(glm::cross(vup, w));
    glm::vec3 v = glm::cross(w, u);

    auto viewport_u = viewport_width * u;
    auto viewport_v = viewport_height * -v;

    m_pixel_delta_u = viewport_u / static_cast<float>(m_image_width);
    m_pixel_delta_v = viewport_v / static_cast<float>(m_image_height);

    auto viewport_upper_left = m_center - (focus_distance * w) - viewport_u / 2.0f - viewport_v / 2.0f;

    m_pixel00_location = viewport_upper_left + 0.5f * m_pixel_delta_u + 0.5f * m_pixel_delta_v;

    auto defocus_radius = focus_distance * std::tan(glm::radians(defocus_angle / 2.0f));
    m_defocus_disk_u    = defocus_radius * u;
    m_defocus_disk_v    = defocus_radius * v;
}