#pragma once

#include "core/macro.hpp"

namespace Yutrel
{
    class RHI;

    struct RenderPipelineInitInfo
    {
    };

    class RenderPipeline
    {
    public:
        virtual void Init(RenderPipelineInitInfo info) = 0;

        virtual void Clear() = 0;

        void SetRHI(Ref<RHI> rhi) { m_rhi = rhi; }

    protected:
        Ref<RHI> m_rhi;
    };

} // namespace Yutrel