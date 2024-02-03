#pragma once

#include "function/window/window.hpp"
#include "resource/component/component.hpp"
#include "resource/component/light.hpp"

#include <stdint.h>
#include <vector>

struct GLFWwindow;

namespace Yutrel
{
    struct RendererInitInfo
    {
        GLFWwindow* raw_window;
        uint32_t width;
        uint32_t height;
    };

    struct RenderObject
    {
        Ref<Mesh> mesh;
        Ref<Material> material;
        Transform transform;
    };

    struct SwapData
    {
        Ref<class WindowUI> ui;

        glm::mat4 view_matrix;
        glm::vec3 camera_position;

        DirectionalLight directional_light;

        std::vector<RenderObject> objects;
    };

    class Renderer
    {
    public:
        virtual void Init(RendererInitInfo info) = 0;

        virtual void Tick(Ref<SwapData> swap_data) = 0;

        virtual void Clear() = 0;

        virtual void UpdateWindowSize(uint32_t width, uint32_t height) = 0;
    };

    class RendererResource
    {
    public:
        RendererResource();
        ~RendererResource();

        static void Init(gecs::resource<RendererResource> render,
                         gecs::resource<WindowResource> window);

        static void Update(gecs::querier<Ref<Mesh>, Ref<Material>, Transform> objects,
                           gecs::querier<DirectionalLight> directional_lights,
                           gecs::resource<RendererResource> render,
                           gecs::resource<class UIResource> ui,
                           gecs::resource<class Camera> camera,
                           gecs::resource<gecs::mut<class AssetManager>> asset_manager);

    public:
        Ref<Renderer> renderer;
    };
} // namespace Yutrel