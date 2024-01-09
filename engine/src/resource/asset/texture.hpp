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
        std::string path;
        bool is_loaded{false};

        Ref<Image> image;

        Texture() = default;
        Texture(const std::string& path)
            : path(path) {}

        void ReleaseImage()
        {
            stbi_image_free(image->pixels);
            image.reset();
        }
    };
} // namespace Yutrel