#pragma once

namespace Yutrel
{
    struct ContextCreateInfo
    {
    };

    class Context
    {
    public:
        Context() = default;

        Context(Context const&)            = delete;
        Context& operator=(Context const&) = delete;

        ~Context();

        bool Init(const ContextCreateInfo& info);

    private:
        void Destroy();
    };
} // namespace Yutrel