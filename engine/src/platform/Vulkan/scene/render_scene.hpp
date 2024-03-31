#pragma once

#include "function/render/renderer.hpp"
#include "platform/Vulkan/asset/vulkan_skybox.hpp"
#include "resource/component/light.hpp"

#include <malloc.h>
#include <vector>

namespace Yutrel
{
    struct VulkanPBRMaterial;
    struct VulkanMesh;
    struct VulkanSkybox;

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

    private:
        void GetDirectionalLightMatrices(Ref<SwapData> pass_data);
        glm::mat4 GetDirectionalLightMatrix(float cur_near_plane, float cur_far_plane);

    public:
        // skybox
        Ref<VulkanSkybox> skybox;

        // 光源
        DirectionalLight directional_light;

        // CSM
        const uint32_t SHADOWMAP_CASCADE_COUNT{4};
        std::vector<glm::mat4> directional_light_VP;
        std::vector<float> cascade_splits;

        // 摄像机
        glm::vec3 camera_position;
        glm::mat4 view_matrix;
        glm::mat4 projection_matrix;
        float fov{70.0f};
        float aspect_radio{1920.0f / 1080.0f};
        float near_plane{0.1f};
        float far_plane{2000.0f};

        // 所有的渲染对象
        std::vector<VulkanRenderObject> render_entities;

        // 水面
        Ref<VulkanMesh> water_plane;
        AllocatedImage water_normal;
        Transform water_transform;

    private:
        Ref<VulkanAssetManager> m_asset_manager;
    };

} // namespace Yutrel