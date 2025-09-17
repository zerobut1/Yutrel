#include "Compute.h"

#include <Context.h>
#include <DescriptorSet.h>
#include <Pipeline.h>
#include <Renderer.h>
#include <Swapchain.h>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_vulkan.h>

#include <random>

#include "Shaders.h"

using namespace Yutrel;

inline double randomDouble()
{
    static std::uniform_real_distribution<double> distribution(0.0, 1.0);
    static std::mt19937 generator;
    return distribution(generator);
}

void Compute::onAttach(Yutrel::Application* app)
{
    m_app      = app;
    m_renderer = app->getRenderer();

    initCameraBuffer();

    Yutrel::RenderTarget::CreateInfo rt_ci{};
    rt_ci.extent = vk::Extent2D{m_camera->getImageWidth(), m_camera->getImageHeight()};
    rt_ci.format = vk::Format::eR32G32B32A32Sfloat;
    rt_ci.layout = vk::ImageLayout::eGeneral;
    m_main_rt    = Yutrel::RenderTarget::create(m_renderer, rt_ci);

    initDataBuffer();
    initDescriptors();
    initPipeline();

    initImGui();
}

void Compute::onDetach()
{
}

void Compute::onRender(vk::CommandBuffer cmd_buffer)
{
    updatePushConstants();

    updateCameraBuffer();

    updateImGui();

    auto swapchain = m_app->getSwapchain();

    // ray tracing
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

    // Imgui
    m_renderer->transitionImageLayout(cmd_buffer,
                                      swapchain->getCurrentImage(),
                                      vk::ImageLayout::eTransferDstOptimal,
                                      vk::ImageLayout::eColorAttachmentOptimal);

    drawImGui(cmd_buffer, swapchain);

    m_renderer->transitionImageLayout(cmd_buffer,
                                      swapchain->getCurrentImage(),
                                      vk::ImageLayout::eColorAttachmentOptimal,
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
    m_camera->resize(m_viewport_width, m_viewport_height);
}

void Compute::initCameraBuffer()
{
    m_camera                    = std::make_unique<Camera>(m_viewport_width, m_viewport_height);
    m_camera->samples_per_pixel = 50;
    m_camera->max_depth         = 10;
    m_camera->vfov              = 20.0f;
    m_camera->look_from         = glm::vec3(13.0f, 2.0f, 3.0f);
    m_camera->look_at           = glm::vec3(0.0f, 0.0f, 0.0f);
    m_camera->vup               = glm::vec3(0.0f, 1.0f, 0.0f);
    m_camera->defocus_angle     = 0.6f;
    m_camera->focus_distance    = 10.0f;
    m_camera->init();

    m_camera_data.center           = m_camera->getCenter();
    m_camera_data.pixel00_location = m_camera->getPixel00Location();
    m_camera_data.pixel_delta_u    = m_camera->getPixelDeltaU();
    m_camera_data.pixel_delta_v    = m_camera->getPixelDeltaV();

    m_camera_data.samples_per_pixel = m_camera->samples_per_pixel;
    m_camera_data.max_depth         = m_camera->max_depth;

    m_camera_data.defocus_angle  = m_camera->defocus_angle;
    m_camera_data.defocus_disk_u = m_camera->getDefocusDiskU();
    m_camera_data.defocus_disk_v = m_camera->getDefocusDiskV();

    m_camera_buffer = m_renderer->createBuffer(sizeof(CameraData), vk::BufferUsageFlagBits::eUniformBuffer, VMA_MEMORY_USAGE_CPU_TO_GPU, VMA_ALLOCATION_CREATE_MAPPED_BIT);
}

void Compute::initDataBuffer()
{
    // ground
    m_material_data.emplace_back(Material{MT_Lambertian, glm::vec4(0.4f, 0.8f, 1.0f, 0.0f)}); // ground
    m_spheres.emplace_back(Sphere{glm::vec3(0, -1000.0f, 0.0f), 1000.0f, 0});

    // small spheres
    for (int a = -11; a < 11; a++)
    {
        for (int b = -11; b < 11; b++)
        {
            auto choose_mat = randomDouble();
            glm::vec3 center(static_cast<float>(a) + 0.9f * static_cast<float>(randomDouble()), 0.2f, static_cast<float>(b) + 0.9f * static_cast<float>(randomDouble()));

            if (glm::length(center - glm::vec3(4.0f, 0.2f, 0.0f)) > 0.9f)
            {
                if (choose_mat < 0.8) // diffuse
                {
                    glm::vec3 albedo = glm::vec3(static_cast<float>(randomDouble()) * static_cast<float>(randomDouble()),
                                                 static_cast<float>(randomDouble()) * static_cast<float>(randomDouble()),
                                                 static_cast<float>(randomDouble()) * static_cast<float>(randomDouble()));
                    m_material_data.emplace_back(Material{MT_Lambertian, glm::vec4(albedo, 0.0f)});
                    m_spheres.emplace_back(Sphere{center, 0.2f, static_cast<uint32_t>(m_material_data.size() - 1)});
                }
                else if (choose_mat < 0.95) // metal
                {
                    glm::vec3 albedo = glm::vec3(0.5f * (1.0f + static_cast<float>(randomDouble())),
                                                 0.5f * (1.0f + static_cast<float>(randomDouble())),
                                                 0.5f * (1.0f + static_cast<float>(randomDouble())));
                    float fuzz       = 0.5f * static_cast<float>(randomDouble());
                    m_material_data.emplace_back(Material{MT_Metal, glm::vec4(albedo, fuzz)});
                    m_spheres.emplace_back(Sphere{center, 0.2f, static_cast<uint32_t>(m_material_data.size() - 1)});
                }
                else // glass
                {
                    m_material_data.emplace_back(Material{MT_Dielectric, glm::vec4(1.5f, 0.0f, 0.0f, 0.0f)});
                    float radius = static_cast<float>(randomDouble());
                    center.y     = radius;
                    m_spheres.emplace_back(Sphere{center, radius, static_cast<uint32_t>(m_material_data.size() - 1)});
                }
            }
        }
    }
    m_push_constants.sphere_count = static_cast<uint32_t>(m_spheres.size());

    m_material_buffer = m_renderer->createBuffer(sizeof(Material) * m_material_data.size(), vk::BufferUsageFlagBits::eStorageBuffer, VMA_MEMORY_USAGE_CPU_TO_GPU, VMA_ALLOCATION_CREATE_MAPPED_BIT);
    memcpy(m_material_buffer.info.pMappedData, m_material_data.data(), sizeof(Material) * m_material_data.size());
    m_sphere_buffer = m_renderer->createBuffer(sizeof(Sphere) * max_sphere_num, vk::BufferUsageFlagBits::eStorageBuffer, VMA_MEMORY_USAGE_CPU_TO_GPU, VMA_ALLOCATION_CREATE_MAPPED_BIT);
    memcpy(m_sphere_buffer.info.pMappedData, m_spheres.data(), sizeof(Sphere) * max_sphere_num);
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

void Compute::initImGui()
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;     // Enable Docking
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

    ImGui::StyleColorsDark();

    ImGuiStyle& style = ImGui::GetStyle();
    style.ScaleAllSizes(2.0f); // Bake a fixed style scale. (until we have a solution for dynamic style scaling, changing this requires resetting Style + calling this again)
    style.FontScaleDpi = 2.0f;

    ImGui_ImplGlfw_InitForVulkan(m_app->getWindow()->getWindow(), true);
    ImGui_ImplVulkan_InitInfo init_info{
        .Instance            = static_cast<VkInstance>(m_renderer->getContext()->getInstance()),
        .PhysicalDevice      = static_cast<VkPhysicalDevice>(m_renderer->getContext()->getGPU()),
        .Device              = static_cast<VkDevice>(m_renderer->getContext()->getDevice()),
        .QueueFamily         = m_renderer->getContext()->getGraphicsQueueIndex(),
        .Queue               = static_cast<VkQueue>(m_renderer->getContext()->getGraphicsQueue()),
        .DescriptorPool      = static_cast<VkDescriptorPool>(m_renderer->getDescriptorPool()),
        .MinImageCount       = 2,
        .ImageCount          = 2,
        .MSAASamples         = VK_SAMPLE_COUNT_1_BIT,
        .UseDynamicRendering = true,
    };
    ImGui_ImplVulkan_Init(&init_info);
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
    m_camera_data.samples_per_pixel = m_camera->samples_per_pixel;
    m_camera_data.max_depth         = m_camera->max_depth;

    memcpy(m_camera_buffer.info.pMappedData, &m_camera_data, sizeof(CameraData));
}

void Compute::updateImGui()
{
    ImGui_ImplVulkan_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    {
        ImGui::Begin("Config");
        ImGui::SliderInt("SamplesPerPixel", reinterpret_cast<int*>(&m_camera->samples_per_pixel), 0, 100);
        ImGui::SliderInt("MaxDepth", reinterpret_cast<int*>(&m_camera->max_depth), 0, 10);
        ImGui::End();
    }

    ImGui::Render();
}

void Compute::drawImGui(vk::CommandBuffer cmd_buffer, std::shared_ptr<Yutrel::Swapchain> swapchain)
{

    auto color_attachment =
        vk::RenderingAttachmentInfo()
            .setImageView(swapchain->getCurrentImageView())
            .setImageLayout(vk::ImageLayout::eGeneral)
            .setLoadOp(vk::AttachmentLoadOp::eLoad)
            .setStoreOp(vk::AttachmentStoreOp::eStore);

    auto render_info =
        vk::RenderingInfo()
            .setRenderArea(vk::Rect2D({0, 0}, swapchain->getExtent()))
            .setLayerCount(1)
            .setColorAttachments(color_attachment);

    cmd_buffer.beginRendering(render_info);

    ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), cmd_buffer);

    cmd_buffer.endRendering();

    if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();
    }
}