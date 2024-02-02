#pragma once

#include "function/render/renderer.hpp"
#include "resource/component/light.hpp"

namespace Yutrel
{
    struct VulkanRenderObject
    {
    };

    class RenderScene
    {
    public:
    public:
        // 光源
        DirectionalLight directional_light;

        // 所有的渲染对象
        std::vector<VulkanRenderObject> m_render_entities;
    };

} // namespace Yutrel