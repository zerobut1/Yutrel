#include "context.hpp"

namespace Yutrel
{
    Context::~Context()
    {
        Destroy();
    }

    bool Context::Init(const ContextCreateInfo& info)
    {
        return true;
    }

    void Context::Destroy()
    {
    }
} // namespace Yutrel