#pragma once

#include "core/macro.hpp"
#include "resource/asset/asset.hpp"

#include <memory>
#include <stdint.h>
#include <string>
#include <vector>

namespace Yutrel
{
    typedef unsigned int GLenum;

    class Texture : public Asset
    {
    public:
        static Ref<Texture> Create(const std::string& img_path, bool flip = false, uint32_t levels = 0);
        static Ref<Texture> Create(const std::string& img_path, uint32_t resolution, uint32_t levels);
        static Ref<Texture> Create(GLenum target, uint32_t width, uint32_t height, uint32_t depth, GLenum i_format, uint32_t levels);

        static void Copy(const Ref<Texture> fr, uint32_t fr_level, const Ref<Texture> to, uint32_t to_level);

    public:
        virtual ~Texture() = default;

        virtual void Bind(uint32_t index) const   = 0;
        virtual void Unbind(uint32_t index) const = 0;

        virtual void SetData(const void* data) const = 0;

        virtual uint32_t GetRendererID() const   = 0;
        virtual uint32_t GetWidth() const        = 0;
        virtual uint32_t GetHeight() const       = 0;
        virtual uint32_t GetLevels() const       = 0;
        virtual GLenum GetInternalFormat() const = 0;

        virtual void BindILS(uint32_t level, uint32_t index, GLenum access) const = 0;
        virtual void UnbindILS(uint32_t index) const                              = 0;

        virtual void GenerateMipmap() const           = 0;
        virtual void Clear(uint32_t level) const      = 0;
        virtual void Invalidate(uint32_t level) const = 0;
    };

    // 读取图片的类
    class Image
    {
    public:
        Image(const std::string& filepath, bool flip = false, uint32_t channels = 0);

        // 无拷贝构造函数
        Image(const Image&)                      = delete;
        Image& operator=(const Image&)           = delete;
        Image(Image&& other) noexcept            = default;
        Image& operator=(Image&& other) noexcept = default;

        bool IsHDR() const;
        uint32_t GetWidth() const;
        uint32_t GetHeight() const;
        GLenum GetFormat() const;
        GLenum GetInternalFormat() const;

        template <typename T>
        const T* GetPixels() const;

    private:
        int m_width, m_height, m_channels;
        bool m_is_hdr;

        struct deleter
        {
            void operator()(uint8_t* buffer);
        };

        std::unique_ptr<uint8_t, deleter> m_pixels;
    };

} // namespace Yutrel