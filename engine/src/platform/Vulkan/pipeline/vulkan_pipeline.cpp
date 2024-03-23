#include "yutrel_pch.hpp"

#include "vulkan_pipeline.hpp"

#include "function/render/renderer.hpp"
#include "platform/Vulkan/pipeline/passes/base_pass.hpp"
#include "platform/Vulkan/pipeline/passes/directional_light_pass.hpp"
#include "platform/Vulkan/pipeline/passes/imgui_pass.hpp"
#include "platform/Vulkan/pipeline/passes/lighting_pass.hpp"
#include "platform/Vulkan/pipeline/passes/main_pass.hpp"
#include "platform/Vulkan/pipeline/passes/render_pass.hpp"
#include "platform/Vulkan/rhi/vulkan_rhi.hpp"

#include <memory>

namespace Yutrel
{
    void VulkanPipeline::Init(RenderPipelineInitInfo info)
    {
        m_rhi           = info.rhi;
        m_asset_manager = info.asset_manager;
        m_render_scene  = info.render_scene;

        m_directional_light_pass = CreateRef<DirectionalLightPass>();
        m_main_pass              = CreateRef<MainPass>();
        m_imgui_pass             = CreateRef<ImguiPass>();
        m_base_pass              = CreateRef<BasePass>();
        m_lighting_pass          = CreateRef<LightingPass>();
        // m_debug_draw_pass        = CreateRef<DebugDrawPass>();

        DirectionalLightPassInitInfo directional_light_init_info{};
        directional_light_init_info.rhi           = m_rhi;
        directional_light_init_info.asset_manager = m_asset_manager;
        directional_light_init_info.render_scene  = m_render_scene;
        m_directional_light_pass->Init(&directional_light_init_info);

        MainPassInitInfo main_init_info{};
        main_init_info.rhi                         = m_rhi;
        main_init_info.asset_manager               = m_asset_manager;
        main_init_info.render_scene                = m_render_scene;
        main_init_info.directional_light_shadowmap = std::static_pointer_cast<DirectionalLightPass>(m_directional_light_pass)->depth_image;
        main_init_info.shadowmap_sampler           = std::static_pointer_cast<DirectionalLightPass>(m_directional_light_pass)->depth_sampler;
        m_main_pass->Init(&main_init_info);

        ImguiPassInitInfo imgui_init_info{};
        imgui_init_info.rhi           = m_rhi;
        imgui_init_info.asset_manager = m_asset_manager;
        imgui_init_info.render_scene  = m_render_scene;
        m_imgui_pass->Init(&imgui_init_info);

        BasePassInitInfo base_init_info{};
        base_init_info.rhi           = m_rhi;
        base_init_info.asset_manager = m_asset_manager;
        base_init_info.render_scene  = m_render_scene;
        m_base_pass->Init(&base_init_info);

        LightingPassInitInfo lighting_init_info{};
        lighting_init_info.rhi                         = m_rhi;
        lighting_init_info.asset_manager               = m_asset_manager;
        lighting_init_info.render_scene                = m_render_scene;
        lighting_init_info.gbuffer_base_color          = std::static_pointer_cast<BasePass>(m_base_pass)->gbuffer_base_color;
        lighting_init_info.gbuffer_world_normal        = std::static_pointer_cast<BasePass>(m_base_pass)->gbuffer_world_normal;
        lighting_init_info.gbuffer_world_position      = std::static_pointer_cast<BasePass>(m_base_pass)->gbuffer_world_position;
        lighting_init_info.gbuffer_metallic_roughness  = std::static_pointer_cast<BasePass>(m_base_pass)->gbuffer_metallic_roughness;
        lighting_init_info.directional_light_shadowmap = std::static_pointer_cast<DirectionalLightPass>(m_directional_light_pass)->depth_image;
        lighting_init_info.shadowmap_sampler           = std::static_pointer_cast<DirectionalLightPass>(m_directional_light_pass)->depth_sampler;
        m_lighting_pass->Init(&lighting_init_info);

        // DebugDrawPassInitInfo debug_init_info{};
        // debug_init_info.rhi           = m_rhi;
        // debug_init_info.rhi           = m_rhi;
        // debug_init_info.render_scene  = m_render_scene;
        // debug_init_info.image_to_draw = std::static_pointer_cast<DirectionalLightPass>(m_directional_light_pass)->depth_image;
        // m_debug_draw_pass->Init(&debug_init_info);
    }

    void VulkanPipeline::ForwardRender()
    {
        m_rhi->PrepareBeforePass();

        std::dynamic_pointer_cast<DirectionalLightPass>(m_directional_light_pass)->DrawForward();
        // std::dynamic_pointer_cast<MainPass>(m_main_pass)->DrawForward();
        // std::dynamic_pointer_cast<DebugDrawPass>(m_debug_draw_pass)->DrawForward();

        std::dynamic_pointer_cast<BasePass>(m_base_pass)->Draw();
        std::dynamic_pointer_cast<LightingPass>(m_lighting_pass)->Draw();

        std::dynamic_pointer_cast<ImguiPass>(m_imgui_pass)->DrawImgui();

        m_rhi->SubmitRendering();
    }

    vk::DescriptorSetLayout VulkanPipeline::GetMaterialDescriptorSetLayout()
    {
        // return std::reinterpret_pointer_cast<MainPass>(m_main_pass)->GetMaterialDescriptorSetLayout();
        return std::reinterpret_pointer_cast<BasePass>(m_main_pass)->GetMaterialDescriptorSetLayout();
    }

} // namespace Yutrel