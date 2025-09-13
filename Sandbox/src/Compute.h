#pragma once

#include <Application.h>
#include <RenderTarget.h>

#include <glm/glm.hpp>

#include "Camera.h"

class Compute : public Yutrel::ComponentBase
{
public:
    void onAttach(Yutrel::Application* app) override;

    void onDetach() override;

    void onRender(vk::CommandBuffer cmd_buffer) override;

    void onResize(uint32_t width, uint32_t height) override;

private:
    void initCameraBuffer();

    void initDataBuffer();

    void initDescriptors();

    void initPipeline();

    void draw(vk::CommandBuffer cmd_buffer);

    void updatePushConstants();

    void updateCameraBuffer();

private:
    Yutrel::Application* m_app;
    std::shared_ptr<Yutrel::Renderer> m_renderer;

    std::unique_ptr<Yutrel::RenderTarget> m_main_rt;

    uint32_t m_viewport_width{1920};
    uint32_t m_viewport_height{1080};

    vk::DescriptorSet m_descriptor_set;
    vk::DescriptorSetLayout m_descriptor_set_layout;

    vk::PipelineLayout m_pipeline_layout;
    vk::Pipeline m_pipeline;

    struct PushConstants
    {
        uint32_t viewport_width;
        uint32_t viewport_height;
        uint32_t sphere_count;
        uint32_t samples_per_pixel;
        uint32_t max_depth;
        float time;
    } m_push_constants;

    // Camera data
    std::unique_ptr<Camera> m_camera;

    struct CameraData
    {
        alignas(16) glm::vec3 center;
        alignas(16) glm::vec3 pixel00_location;
        alignas(16) glm::vec3 pixel_delta_u;
        alignas(16) glm::vec3 pixel_delta_v;
    } m_camera_data;

    Yutrel::Buffer m_camera_buffer;

    // Sphere data
    struct Sphere
    {
        glm::vec3 center;
        float radius;
    };
    std::vector<Sphere> m_spheres;

    Yutrel::Buffer m_sphere_buffer;

    constexpr static uint32_t max_sphere_num = 1000;
};