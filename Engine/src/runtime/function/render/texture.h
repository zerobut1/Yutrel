#pragma once

#include <memory>
#include <string>
#include <vector>

namespace Yutrel
{
    // 纹理类
    class Texture
    {
    public:
        virtual ~Texture() = default;

        virtual uint32_t getWidth() const  = 0;
        virtual uint32_t getHeight() const = 0;

        virtual void setData(void *data, uint32_t size) = 0;

        virtual void Bind(uint32_t slot = 0) const = 0;
    };

    // 2D纹理
    class Texture2D : public Texture
    {
    public:
        static std::shared_ptr<Texture2D> create(uint32_t width, uint32_t height);
        static std::shared_ptr<Texture2D> create(const std::string &path);
    };

    // 立方体贴图
    class TextureCubemaps : public Texture
    {
    public:
        static std::shared_ptr<TextureCubemaps> create(const std::vector<std::string> &paths);
    };
} // namespace Yutrel