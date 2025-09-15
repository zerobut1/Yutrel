#include "Compute.h"

#include <DescriptorSet.h>
#include <Pipeline.h>
#include <Renderer.h>
#include <Swapchain.h>

#include "Shaders.h"

using namespace Yutrel;

void Compute::onAttach(Yutrel::Application* app)
{
    m_app      = app;
    m_renderer = app->getRenderer();

    m_camera = std::make_unique<Camera>(m_viewport_width, m_viewport_height);

    Yutrel::RenderTarget::CreateInfo rt_ci{};
    rt_ci.extent = vk::Extent2D{m_camera->getImageWidth(), m_camera->getImageHeight()};
    rt_ci.format = vk::Format::eR32G32B32A32Sfloat;
    rt_ci.layout = vk::ImageLayout::eGeneral;
    m_main_rt    = Yutrel::RenderTarget::create(m_renderer, rt_ci);

    initCameraBuffer();
    initDataBuffer();
    initDescriptors();
    initPipeline();
}

void Compute::onDetach()
{
}

void Compute::onRender(vk::CommandBuffer cmd_buffer)
{
    updatePushConstants();

    updateCameraBuffer();

    auto swapchain = m_app->getSwapchain();

    m_main_rt->transitionImageLayout(cmd_buffer, vk::ImageLayout::eGeneral);

    draw(cmd_buffer);

    m_main_rt->transitionImageLayout(cmd_buffer, vk::ImageLayout::eTransferSrcOptimal);

    m_renderer->transitionImageLayout(cmd_buffer,
                                      swapchain->getCurrentImage(),
                                      vk::ImageLayout::eUndefined,
                                      vk::ImageLayout::eTransferDstOptimal);

    m_renderer->copyImageToImage(cmd_buffer,
                                 m_main_rt->getImage(),
                                 swapchain->getCurrentImage(),
                                 m_main_rt->getExtent(),
                                 swapchain->getExtent());

    m_renderer->transitionImageLayout(cmd_buffer,
                                      swapchain->getCurrentImage(),
                                      vk::ImageLayout::eTransferDstOptimal,
                                      vk::ImageLayout::ePresentSrcKHR);
}

void Compute::onResize(uint32_t width, uint32_t height)
{
    // push constants
    m_viewport_width  = width;
    m_viewport_height = height;

    m_push_constants.viewport_width  = m_viewport_width;
    m_push_constants.viewport_height = m_viewport_height;

    // camera
    m_camera = std::make_unique<Camera>(m_viewport_width, m_viewport_height);

    m_camera_data.center           = m_camera->getCenter();
    m_camera_data.pixel00_location = m_camera->getPixel00Location();
    m_camera_data.pixel_delta_u    = m_camera->getPixelDeltaU();
    m_camera_data.pixel_delta_v    = m_camera->getPixelDeltaV();
}

void Compute::initCameraBuffer()
{
    m_camera_buffer = m_renderer->createBuffer(sizeof(CameraData), vk::BufferUsageFlagBits::eUniformBuffer, VMA_MEMORY_USAGE_CPU_TO_GPU, VMA_ALLOCATION_CREATE_MAPPED_BIT);
}

void Compute::initDataBuffer()
{
    // material
    m_material_data.emplace_back(Material{MT_Lambertian,glm::vec4(0.8f, 0.8f, 0.0f, 0.0f)});
    m_material_data.emplace_back(Material{MT_Lambertian,glm::vec4(0.1f, 0.2f, 0.5f, 0.0f)});
    m_material_data.emplace_back(Material{MT_Metal, glm::vec4(0.8f, 0.8f, 0.8f, 0.3f)});
    m_material_data.emplace_back(Material{MT_Metal, glm::vec4(0.8f, 0.6f, 0.2f, 1.0f)});

    m_material_buffer = m_renderer->createBuffer(sizeof(Material) * m_material_data.size(), vk::BufferUsageFlagBits::eStorageBuffer, VMA_MEMORY_USAGE_CPU_TO_GPU, VMA_ALLOCATION_CREATE_MAPPED_BIT);
    memcpy(m_material_buffer.info.pMappedData, m_material_data.data(), sizeof(Material) * m_material_data.size());

    // sphere
    m_spheres.emplace_back(Sphere{glm::vec3(0, -100.5f, -1.0f), 100.0f, 0});
    m_spheres.emplace_back(Sphere{glm::vec3(0.0f, 0.0f, -1.2f), 0.5f, 1});
    m_spheres.emplace_back(Sphere{glm::vec3(-1.0f, 0.0f, -1.0f), 0.5f, 2});
    m_spheres.emplace_back(Sphere{glm::vec3(1.0f, 0.0f, -1.0f), 0.5f, 3});
    m_sphere_buffer = m_renderer->createBuffer(sizeof(Sphere) * max_sphere_num, vk::BufferUsageFlagBits::eStorageBuffer, VMA_MEMORY_USAGE_CPU_TO_GPU, VMA_ALLOCATION_CREATE_MAPPED_BIT);
    memcpy(m_sphere_buffer.info.pMappedData, m_spheres.data(), sizeof(Sphere) * max_sphere_num);

    // push constants
    m_push_constants.sphere_count      = static_cast<uint32_t>(m_spheres.size());
    m_push_constants.max_depth         = 10;
    m_push_constants.samples_per_pixel = 10;
}

void Compute::initDescriptors()
{
    auto layout_ci =
        DescriptorSetLayoutCreateInfo()
            .AddBinding(0, vk::DescriptorType::eStorageImage)
            .AddBinding(1, vk::DescriptorType::eUniformBuffer)
            .AddBinding(2, vk::DescriptorType::eStorageBuffer) // material
            .AddBinding(3, vk::DescriptorType::eStorageBuffer) // sphere
            .SetShaderStage(vk::ShaderStageFlagBits::eCompute);

    m_descriptor_set_layout = m_renderer->createDescriptorSetLayout(layout_ci);
    m_descriptor_set        = m_renderer->allocateDescriptorSets(m_descriptor_set_layout);

    DescriptorWriter writer;
    writer.WriteImage(0, m_main_rt->getDescriptorImageInfo(), vk::DescriptorType::eStorageImage)
        .WriteBuffer(1, m_camera_buffer.buffer, m_camera_buffer.info.size, 0, vk::DescriptorType::eUniformBuffer)
        .WriteBuffer(2, m_material_buffer.buffer, m_material_buffer.info.size, 0, vk::DescriptorType::eStorageBuffer)
        .WriteBuffer(3, m_sphere_buffer.buffer, m_sphere_buffer.info.size, 0, vk::DescriptorType::eStorageBuffer);

    m_renderer->updateDescriptorSets(writer, m_descriptor_set);
}

void Compute::initPipeline()
{
    auto push_constant_range =
        vk::PushConstantRange()
            .setOffset(0)
            .setSize(sizeof(PushConstants))
            .setStageFlags(vk::ShaderStageFlagBits::eCompute);

    auto compute_layout_ci =
        vk::PipelineLayoutCreateInfo()
            .setPushConstantRanges(push_constant_range)
            .setSetLayouts(m_descriptor_set_layout);

    m_pipeline_layout = m_renderer->createPipelineLayout(compute_layout_ci);

    vk::ShaderModule compute_shader = m_renderer->createShaderModule(MAIN_COMP);

    auto compute_shader_stage =
        vk::PipelineShaderStageCreateInfo()
            .setStage(vk::ShaderStageFlagBits::eCompute)
            .setModule(compute_shader)
            .setPName("main");

    auto render_pipeline_ci =
        vk::ComputePipelineCreateInfo()
            .setLayout(m_pipeline_layout)
            .setStage(compute_shader_stage);

    m_pipeline = m_renderer->createComputePipeline(render_pipeline_ci);

    //--------清除---------
    m_renderer->destroyShaderModule(compute_shader);
}

void Compute::draw(vk::CommandBuffer cmd_buffer)
{
    cmd_buffer.bindPipeline(vk::PipelineBindPoint::eCompute, m_pipeline);

    cmd_buffer.bindDescriptorSets(vk::PipelineBindPoint::eCompute, m_pipeline_layout, 0, 1, &m_descriptor_set, 0, nullptr);

    cmd_buffer.pushConstants(m_pipeline_layout, vk::ShaderStageFlagBits::eCompute, 0, sizeof(m_push_constants), &m_push_constants);

    auto extent = m_main_rt->getExtent();

    cmd_buffer.dispatch(static_cast<uint32_t>(std::ceil(extent.width / 16.0)), static_cast<uint32_t>(std::ceil(extent.height / 16.0)), 1);
}

void Compute::updatePushConstants()
{
    m_push_constants.time = static_cast<float>(m_app->getTime());
}

void Compute::updateCameraBuffer()
{
    memcpy(m_camera_buffer.info.pMappedData, &m_camera_data, sizeof(CameraData));
}