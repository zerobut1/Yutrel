#pragma once

#include <stb_image.h>

#include <string>

namespace Yutrel
{
    struct Image
    {
        uint32_t width;
        uint32_t height;
        uint32_t channels;
        void* pixels;
    };

    struct Texture
    {
    public:
        Texture() = default;
        Texture(const std::string& path)
            : path(path) {}

        void ReleaseImage()
        {
            stbi_image_free(image->pixels);
            image.reset();
        }

    public:
        std::string path;
        bool is_loaded{false};

        Ref<Image> image;
    };
} // namespace Yutrel