#include "yutrel_pch.hpp"

#include "vulkan_asset.hpp"

#include "platform/Vulkan/vulkan_renderer.hpp"
#include <stdint.h>
#include <vk_mem_alloc_enums.hpp>
#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_enums.hpp>
#include <vulkan/vulkan_handles.hpp>
#include <vulkan/vulkan_structs.hpp>

namespace Yutrel
{
    void VulkanAssetManager::Init(AssetManagerInitInfo info)
    {
        m_rhi = info.rhi;

        InitDefaultData();
    }

    void VulkanAssetManager::InitDefaultData()
    {
        // 默认纹理
        uint32_t white   = 0xFFFFFFFF;
        uint32_t black   = 0xFF000000;
        uint32_t magenta = 0xFFFF00FF;
        std::array<uint32_t, 16 * 16> pixels;
        for (int x = 0; x < 16; x++)
        {
            for (int y = 0; y < 16; y++)
            {
                pixels[y * 16 + x] = ((x % 2) ^ (y % 2)) ? magenta : black;
            }
        }

        m_default_data.white_texture = m_rhi->CreateImage(vk::Extent3D{1, 1, 1}, vk::Format::eR8G8B8A8Unorm, vk::ImageUsageFlagBits::eSampled | vk::ImageUsageFlagBits::eTransferDst);
        m_default_data.error_texture = m_rhi->CreateImage(vk::Extent3D{16, 16, 1}, vk::Format::eR8G8B8A8Unorm, vk::ImageUsageFlagBits::eSampled | vk::ImageUsageFlagBits::eTransferDst);

        m_rhi->UploadImageData((void*)&white, m_default_data.white_texture);
        m_rhi->UploadImageData(pixels.data(), m_default_data.error_texture);

        // 默认采样器
        auto sampler_ci =
            vk::SamplerCreateInfo()
                .setMagFilter(vk::Filter::eNearest)
                .setMinFilter(vk::Filter::eNearest)
                .setAddressModeU(vk::SamplerAddressMode::eRepeat)
                .setAddressModeV(vk::SamplerAddressMode::eRepeat)
                .setAddressModeW(vk::SamplerAddressMode::eRepeat)
                .setAnisotropyEnable(vk::True)
                .setMaxAnisotropy(m_rhi->GetPhysicalDeviceProperties().limits.maxSamplerAnisotropy)
                .setBorderColor(vk::BorderColor::eIntOpaqueBlack)
                .setUnnormalizedCoordinates(vk::False)
                .setCompareEnable(vk::False)
                .setCompareOp(vk::CompareOp::eAlways)
                .setMipmapMode(vk::SamplerMipmapMode::eNearest)
                .setMipLodBias(0.0f)
                .setMinLod(0.0f)
                .setMaxLod(vk::LodClampNone);

        m_default_data.nearset_sampler = m_rhi->CreateSampler(sampler_ci);

        sampler_ci
            .setMagFilter(vk::Filter::eLinear)
            .setMinFilter(vk::Filter::eLinear)
            .setMipmapMode(vk::SamplerMipmapMode::eNearest);

        m_default_data.linear_sampler = m_rhi->CreateSampler(sampler_ci);
    }

    Ref<VulkanMesh> VulkanAssetManager::SetVulkanMesh(Ref<Mesh> mesh)
    {
        if (!m_meshes.count(mesh))
        {
            auto vulkan_mesh = UploadMesh(mesh);
            m_meshes.insert({mesh, vulkan_mesh});
        }
        return m_meshes[mesh];
    }

    Ref<VulkanMesh> VulkanAssetManager::UploadMesh(Ref<Mesh> mesh)
    {
        const size_t VERTEX_BUFFER_SIZE = mesh->vertices->size() * sizeof(Vertex);
        const size_t INDEX_BUFFER_SIZE  = mesh->indices->size() * sizeof(uint32_t);

        Ref<VulkanMesh> vulkan_mesh = CreateRef<VulkanMesh>();
        vulkan_mesh->index_count    = mesh->indices->size();

        // 顶点缓冲
        vulkan_mesh->vertex_buffer =
            m_rhi->CreateBuffer(VERTEX_BUFFER_SIZE,
                                vk::BufferUsageFlagBits::eStorageBuffer | vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eShaderDeviceAddress,
                                vma::MemoryUsage::eGpuOnly);

        auto device_address_info =
            vk::BufferDeviceAddressInfo()
                .setBuffer(vulkan_mesh->vertex_buffer.buffer);

        vulkan_mesh->vertex_buffer_address = m_rhi->GetBufferDeviceAddress(device_address_info);

        // 索引缓冲
        vulkan_mesh->index_buffer =
            m_rhi->CreateBuffer(INDEX_BUFFER_SIZE,
                                vk::BufferUsageFlagBits::eIndexBuffer | vk::BufferUsageFlagBits::eTransferDst,
                                vma::MemoryUsage::eGpuOnly);

        // 创建暂存缓冲区
        AllocatedBuffer staging_buffer =
            m_rhi->CreateBuffer(VERTEX_BUFFER_SIZE + INDEX_BUFFER_SIZE,
                                vk::BufferUsageFlagBits::eTransferSrc,
                                vma::MemoryUsage::eCpuOnly,
                                false);

        // 将数据拷贝到暂存缓冲区
        void* data = staging_buffer.info.pMappedData;

        memcpy(data, mesh->vertices->data(), VERTEX_BUFFER_SIZE);
        memcpy((char*)data + VERTEX_BUFFER_SIZE, mesh->indices->data(), INDEX_BUFFER_SIZE);

        // 从暂存缓冲区拷贝到gpu
        vk::CommandBuffer cmd_buffer = m_rhi->BeginSingleTimeCommands();

        auto vertex_copy =
            vk::BufferCopy()
                .setSrcOffset(0)
                .setDstOffset(0)
                .setSize(VERTEX_BUFFER_SIZE);

        cmd_buffer.copyBuffer(staging_buffer.buffer, vulkan_mesh->vertex_buffer.buffer, vertex_copy);

        auto index_copy =
            vk::BufferCopy()
                .setSrcOffset(VERTEX_BUFFER_SIZE)
                .setDstOffset(0)
                .setSize(INDEX_BUFFER_SIZE);

        cmd_buffer.copyBuffer(staging_buffer.buffer, vulkan_mesh->index_buffer.buffer, index_copy);

        m_rhi->EndSingleTimeCommands(cmd_buffer);

        // 将顶点和索引从内存释放
        mesh->ReleaseVertices();

        // 销毁暂存缓冲区
        m_rhi->DestroyBuffer(staging_buffer);

        return vulkan_mesh;
    }

    Ref<VulkanPBRMaterial> VulkanAssetManager::SetVulkanMaterial(Ref<Material> material)
    {
        if (!m_materials.count(material))
        {
            auto vulkan_material = CreateRef<VulkanPBRMaterial>();

            // 加载数据到uniform buffer
            vulkan_material->uniform_buffer = UploadMaterialData(material);

            // 加载纹理到GPU
            if (material->base_color_texture && !m_textures.count(material->base_color_texture))
            {
                m_textures.insert({material->base_color_texture, UploadTexture(material->base_color_texture)});
            }
            if (material->metallic_roughness_texture && !m_textures.count(material->metallic_roughness_texture))
            {
                m_textures.insert({material->metallic_roughness_texture, UploadTexture(material->metallic_roughness_texture)});
            }
            if (material->normal_texture && !m_textures.count(material->normal_texture))
            {
                m_textures.insert({material->normal_texture, UploadTexture(material->normal_texture)});
            }
            vulkan_material->base_color_texture         = m_textures[material->base_color_texture];
            vulkan_material->metallic_roughness_texture = m_textures[material->metallic_roughness_texture];
            vulkan_material->normal_texture             = m_textures[material->normal_texture];

            // 分配描述符集
            vulkan_material->descriptor_set = m_rhi->AllocateDescriptorSets(m_material_descriptor_set_layout);

            // 写描述符集
            DescriptorWriter writer;
            writer.WriteBuffer(0,
                               vulkan_material->uniform_buffer.buffer,
                               sizeof(MaterialUniformData),
                               0,
                               vk::DescriptorType::eUniformBuffer);
            writer.WriteImage(1,
                              vulkan_material->base_color_texture.image_view,
                              m_default_data.linear_sampler,
                              vk::ImageLayout::eShaderReadOnlyOptimal,
                              vk::DescriptorType::eCombinedImageSampler);
            writer.WriteImage(2,
                              material->metallic_roughness_texture ? vulkan_material->metallic_roughness_texture.image_view : m_default_data.white_texture.image_view,
                              m_default_data.linear_sampler,
                              vk::ImageLayout::eShaderReadOnlyOptimal,
                              vk::DescriptorType::eCombinedImageSampler);
            writer.WriteImage(3,
                              material->normal_texture ? vulkan_material->normal_texture.image_view : m_default_data.white_texture.image_view,
                              m_default_data.linear_sampler,
                              vk::ImageLayout::eShaderReadOnlyOptimal,
                              vk::DescriptorType::eCombinedImageSampler);

            m_rhi->UpdateDescriptorSets(writer, vulkan_material->descriptor_set);

            m_materials.insert({material, vulkan_material});
        }
        return m_materials[material];
    }

    AllocatedBuffer VulkanAssetManager::UploadMaterialData(Ref<Material> material)
    {
        const size_t UNIFORM_BUFFER_SIZE = sizeof(MaterialUniformData);

        // 数据
        MaterialUniformData uniform_data{};
        uniform_data.base_color_factor = material->base_color_factor;
        uniform_data.metallic_factor   = material->metallic_factor;
        uniform_data.roughness_factor  = material->roughness_factor;

        // uniform缓冲
        auto uniform_buffer =
            m_rhi->CreateBuffer(UNIFORM_BUFFER_SIZE,
                                vk::BufferUsageFlagBits::eUniformBuffer | vk::BufferUsageFlagBits::eTransferDst,
                                vma::MemoryUsage::eCpuToGpu);

        // 将数据拷贝到缓冲区
        memcpy(uniform_buffer.info.pMappedData, &uniform_data, UNIFORM_BUFFER_SIZE);

        return uniform_buffer;
    }

    AllocatedImage VulkanAssetManager::UploadTexture(Ref<Texture> texture)
    {
        Ref<Image> image = texture->image;

        vk::Extent3D extent{
            static_cast<uint32_t>(image->width),
            static_cast<uint32_t>(image->height),
            1,
        };

        // 创建gpu图片
        AllocatedImage new_image =
            m_rhi->CreateImage(extent,
                               vk::Format::eR8G8B8A8Unorm,
                               vk::ImageUsageFlagBits::eSampled |
                                   vk::ImageUsageFlagBits::eTransferDst |
                                   vk::ImageUsageFlagBits::eTransferSrc,
                               true);

        // 将数据加载到图片
        m_rhi->UploadImageData(texture->image->pixels, new_image);

        // 释放内存
        texture->ReleaseImage();

        return new_image;
    }

} // namespace Yutrel