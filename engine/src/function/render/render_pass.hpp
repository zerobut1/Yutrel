#pragma once

#include "core/macro.hpp"

namespace Yutrel
{
    class RHI;

    struct RenderPassInitInfo
    {};

    class RenderPass
    {
    public:
        virtual void Init(RenderPassInitInfo* info) = 0;

        void SetRHI(Ref<RHI> rhi) { m_rhi = rhi; }

    protected:
        Ref<RHI> m_rhi;
    };
} // namespace Yutrel