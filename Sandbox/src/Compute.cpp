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

    Yutrel::RenderTarget::CreateInfo rt_ci{};
    rt_ci.extent = vk::Extent2D{1920, 1080};
    rt_ci.format = vk::Format::eR32G32B32A32Sfloat;
    rt_ci.layout = vk::ImageLayout::eGeneral;
    m_main_rt    = Yutrel::RenderTarget::create(m_renderer, rt_ci);

    initDescriptors();
    initPipeline();
}

void Compute::onDetach()
{
}

void Compute::onRender(vk::CommandBuffer cmd_buffer)
{
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
    m_viewport_width  = width;
    m_viewport_height = height;

    m_push_constants.viewport_width  = m_viewport_width;
    m_push_constants.viewport_height = m_viewport_height;
}

void Compute::initDescriptors()
{
    auto layout_ci =
        DescriptorSetLayoutCreateInfo()
            .AddBinding(0, vk::DescriptorType::eStorageImage)
            .SetShaderStage(vk::ShaderStageFlagBits::eCompute);

    m_descriptor_set_layout = m_renderer->createDescriptorSetLayout(layout_ci);
    m_descriptor_set        = m_renderer->allocateDescriptorSets(m_descriptor_set_layout);

    auto writer =
        DescriptorWriter()
            .WriteImage(0, m_main_rt->getDescriptorImageInfo(), vk::DescriptorType::eStorageImage);
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

    vk::ShaderModule compute_shader = m_renderer->createShaderModule(TEST_COMP);

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

    cmd_buffer.dispatch(std::ceil(extent.width / 16.0), std::ceil(extent.height / 16.0), 1);
}