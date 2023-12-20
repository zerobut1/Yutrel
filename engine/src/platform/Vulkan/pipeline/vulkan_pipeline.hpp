#pragma once

#include "core/macro.hpp"
#include "function/render/pipeline.hpp"

namespace Yutrel
{
    class RenderPass;

    class VulkanPipeline : public RenderPipeline
    {
    public:
        virtual void Init(RenderPipelineInitInfo info) override;

        virtual void Clear() override;

    private:
        Ref<RenderPass> m_main_pass;
    };
} // namespace Yutrel