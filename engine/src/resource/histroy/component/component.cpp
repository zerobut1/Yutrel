// #include "yutrel_pch.hpp"

// #include "component.hpp"

// #include "core/application/application.hpp"
// #include "core/sync/sync.hpp"
// #include "function/render/renderer.hpp"

// #include <glad/glad.h>
// #include <stdint.h>

// namespace Yutrel
// {
//     SkyBox::SkyBox(const std::string& path)
//     {
//         environment_map = Texture::Create(path, (uint32_t)2048, 0);

//         auto resources   = Resources(Application::GetInstance().GetWorld());
//         auto renderer    = resources.Get<Ref<Renderer>>();
//         auto render_data = renderer->GetRenderData();

//         renderer->SeamlessCubemap(true);
//         renderer->DepthTest(false);
//         renderer->FaceCulling(true);

//         auto irradiance_shader = render_data->irradiance_shader;
//         auto prefilter_shader  = render_data->prefilter_shader;

//         environment_map->Bind(0);

//         irradiance_map = Texture::Create(GL_TEXTURE_CUBE_MAP, 128, 128, 6, GL_RGBA16F, 1);
//         prefilter_map  = Texture::Create(GL_TEXTURE_CUBE_MAP, 2048, 2048, 6, GL_RGBA16F, 8);

//         LOG_INFO("Precomputing diffuse irradiance map from {0}", path);
//         irradiance_map->BindILS(0, 0, GL_WRITE_ONLY);

//         if (irradiance_shader->Bind(); true)
//         {
//             irradiance_shader->Dispatch(128 / 32, 128 / 32, 6);
//             irradiance_shader->SyncWait(GL_TEXTURE_FETCH_BARRIER_BIT | GL_ATOMIC_COUNTER_BARRIER_BIT);

//             auto irradiance_fence = Sync(0);
//             irradiance_fence.ClientWaitSync();
//             irradiance_map->UnbindILS(0);
//         }

//         LOG_INFO("Precomputing specular prefiltered envmap from {0}", path);
//         Texture::Copy(environment_map, 0, prefilter_map, 0); // copy the base level

//         const GLuint max_level = prefilter_map->GetLevels() - 1;
//         GLuint resolution      = prefilter_map->GetWidth() / 2;
//         prefilter_shader->Bind();

//         for (unsigned int level = 1; level <= max_level; level++, resolution /= 2)
//         {
//             float roughness = level / static_cast<float>(max_level);
//             GLuint n_groups = glm::max<GLuint>(resolution / 32, 1);

//             prefilter_map->BindILS(level, 1, GL_WRITE_ONLY);
//             prefilter_shader->SetFloat("roughness", roughness);
//             prefilter_shader->Dispatch(n_groups, n_groups, 6);
//             prefilter_shader->SyncWait(GL_TEXTURE_FETCH_BARRIER_BIT | GL_ATOMIC_COUNTER_BARRIER_BIT);

//             auto prefilter_fence = Sync(level);
//             prefilter_fence.ClientWaitSync();
//             prefilter_map->UnbindILS(1);
//         }
//         renderer->FaceCulling(false);
//     }
// } // namespace Yutrel