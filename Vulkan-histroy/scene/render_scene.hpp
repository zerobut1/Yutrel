#pragma once

#include "function/render/renderer.hpp"
#include "resource/component/light.hpp"

namespace Yutrel
{
    struct VulkanPBRMaterial;
    struct VulkanMesh;

    struct VulkanRenderObject
    {
        Ref<VulkanMesh> mesh;
        Ref<VulkanPBRMaterial> material;
        glm::mat4 model_matrix;
    };

    struct RenderSceneInitInfo
    {
        Ref<class VulkanAssetManager> asset_manager;
    };

    class RenderScene
    {
    public:
        void Init(RenderSceneInitInfo info);

        void ProcessRenderData(Ref<SwapData> pass_data);

    public:
        // 光源
        DirectionalLight directional_light;
        glm::mat4 directional_light_VP;

        // 摄像机
        glm::vec3 camera_position;
        glm::mat4 view_matrix;
        float fov{70.0f};
        float near_plane{0.1f};
        float far_plane{10000.0f};

        // 所有的渲染对象
        std::vector<VulkanRenderObject> m_render_entities;

    private:
        Ref<VulkanAssetManager> m_asset_manager;
    };

} // namespace Yutrel