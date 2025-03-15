// #include "resource_manager.h"

// #include "context.h"

// namespace Yutrel
// {
//     ResourceManager::ResourceManager(std::shared_ptr<class Context> context)
//         : m_context(context)
//     {
//         init();
//     }

//     ResourceManager::~ResourceManager()
//     {
//         destroy();
//     }

//     void ResourceManager::init()
//     {
//         // vma
//         // auto allocator_ci =
//         //     vma::AllocatorCreateInfo()
//         //         .setPhysicalDevice(m_context->getGPU())
//         //         .setDevice(m_context->getDevice())
//         //         .setInstance(m_context->getInstance());

//         // m_allocator = vma::createAllocator(allocator_ci);
//     }

//     void ResourceManager::destroy()
//     {
//         // m_allocator.destroy();
//     }

//     Image ResourceManager::createImage(const vk::ImageCreateInfo& info)
//     {
//         // Image res_image;
//         // res_image.image = m_context->getDevice().createImage(info);

//         // auto image_ai =
//         //     vma::AllocationCreateInfo()
//         //         .setUsage(vma::MemoryUsage::eGpuOnly)
//         //         .setRequiredFlags(vk::MemoryPropertyFlagBits::eDeviceLocal);

//         // std::tie(res_image.image, res_image.allocation) = m_allocator.createImage(info, image_ai);

//         // m_main_deletion_queue.PushFunction(
//         //     [=]()
//         //     {
//         //         m_allocator.destroyImage(res_image.image, res_image.allocation);
//         //     });

//         // return res_image;
//     }

//     vk::ImageView ResourceManager::createImageView(const vk::ImageViewCreateInfo& info)
//     {
//         // auto device = m_context->getDevice();

//         // auto res_image_view = device.createImageView(info);

//         // m_main_deletion_queue.PushFunction(
//         //     [=]()
//         //     {
//         //         device.destroyImageView(res_image_view);
//         //     });

//         // return res_image_view;
//     }

//     vk::Sampler ResourceManager::createSampler(const vk::SamplerCreateInfo& info)
//     {
//         // auto device = m_context->getDevice();

//         // auto res_sampler = device.createSampler(info);

//         // m_main_deletion_queue.PushFunction(
//         //     [=]()
//         //     {
//         //         device.destroy(res_sampler);
//         //     });

//         // return res_sampler;
//     }

// } // namespace Yutrel