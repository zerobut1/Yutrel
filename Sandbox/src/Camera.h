#pragma once

#include <glm/glm.hpp>

class Camera
{
public:
    Camera(uint32_t image_width, uint32_t image_height);

    void init();

    void resize(uint32_t image_width, uint32_t image_height);

    uint32_t getImageWidth() const { return m_image_width; }
    uint32_t getImageHeight() const { return m_image_height; }
    glm::vec3 getCenter() const { return m_center; }
    glm::vec3 getPixel00Location() const { return m_pixel00_location; }
    glm::vec3 getPixelDeltaU() const { return m_pixel_delta_u; }
    glm::vec3 getPixelDeltaV() const { return m_pixel_delta_v; }
    glm::vec3 getDefocusDiskU() const { return m_defocus_disk_u; }
    glm::vec3 getDefocusDiskV() const { return m_defocus_disk_v; }

public:
    uint32_t samples_per_pixel{10};
    uint32_t max_depth{10};
    float vfov{90.0f};
    glm::vec3 look_from{0.0f, 0.0f, 0.0f};
    glm::vec3 look_at{0.0f, 0.0f, -1.0f};
    glm::vec3 vup{0.0f, 1.0f, 0.0f};

    float defocus_angle{0.0f};
    float focus_distance{10.0f};

private:
    uint32_t m_image_width{100};
    uint32_t m_image_height{100};
    float m_aspect_ratio{1.0f};

    glm::vec3 m_center;
    glm::vec3 m_pixel00_location;
    glm::vec3 m_pixel_delta_u;
    glm::vec3 m_pixel_delta_v;
    glm::vec3 m_defocus_disk_u;
    glm::vec3 m_defocus_disk_v;
};