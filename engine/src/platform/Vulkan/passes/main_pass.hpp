#pragma once

#include "function/render/render_pass.hpp"

namespace Yutrel
{
    struct MainPassInitInfo : RenderPassInitInfo
    {};

    class MainPass : public RenderPass
    {
    public:
        virtual void Init(RenderPassInitInfo* info) override;

    private:
    };
} // namespace Yutrel