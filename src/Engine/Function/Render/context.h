#pragma once

#include <vulkan/vulkan.hpp>

namespace Yutrel
{
    struct ContextCreateInfo
    {
        bool enable_validation_layers{true};
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

    private:
        vk::Instance m_instance;
        vk::DebugUtilsMessengerEXT m_debug_messenger;
    };
} // namespace Yutrel