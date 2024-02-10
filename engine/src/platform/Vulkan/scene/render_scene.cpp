#include "yutrel_pch.hpp"

#include "render_scene.hpp"

#include "platform/Vulkan/asset/vulkan_asset.hpp"

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/matrix.hpp>

namespace Yutrel
{
    void RenderScene::Init(RenderSceneInitInfo info)
    {
        m_asset_manager = info.asset_manager;
    }

    void RenderScene::ProcessRenderData(Ref<SwapData> pass_data)
    {
        // 光源
        directional_light = pass_data->directional_light;
        // 平行光VP矩阵
        glm::vec3 directional_light_pos = glm::vec3();
        directional_light_pos.x         = -directional_light.direction.x * 30.0f;
        directional_light_pos.y         = -directional_light.direction.y * 30.0f;
        directional_light_pos.z         = -directional_light.direction.z * 30.0f;

        glm::mat4 directional_light_view = glm::lookAt(directional_light_pos, glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        glm::mat4 directional_light_proj = glm::ortho(-20.0f, 20.0f, -20.0f, 20.0f, 100.0f, 0.1f);
        directional_light_proj[1][1] *= -1;
        directional_light_VP = directional_light_proj * directional_light_view;

        // 摄像机
        camera_position = pass_data->camera_position;
        view_matrix     = pass_data->view_matrix;
        // todo 摄像机控制fov和近远平面

        render_entities = std::vector<VulkanRenderObject>();

        for (auto& object : pass_data->objects)
        {
            auto& vulkan_object = render_entities.emplace_back(VulkanRenderObject{});

            // mesh加载到GPU，并释放内存
            vulkan_object.mesh = m_asset_manager->SetVulkanMesh(object.mesh);

            // 材质的数据加载到GPU
            vulkan_object.material = m_asset_manager->SetVulkanMaterial(object.material);

            vulkan_object.model_matrix = object.transform.model_matrix;
        }
    }

} // namespace Yutrel