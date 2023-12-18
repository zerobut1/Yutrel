#pragma once

#include "glm/fwd.hpp"
#include "ui/editor_ui.hpp"

#include <Yutrel.hpp>
#include <glm/glm.hpp>
#include <memory>
#include <stdint.h>

struct Framebuffers
{
    Yutrel::Ref<Yutrel::FrameBuffer> viewport;

    Yutrel::Ref<Yutrel::FrameBuffer> G;
    Yutrel::Ref<Yutrel::FrameBuffer> ssao;

    Yutrel::Ref<Yutrel::FrameBuffer> opaque;
    Yutrel::Ref<Yutrel::FrameBuffer> transparent;

    Yutrel::Ref<Yutrel::FrameBuffer> shadow_csm;
    Yutrel::Ref<Yutrel::FrameBuffer> shadow_osm;

    Framebuffers();
    Framebuffers(uint32_t width, uint32_t height);
};

struct Shaders
{
    Yutrel::Ref<Yutrel::Shader> g_buffer;
    Yutrel::Ref<Yutrel::Shader> ssao;
    Yutrel::Ref<Yutrel::Shader> pbr;
    Yutrel::Ref<Yutrel::Shader> transparent;
    Yutrel::Ref<Yutrel::Shader> composite;
    Yutrel::Ref<Yutrel::Shader> animation;
    Yutrel::Ref<Yutrel::Shader> csm;
    Yutrel::Ref<Yutrel::Shader> osm;

    Shaders();
};

struct UniformBuffers
{
    Yutrel::Ref<Yutrel::UniformBuffer> camera;
    Yutrel::Ref<Yutrel::UniformBuffer> sun;
    Yutrel::Ref<Yutrel::UniformBuffer> csm_light_matrices;

    UniformBuffers();
};

struct CameraData
{
    glm::vec4 position;
    glm::mat4 view;
    glm::mat4 projection;
};

struct SunData
{
    glm::vec4 color;
    glm::vec4 direction;
    glm::vec4 intensity; // 不明原因传不进去，先用vec4凑合
    glm::mat4 light_matrix[4];
};

void SpawnCamera(Yutrel::Commands& cmd, Yutrel::Resources resources);
void SpawnLight(Yutrel::Commands& cmd, Yutrel::Resources resources);
void SpawnSkybox(Yutrel::Commands& cmd, Yutrel::Resources resources);

void SpawnPBRScene(Yutrel::Commands& cmd, Yutrel::Resources resources);
void DrawPBRScene(Yutrel::Commands& cmd, Yutrel::Querier querier, Yutrel::Resources resources, Yutrel::Events& events);

void UpdateScene(Yutrel::Commands& cmd, Yutrel::Querier querier, Yutrel::Resources resources, Yutrel::Events& events);

// 显示UI
void DrawUI(Yutrel::Commands& cmd, Yutrel::Querier querier, Yutrel::Resources resources, Yutrel::Events& events);

// CSM
extern const float camera_near_plane;
extern const float camera_far_plane;
extern const std::vector<float> shadow_cascade_levels;

std::vector<glm::mat4> GetLightSpaceMatrices();
glm::mat4 GetLightSpaceMatrix(const float near_plane, const float far_plane);
std::vector<glm::vec4> GetFrustumCornersWorldSpace(const glm::mat4& proj, const glm::mat4& view);