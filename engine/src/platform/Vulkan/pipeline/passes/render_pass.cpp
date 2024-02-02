#include "yutrel_pch.hpp"

#include "render_pass.hpp"

namespace Yutrel
{
    void RenderPass::Init(RenderPassInitInfo* info)
    {
        m_rhi           = info->rhi;
        m_asset_manager = info->asset_manager;
        m_render_scene  = info->render_scene;
    }

} // namespace Yutrel