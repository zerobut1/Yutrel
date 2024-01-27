#pragma once

#include "function/render/renderer.hpp"
#include "platform/Vulkan/vulkan_types.hpp"

#include <unordered_map>
#include <vector>

namespace Yutrel
{
    class VulkanRHI;
    class VulkanPipeline;
    class VulkanAssetManager;

    struct RenderData
    {
        glm::mat4 directional_light_MVP;
        std::unordered_map<Ref<struct VulkanPBRMaterial>, std::unordered_map<Ref<struct VulkanMesh>, std::vector<glm::mat4>>> objects;
    };

    class VulkanRenderer final : public Renderer
    {
    public:
        virtual void Init(RendererInitInfo info) override;

        virtual void Tick(Ref<SwapData> swap_data) override;

        virtual void Clear() override;

        virtual void UpdateWindowSize(uint32_t width, uint32_t height) override;

    private:
        void ProcessRenderData(Ref<SwapData> swap_data);

        void NewImguiFrame(Ref<WindowUI> ui);

    private:
        Ref<VulkanRHI> m_rhi;
        Ref<VulkanPipeline> m_render_pipeline;
        Ref<VulkanAssetManager> m_asset_manager;

        Ref<RenderData> m_render_data;
    };
} // namespace Yutrel