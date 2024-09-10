#include "resource_manager.h"

#include "Function/Render/context.h"

namespace Yutrel
{
    ResourceManager::ResourceManager(std::shared_ptr<class Context> context)
        : m_context(context)
    {
        init();
    }

    ResourceManager::~ResourceManager()
    {
        destroy();
    }

    void ResourceManager::init()
    {
        // vma
        auto allocator_ci =
            vma::AllocatorCreateInfo()
                .setPhysicalDevice(m_context->getGPU())
                .setDevice(m_context->getDevice())
                .setInstance(m_context->getInstance());

        m_allocator = vma::createAllocator(allocator_ci);
    }

    void ResourceManager::destroy()
    {
        m_allocator.destroy();
    }

} // namespace Yutrel