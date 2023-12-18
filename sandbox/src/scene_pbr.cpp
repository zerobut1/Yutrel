// #include "scene_pbr.hpp"

// #include "glm/ext/matrix_transform.hpp"
// #include "glm/ext/vector_float3.hpp"
// #include "glm/fwd.hpp"
// #include "glm/geometric.hpp"
// #include "imgui.h"
// #include <glm/glm.hpp>

// #include <random>
// #include <stdint.h>
// #include <string>
// #include <utility>

// Framebuffers::Framebuffers(uint32_t width, uint32_t height)
// {
//     viewport = Yutrel::FrameBuffer::Create(width, height);
//     viewport->AddColorTexture(1);

//     G = Yutrel::FrameBuffer::Create(width, height);
//     G->AddColorTexture(5);
//     G->AddDepStRenderBuffer();

//     ssao = Yutrel::FrameBuffer::Create(width, height);
//     ssao->AddColorTexture(1);

//     opaque = Yutrel::FrameBuffer::Create(width, height);
//     opaque->AddColorTexture(1);
//     opaque->AddDepStRenderBuffer();

//     transparent = Yutrel::FrameBuffer::Create(width, height);
//     transparent->AddColorTexture(2);
//     transparent->AddDepStRenderBuffer();

//     shadow_csm = Yutrel::FrameBuffer::Create(4096, 4096);
//     shadow_csm->AddDepthTexture3D();

//     shadow_osm = Yutrel::FrameBuffer::Create(2048, 2048);
//     shadow_osm->AddDepthCubemap();
// }

// Shaders::Shaders()
// {
//     g_buffer    = Yutrel::Shader::Create("engine/asset/shader/g_buffer.vert", "engine/asset/shader/g_buffer.frag");
//     ssao        = Yutrel::Shader::Create("engine/asset/shader/to_screen.vert", "engine/asset/shader/ssao.frag");
//     // csm         = Yutrel::Shader::Create("engine/asset/shader/shadow.vert", "engine/asset/shader/csm.frag", "engine/asset/shader/csm.geom");
//     // osm         = Yutrel::Shader::Create("engine/asset/shader/shadow.vert", "engine/asset/shader/osm.frag", "engine/asset/shader/osm.geom");
//     pbr         = Yutrel::Shader::Create("engine/asset/shader/to_screen.vert", "engine/asset/shader/pbr.frag");
//     transparent = Yutrel::Shader::Create("engine/asset/shader/transparent.vert", "engine/asset/shader/transparent.frag");
//     composite   = Yutrel::Shader::Create("engine/asset/shader/to_screen.vert", "engine/asset/shader/composite.frag");
// }

// UniformBuffers::UniformBuffers()
// {
//     camera = Yutrel::UniformBuffer::Create(sizeof(CameraData), 0);
//     sun    = Yutrel::UniformBuffer::Create(sizeof(SunData), 1);
// }

// void SpawnCamera(Yutrel::Commands& cmd, Yutrel::Resources resources)
// {
//     cmd.Spawn<Yutrel::Ref<Yutrel::CameraController>>(
//         Yutrel::CameraController::Create((1920.0f / 1080.0f), glm::vec3(0.0f, 5.0f, 0.0f)));

//     // todo set direction
// }

// void SpawnLight(Yutrel::Commands& cmd, Yutrel::Resources resources)
// {
//     // SUN (direction light)
//     cmd.Spawn<Yutrel::DirectionLight>(
//         Yutrel::DirectionLight(glm::vec3(1.0f), glm::normalize(glm::vec3(1.0f, -1.0f, -1.0f))));

//     // 点光源
//     Yutrel::Transform transform;
//     transform.Translate(15.6191521, 3.27584434, -0.9174349);
//     transform.Rotate(2.98023224E-08, -0.649522066, 0.0, 0.7603427, Yutrel::Space::Local);

//     cmd.Spawn<Yutrel::PointLight, Yutrel::Transform>(
//         Yutrel::PointLight(glm::vec3(1.0, 0.456014365, 0.114013553)),
//         std::move(transform));
// }

// void SpawnSkybox(Yutrel::Commands& cmd, Yutrel::Resources resources)
// {
//     auto paths = resources.Get<Yutrel::Paths>();

//     auto skybox     = Yutrel::SkyBox(paths.resource + "texture\\hdr\\quarry_04_4k.hdr");
//     skybox.exposure = 0.1f;

//     cmd.Spawn<Yutrel::SkyBox>(std::move(skybox));
// }

// void SpawnPBRScene(Yutrel::Commands& cmd, Yutrel::Resources resources)
// {
//     auto paths = resources.Get<Yutrel::Paths>();

//     // Sponza
//     auto model = Yutrel::Model::Create(paths.resource + "scene\\Main.1_Sponza\\NewSponza_Main_glTF_002.gltf", false);
//     model->SetPBRMaterial();
//     // model->SetLight();

//     cmd.Spawn<Yutrel::Ref<Yutrel::Model>, Yutrel::Transform, Yutrel::Scene>(
//         std::move(model),
//         Yutrel::Transform(),
//         Yutrel::Scene{});

//     // // 测试用金属球
//     // model = resources.Get<Yutrel::RenderData>().sphere_model;
//     // Yutrel::Material material;
//     // material.SetUniform("u_albedo", glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
//     // material.SetUniform("u_metallic", 1.0f);
//     // material.SetUniform("u_roughness", 0.0f);
//     // material.SetUniform("u_blend_toggle", 0.0f);
//     // model->SetMaterial("DefaultMaterial", material);
//     // // model->SetMaterial("Material", material);

//     // model_matrix = glm::mat4(1.0f);
//     // model_matrix = glm::translate(model_matrix, glm::vec3(5.0f, 0.0f, 0.0f));

//     // cmd.Spawn<Yutrel::Model*, Yutrel::Transform, Yutrel::Scene>(
//     //     std::move(model),
//     //     Yutrel::Transform{model_matrix},
//     //     Yutrel::Scene{});
// }

// void DrawPBRScene(Yutrel::Commands& cmd, Yutrel::Querier querier, Yutrel::Resources resources, Yutrel::Events& events)
// {
//     // 获取摄像机
//     auto camera_controller_entity = querier.Query<Yutrel::Ref<Yutrel::CameraController>>()[0];
//     auto camera_controller        = querier.Get<Yutrel::Ref<Yutrel::CameraController>>(camera_controller_entity);
//     auto camera                   = camera_controller->GetCamera();
//     CameraData camera_data        = {glm::vec4(camera->getPosition(), 0.0),
//                                      camera->getViewMatrix(),
//                                      camera->getProjectionMatrix()};

//     // 获取renderer
//     auto renderer = resources.Get<Yutrel::Ref<Yutrel::Renderer>>();
//     renderer->FaceCulling(true);

//     // 获取skybox
//     auto skybox_entity  = querier.Query<Yutrel::SkyBox>()[0];
//     auto skybox         = querier.Get<Yutrel::SkyBox>(skybox_entity);
//     auto irradiance_map = querier.Get<Yutrel::SkyBox>(skybox_entity).irradiance_map;
//     auto prefilter_map  = querier.Get<Yutrel::SkyBox>(skybox_entity).prefilter_map;
//     auto brdf_lut       = renderer->GetRenderData()->brdf_LUT;

//     // 获取framebuffers
//     auto framebuffers = resources.Get<Framebuffers>();

//     // 获取shaders
//     auto shaders = resources.Get<Shaders>();

//     // 获取sun
//     auto sun         = querier.Get<Yutrel::DirectionLight>(querier.Query<Yutrel::DirectionLight>()[0]);
//     SunData sun_data = {glm::vec4(sun.color, 1.0f),
//                         glm::vec4(sun.direction, 0.0f),
//                         glm::vec4(sun.intensity)};
//     // 获取级联光照视角投影矩阵
//     auto light_matrices = GetLightSpaceMatrices();
//     for (int i = 0; i < light_matrices.size(); i++)
//     {
//         sun_data.light_matrix[i] = light_matrices[i];
//     }

//     // 获取点光源
//     auto point_light_entity   = querier.Query<Yutrel::PointLight>()[0];
//     auto point_light          = querier.Get<Yutrel::PointLight>(point_light_entity);
//     auto point_light_position = querier.Get<Yutrel::Transform>(point_light_entity).position;

//     // 设置uniformbuffer
//     resources.Get<UniformBuffers>().camera->SetData(&camera_data, sizeof(CameraData));
//     resources.Get<UniformBuffers>().sun->SetData(&sun_data, sizeof(SunData));

//     ////////////////////////////////////////
//     // 阶段一 渲染平行光的级联阴影贴图
//     ///////////////////////////////////////

//     framebuffers.shadow_csm->Bind();
//     framebuffers.shadow_csm->Clear();
//     renderer->Clear();
//     renderer->SetViewport(4096, 4096);
//     shaders.csm->Bind();
//     // renderer->SetCullFace(1);

//     // test 看看需不需要裁剪
//     renderer->RenderScene(shaders.csm, camera_controller);

//     // renderer->SetCullFace(0);
//     renderer->SetViewport(framebuffers.viewport->GetWidth(), framebuffers.viewport->GetHeight());
//     framebuffers.shadow_csm->Unbind();

//     ////////////////////////////////////////
//     //  渲染点光源的万象阴影贴图
//     ///////////////////////////////////////

//     // 点光源的视角投影矩阵
//     float near_plane     = 1.0f;
//     float far_plane      = 25.0f;
//     glm::mat4 shadowProj = glm::perspective(glm::radians(90.0f), (float)2048 / (float)2048, near_plane, far_plane);
//     std::vector<glm::mat4> shadowTransforms;
//     shadowTransforms.push_back(shadowProj * glm::lookAt(point_light_position, point_light_position + glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
//     shadowTransforms.push_back(shadowProj * glm::lookAt(point_light_position, point_light_position + glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
//     shadowTransforms.push_back(shadowProj * glm::lookAt(point_light_position, point_light_position + glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)));
//     shadowTransforms.push_back(shadowProj * glm::lookAt(point_light_position, point_light_position + glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f)));
//     shadowTransforms.push_back(shadowProj * glm::lookAt(point_light_position, point_light_position + glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
//     shadowTransforms.push_back(shadowProj * glm::lookAt(point_light_position, point_light_position + glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, -1.0f, 0.0f)));

//     framebuffers.shadow_osm->Bind();
//     renderer->Clear();
//     renderer->SetViewport(2048, 2048);

//     shaders.osm->Bind();
//     for (uint32_t i = 0; i < 6; i++)
//     {
//         shaders.osm->SetMat4("u_shadow_matrices[" + std::to_string(i) + "]", shadowTransforms[i]);
//     }
//     shaders.osm->SetFloat("u_far_plane", far_plane);
//     shaders.osm->SetFloat3("u_light_position", point_light_position);

//     renderer->RenderScene(shaders.osm);

//     renderer->SetViewport(framebuffers.viewport->GetWidth(), framebuffers.viewport->GetHeight());
//     framebuffers.shadow_osm->Unbind();

//     ////////////////////////////////////////
//     // 阶段一  G_BUFFER
//     ///////////////////////////////////////

//     framebuffers.G->Bind();
//     renderer->Clear();

//     shaders.g_buffer->Bind();
//     renderer->RenderScene(shaders.g_buffer, camera_controller, true, 1);

//     framebuffers.G->Unbind();

//     ////////////////////////////////////////
//     // 阶段一  SSAO
//     ///////////////////////////////////////

//     // 生成随机的采样样本
//     std::uniform_real_distribution<float> randomFloats(0.0, 1.0);
//     std::default_random_engine generator;
//     std::vector<glm::vec3> ssaoKernel;
//     auto Lerp = [](float a, float b, float f)
//     {
//         return a + f * (b - a);
//     };
//     for (unsigned int i = 0; i < 64; ++i)
//     {
//         glm::vec3 sample(randomFloats(generator) * 2.0 - 1.0, randomFloats(generator) * 2.0 - 1.0, randomFloats(generator));
//         sample = glm::normalize(sample);
//         sample *= randomFloats(generator);
//         float scale = float(i) / 64.0f;

//         // scale samples s.t. they're more aligned to center of kernel
//         scale = Lerp(0.1f, 1.0f, scale * scale);
//         sample *= scale;
//         ssaoKernel.push_back(sample);
//     }

//     framebuffers.ssao->Bind();
//     renderer->Clear();

//     shaders.ssao->Bind();
//     for (int i = 0; i < ssaoKernel.size(); i++)
//     {
//         shaders.ssao->SetFloat3("u_samples[" + std::to_string(i) + "]", ssaoKernel[i]);
//     }
//     framebuffers.G->GetColorTexture(0)->Bind(20);
//     framebuffers.G->GetColorTexture(1)->Bind(21);

//     renderer->RenderShader(shaders.ssao);

//     framebuffers.ssao->Unbind();

//     ////////////////////////////////////////
//     // 阶段二 延迟渲染不透明物体
//     ///////////////////////////////////////

//     renderer->SetTranspatent(0);

//     framebuffers.opaque->Bind();
//     renderer->Clear();

//     shaders.pbr->Bind();
//     shaders.pbr->SetFloat("u_ibl_exposure", skybox.exposure);
//     // csm
//     shaders.pbr->SetInt("u_cascade_count", shadow_cascade_levels.size());
//     shaders.pbr->SetFloat("u_csm_far_plane", camera_far_plane);
//     shaders.pbr->SetFloatArray("u_cascade_plane_distances", &shadow_cascade_levels[0], shadow_cascade_levels.size());
//     // osm
//     shaders.pbr->SetFloat3("u_point_light_position", point_light_position);
//     shaders.pbr->SetFloat("u_osm_far_plane", far_plane);
//     shaders.pbr->SetFloat3("u_point_light_color", point_light.color);
//     shaders.pbr->SetFloat("u_point_light_intensity", point_light.intensity);

//     irradiance_map->Bind(11);
//     prefilter_map->Bind(12);
//     brdf_lut->Bind(13);
//     framebuffers.shadow_csm->GetDepthTexture()->Bind(14);
//     framebuffers.shadow_osm->GetDepthTexture()->Bind(15);
//     framebuffers.G->GetColorTexture(0)->Bind(20);
//     framebuffers.G->GetColorTexture(1)->Bind(21);
//     framebuffers.G->GetColorTexture(2)->Bind(22);
//     framebuffers.G->GetColorTexture(3)->Bind(23);
//     framebuffers.G->GetColorTexture(4)->Bind(24);
//     framebuffers.ssao->GetColorTexture(0)->Bind(25);

//     renderer->RenderShader(shaders.pbr);

//     framebuffers.opaque->Unbind();

//     ////////////////////////////////////////
//     // 阶段三 渲染透明物体
//     ///////////////////////////////////////

//     renderer->SetTranspatent(1);

//     framebuffers.transparent->Bind();
//     renderer->Clear();
//     framebuffers.transparent->Clear(0, glm::vec4(0.0f));
//     framebuffers.transparent->Clear(1, glm::vec4(1.0f));

//     Yutrel::FrameBuffer::CopyDepth(framebuffers.G, framebuffers.transparent);

//     shaders.transparent->Bind();
//     shaders.transparent->SetFloat("u_ibl_exposure", skybox.exposure);
//     // csm
//     shaders.transparent->SetInt("u_cascade_count", shadow_cascade_levels.size());
//     shaders.transparent->SetFloat("u_csm_far_plane", camera_far_plane);
//     shaders.transparent->SetFloatArray("u_cascade_plane_distances", &shadow_cascade_levels[0], shadow_cascade_levels.size());
//     // osm
//     shaders.transparent->SetFloat3("u_point_light_position", point_light_position);
//     shaders.transparent->SetFloat("u_osm_far_plane", far_plane);
//     shaders.transparent->SetFloat3("u_point_light_color", point_light.color);
//     shaders.transparent->SetFloat("u_point_light_intensity", point_light.intensity);

//     irradiance_map->Bind(11);
//     prefilter_map->Bind(12);
//     brdf_lut->Bind(13);
//     framebuffers.shadow_csm->GetDepthTexture()->Bind(14);
//     framebuffers.shadow_osm->GetDepthTexture()->Bind(15);

//     renderer->RenderScene(shaders.transparent, camera_controller, true, 2);

//     framebuffers.transparent->Unbind();

//     ////////////////////////////////////////
//     // 阶段四 混合
//     ///////////////////////////////////////
//     renderer->SetTranspatent(2);

//     Yutrel::FrameBuffer::CopyDepth(framebuffers.G, framebuffers.opaque);

//     framebuffers.opaque->Bind();
//     // renderer->Clear();

//     shaders.composite->Bind();
//     framebuffers.transparent->GetColorTexture(0)->Bind(0);
//     framebuffers.transparent->GetColorTexture(1)->Bind(1);

//     renderer->RenderShader(shaders.composite);

//     renderer->RenderSkybox(skybox);

//     framebuffers.opaque->Unbind();

//     ////////////////////////////////////////
//     // 阶段五 绘制到viewport
//     ///////////////////////////////////////

//     renderer->SetTranspatent(0);
//     renderer->Clear();
//     framebuffers.viewport->Bind();
//     framebuffers.opaque->Draw(0);
//     framebuffers.viewport->Unbind();
// }

// // 暂时这么定义
// // todo 修改camera
// const float camera_near_plane = 0.1f;
// const float camera_far_plane  = 100.0f;
// // 分割线
// const std::vector<float> shadow_cascade_levels{camera_far_plane / 10.0f, camera_far_plane / 5.0f, camera_far_plane / 2.0f};

// // 获取定向光的各个级联的视角投影矩阵
// std::vector<glm::mat4> GetLightSpaceMatrices()
// {
//     std::vector<glm::mat4> ret;
//     for (size_t i = 0; i < shadow_cascade_levels.size() + 1; ++i)
//     {
//         if (i == 0)
//         {
//             ret.push_back(GetLightSpaceMatrix(camera_near_plane, shadow_cascade_levels[i]));
//         }
//         else if (i < shadow_cascade_levels.size())
//         {
//             ret.push_back(GetLightSpaceMatrix(shadow_cascade_levels[i - 1], shadow_cascade_levels[i]));
//         }
//         else
//         {
//             ret.push_back(GetLightSpaceMatrix(shadow_cascade_levels[i - 1], camera_far_plane));
//         }
//     }
//     return ret;
// }

// // 得到一定范围内的光线视角投影矩阵
// glm::mat4 GetLightSpaceMatrix(const float nearPlane, const float farPlane)
// {
//     auto querier              = Yutrel::Querier(Yutrel::Application::GetInstance().GetWorld());
//     auto resources            = Yutrel::Resources(Yutrel::Application::GetInstance().GetWorld());
//     auto camera_controller    = querier.Get<Yutrel::Ref<Yutrel::CameraController>>(querier.Query<Yutrel::Ref<Yutrel::CameraController>>()[0]);
//     auto viewport_framebuffer = resources.Get<Framebuffers>().viewport;

//     auto proj = glm::perspective(
//         glm::radians(camera_controller->GetZoomLevel()),
//         (float)viewport_framebuffer->GetWidth() / (float)viewport_framebuffer->GetHeight(),
//         nearPlane,
//         farPlane);

//     // 得到视见体的八个角落
//     auto corners = GetFrustumCornersWorldSpace(proj, camera_controller->GetCamera()->getViewMatrix());

//     // 得到视见体的中心点
//     glm::vec3 center = glm::vec3(0, 0, 0);
//     for (const auto& v : corners)
//     {
//         center += glm::vec3(v);
//     }
//     center /= corners.size();

//     auto sun = querier.Get<Yutrel::DirectionLight>(querier.Query<Yutrel::DirectionLight>()[0]);

//     const auto lightView = glm::lookAt(center - sun.direction, center, glm::vec3(0.0f, 1.0f, 0.0f));

//     float minX = std::numeric_limits<float>::max();
//     float maxX = std::numeric_limits<float>::lowest();
//     float minY = std::numeric_limits<float>::max();
//     float maxY = std::numeric_limits<float>::lowest();
//     float minZ = std::numeric_limits<float>::max();
//     float maxZ = std::numeric_limits<float>::lowest();
//     for (const auto& v : corners)
//     {
//         const auto trf = lightView * v;
//         minX           = std::min(minX, trf.x);
//         maxX           = std::max(maxX, trf.x);
//         minY           = std::min(minY, trf.y);
//         maxY           = std::max(maxY, trf.y);
//         minZ           = std::min(minZ, trf.z);
//         maxZ           = std::max(maxZ, trf.z);
//     }

//     // Tune this parameter according to the scene
//     constexpr float zMult = 10.0f;
//     if (minZ < 0)
//     {
//         minZ *= zMult;
//     }
//     else
//     {
//         minZ /= zMult;
//     }
//     if (maxZ < 0)
//     {
//         maxZ /= zMult;
//     }
//     else
//     {
//         maxZ *= zMult;
//     }

//     const glm::mat4 lightProjection = glm::ortho(minX, maxX, minY, maxY, minZ, maxZ);
//     return lightProjection * lightView;
// }

// // 得到视见体八个角落的世界空间坐标
// std::vector<glm::vec4> GetFrustumCornersWorldSpace(const glm::mat4& proj, const glm::mat4& view)
// {
//     const auto inv = glm::inverse(proj * view);

//     std::vector<glm::vec4> frustumCorners;
//     for (unsigned int x = 0; x < 2; ++x)
//     {
//         for (unsigned int y = 0; y < 2; ++y)
//         {
//             for (unsigned int z = 0; z < 2; ++z)
//             {
//                 const glm::vec4 pt = inv * glm::vec4(2.0f * x - 1.0f, 2.0f * y - 1.0f, 2.0f * z - 1.0f, 1.0f);
//                 frustumCorners.push_back(pt / pt.w);
//             }
//         }
//     }

//     return frustumCorners;
// }

// void UpdateScene(Yutrel::Commands& cmd, Yutrel::Querier querier, Yutrel::Resources resources, Yutrel::Events& events)
// {
//     // 获取摄像机
//     auto camera_controller_entity = querier.Query<Yutrel::Ref<Yutrel::CameraController>>()[0];
//     auto camera_controller        = querier.Get<Yutrel::Ref<Yutrel::CameraController>>(camera_controller_entity);

//     // 获取 framebuffer
//     auto& framebuffers = resources.Get<Framebuffers>();
//     auto viewport      = resources.Get<Yutrel::Ref<EditorUI>>()->GetViewport();

//     // viewport有变化则更新
//     if (viewport.width > 0.0f && viewport.height > 0.0f &&
//         (framebuffers.viewport->GetWidth() != viewport.width || framebuffers.viewport->GetHeight() != viewport.height))
//     {
//         framebuffers = Framebuffers(viewport.width, viewport.height);

//         resources.Get<Yutrel::Ref<Yutrel::Renderer>>()->SetViewport(viewport.width, viewport.height);

//         camera_controller->Resize((float)viewport.width / (float)viewport.height);
//     }

//     // camera
//     // if (m_viewport_focused)
//     {
//         // 更新摄像机
//         float delta_time = resources.Get<Yutrel::Ref<Yutrel::Time>>()->GetDeltaTime();
//         // float delta_time = Yutrel::Time::GetDeltaTime();
//         camera_controller->Tick(delta_time);
//     }
// }

// void DrawUI(Yutrel::Commands& cmd, Yutrel::Querier querier, Yutrel::Resources resources, Yutrel::Events& events)
// {
//     auto ui       = resources.Get<Yutrel::Ref<EditorUI>>();
//     auto renderer = resources.Get<Yutrel::Ref<Yutrel::Renderer>>();
//     renderer->RenderUI(ui);
// }