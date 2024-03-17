#pragma once

#include <stb_image.h>

#include <string>

namespace Yutrel
{
    struct Image
    {
        int width;
        int height;
        int channels;
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

        bool is_HDR{false};

        Ref<Image> image;
    };
} // namespace Yutrel