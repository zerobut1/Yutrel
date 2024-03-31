#include "yutrel_pch.hpp"

#include "render_scene.hpp"

#include "platform/Vulkan/asset/vulkan_asset.hpp"

#include <array>
#include <cmath>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/matrix.hpp>
#include <vcruntime.h>

namespace Yutrel
{
    void RenderScene::Init(RenderSceneInitInfo info)
    {
        m_asset_manager = info.asset_manager;
    }

    void RenderScene::ProcessRenderData(Ref<SwapData> pass_data)
    {
        // 摄像机
        camera_position   = pass_data->camera_position;
        aspect_radio      = pass_data->aspect_radio;
        view_matrix       = pass_data->view_matrix;
        projection_matrix = glm::perspective(glm::radians(fov), aspect_radio, far_plane, near_plane);
        projection_matrix[1][1] *= -1;
        // todo 摄像机控制fov和近远平面

        // skybox
        if (!skybox)
        {
            skybox = m_asset_manager->SetSkybox(pass_data->skybox);
        }

        // 水面
        if (!water_plane)
        {
            water_plane     = m_asset_manager->SetVulkanMesh(pass_data->water.plane);
            water_normal    = m_asset_manager->UploadTexture(pass_data->water.normal);
            water_transform = pass_data->water.transform;
        }

        // 平行光的级联阴影投影矩阵
        directional_light = pass_data->directional_light;
        GetDirectionalLightMatrices(pass_data);

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

    void RenderScene::GetDirectionalLightMatrices(Ref<SwapData> pass_data)
    {
        directional_light_VP.resize(SHADOWMAP_CASCADE_COUNT);
        cascade_splits.resize(SHADOWMAP_CASCADE_COUNT);

        // 计算级联的分割平面

        float cascade_split_lambda = 0.95f;

        float range = far_plane - near_plane;
        float ratio = far_plane / near_plane;

        for (size_t i = 0; i < SHADOWMAP_CASCADE_COUNT; i++)
        {
            float p           = (i + 1) / static_cast<float>(SHADOWMAP_CASCADE_COUNT);
            float log         = near_plane * std::pow(ratio, p);
            float uniform     = near_plane + range * p;
            float d           = cascade_split_lambda * (log - uniform) + uniform;
            cascade_splits[i] = d;
        }

        // 计算级联的VP矩阵
        float last_split_dist = 0.0f;
        for (size_t i = 0; i < SHADOWMAP_CASCADE_COUNT; i++)
        {
            float cur_split_dist = (cascade_splits[i] - near_plane) / range;

            std::array<glm::vec3, 8> frustum_corners = {
                glm::vec3(-1.0f, 1.0f, 0.0f),
                glm::vec3(1.0f, 1.0f, 0.0f),
                glm::vec3(1.0f, -1.0f, 0.0f),
                glm::vec3(-1.0f, -1.0f, 0.0f),
                glm::vec3(-1.0f, 1.0f, 1.0f),
                glm::vec3(1.0f, 1.0f, 1.0f),
                glm::vec3(1.0f, -1.0f, 1.0f),
                glm::vec3(-1.0f, -1.0f, 1.0f),
            };

            // 获取完整级联视见体的八个角落的世界空间坐标
            glm::mat4 inv_cam = glm::inverse(projection_matrix * view_matrix);
            // glm::mat4 inv_cam = glm::inverse(projection_matrix);
            for (size_t j = 0; j < 8; j++)
            {
                glm::vec4 inv_corner = inv_cam * glm::vec4(frustum_corners[j], 1.0f);
                frustum_corners[j]   = inv_corner / inv_corner.w;
            }

            // 获取当前级联视见体的八个角落的世界空间坐标
            for (size_t j = 0; j < 4; j++)
            {
                glm::vec3 dist         = frustum_corners[j] - frustum_corners[j + 4];
                frustum_corners[j]     = frustum_corners[j + 4] + (dist * last_split_dist);
                frustum_corners[j + 4] = frustum_corners[j + 4] + (dist * cur_split_dist);
            }

            // 获取当前级联视见体中心的世界空间坐标
            glm::vec3 frustum_center = glm::vec3(0.0f);
            for (size_t j = 0; j < 8; j++)
            {
                frustum_center += frustum_corners[j];
            }
            frustum_center /= 8.0f;

            // 获取当前级联视见体的最大对角线长度
            // 则为包围球的半径
            glm::vec3 diag_1 = frustum_corners[0] - frustum_corners[6];
            glm::vec3 diag_2 = frustum_corners[0] - frustum_corners[2];
            float radius     = std::max(glm::length(diag_1), glm::length(diag_2)) / 2.0f;

            // 计算阴影贴图中一格像素在光源观察空间的长宽
            float units_per_pixel = radius * 2.0f / 4096.0f;

            // 光源视角的观察矩阵（临时）
            glm::mat4 light_view = glm::lookAt(glm::vec3(0.0f), -directional_light.direction, glm::vec3(0.0f, 1.0f, 0.0f));

            // 当前级联视见体中心的光源观察空间坐标
            glm::vec4 frustum_center_light_space = light_view * glm::vec4(frustum_center, 1.0f);

            // 使当前级联视见体以unit的整数倍为单位进行移动
            frustum_center_light_space.x = std::ceil(frustum_center_light_space.x / units_per_pixel) * units_per_pixel;
            frustum_center_light_space.y = std::ceil(frustum_center_light_space.y / units_per_pixel) * units_per_pixel;
            frustum_center_light_space.z = std::ceil(frustum_center_light_space.z / units_per_pixel) * units_per_pixel;

            frustum_center = glm::inverse(light_view) * frustum_center_light_space;

            // todo 根据场景的AABB计算投影矩阵的远近平面

            // 光源视角的VP矩阵
            light_view           = glm::lookAt(frustum_center, frustum_center + directional_light.direction, glm::vec3(0.0f, 1.0f, 0.0f));
            glm::mat4 light_proj = glm::ortho(-radius, radius, -radius, radius, radius * 10.0f, -radius * 10.0f);
            light_proj[1][1] *= -1;

            directional_light_VP[i] = light_proj * light_view;

            last_split_dist = cur_split_dist;
        }
    }

} // namespace Yutrel