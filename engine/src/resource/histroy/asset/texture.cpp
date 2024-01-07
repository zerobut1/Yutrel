// #include "yutrel_pch.hpp"

// #include "texture.hpp"

// #include "platform/OpenGL/opengl_texture.hpp"

// #define STB_IMAGE_IMPLEMENTATION
// #include <stb_image.h>

// #include <glm/glm.hpp>
// #include <stdint.h>

// namespace Yutrel
// {
//     //////////////////////////////////////////////
//     // texture
//     //////////////////////////////////////////////
//     Ref<Texture> Texture::Create(const std::string& img_path, bool flip, uint32_t levels)
//     {
//         return CreateRef<OpenGLTexture>(img_path, flip, levels);
//     }

//     Ref<Texture> Texture::Create(const std::string& img_path, uint32_t resolution, uint32_t levels)
//     {
//         return CreateRef<OpenGLTexture>(img_path, (uint32_t)resolution, levels);
//     }

//     Ref<Texture> Texture::Create(GLenum target, uint32_t width, uint32_t height, uint32_t depth, GLenum i_format, uint32_t levels)
//     {
//         return CreateRef<OpenGLTexture>(target, width, height, depth, i_format, levels);
//     }

//     void Texture::Copy(const Ref<Texture> fr, uint32_t fr_level, const Ref<Texture> to, uint32_t to_level)
//     {
//         OpenGLTexture::Copy(std::static_pointer_cast<OpenGLTexture>(fr), fr_level, std::static_pointer_cast<OpenGLTexture>(to), to_level);
//     }

//     //////////////////////////////////////////////
//     // Image
//     //////////////////////////////////////////////

//     void Image::deleter::operator()(uint8_t* buffer)
//     {
//         if (buffer != nullptr)
//         {
//             stbi_image_free(buffer);
//         }
//     }

//     Image::Image(const std::string& filepath, bool flip, GLuint channels)
//         : m_width(0), m_height(0), m_channels(0)
//     {
//         stbi_set_flip_vertically_on_load(flip);

//         // 目前支持的文件拓展名
//         const std::vector<std::string> extensions{".jpg", ".png", ".jpeg", ".bmp", ".hdr", ".exr"};

//         // 检查当前文件是否支持
//         auto ext = filepath.substr(filepath.rfind("."));
//         if (std::find(extensions.begin(), extensions.end(), ext) == extensions.end())
//         {
//             LOG_ERROR("Image file format is not supported: {0}", filepath);
//             return;
//         }

//         LOG_INFO("Loading image from: {0}", filepath);

//         // 如果是hdr则读取为float
//         this->m_is_hdr = stbi_is_hdr(filepath.c_str());
//         if (m_is_hdr)
//         {
//             float* buffer = stbi_loadf(filepath.c_str(), &m_width, &m_height, &m_channels, 4);

//             if (buffer == nullptr)
//             {
//                 LOG_ERROR("Failed to load image: {0}", filepath);
//                 LOG_ERROR("STBI failure reason: {0}", stbi_failure_reason());
//                 return;
//             }

//             // 报告hdr图像的一些信息
//             size_t n_pixels         = m_width * m_height;
//             float min_luminance     = std::numeric_limits<float>::max();
//             float max_luminance     = std::numeric_limits<float>::min();
//             float sum_log_luminance = 0.0f;

//             for (size_t i = 0; i < n_pixels; i++)
//             {
//                 const float* pixel_ptr = buffer + (i * 3); // move forward 3 channels

//                 auto color     = glm::vec3(pixel_ptr[0], pixel_ptr[1], pixel_ptr[2]);
//                 auto luminance = glm::dot(color, glm::vec3(0.2126f, 0.7152f, 0.0722f));

//                 min_luminance = std::min(min_luminance, luminance);
//                 max_luminance = std::max(max_luminance, luminance);
//                 // 避免除0
//                 sum_log_luminance += std::max(log(luminance + 0.00001f), 0.0f);
//             }

//             float log_average_luminance = exp(sum_log_luminance / n_pixels);

//             LOG_INFO("HDR image luminance report:");
//             LOG_TRACE("------------------------------------------------------------------------");
//             LOG_TRACE("min: {0}, max: {1}, log average: {2}", min_luminance, max_luminance, log_average_luminance);
//             LOG_TRACE("------------------------------------------------------------------------");

//             float luminance_diff = max_luminance - min_luminance;
//             if (luminance_diff > 10000.0f)
//             {
//                 LOG_WARN("Input HDR image is too bright, some pixels have values close to infinity!");
//                 LOG_WARN("This can lead to serious artifact in IBL or even completely white images!");
//                 LOG_WARN("Please use a different image or manually adjust the exposure values (EV)!");
//             }

//             m_pixels.reset(reinterpret_cast<uint8_t*>(buffer));
//         }
//         // 读取非hdr图像
//         else
//         {
//             YUTREL_ASSERT(channels <= 4, "You can only force read up to 4 channels!");
//             uint8_t* buffer = stbi_load(filepath.c_str(), &m_width, &m_height, &m_channels, channels);

//             if (buffer == nullptr)
//             {
//                 LOG_ERROR("Failed to load image: {0}", filepath);
//                 LOG_ERROR("STBI failure reason: {0}", stbi_failure_reason());
//                 return;
//             }

//             m_pixels.reset(buffer);
//         }

//         YUTREL_ASSERT(m_channels <= 4, "Unexpected image format with {0} channels...", m_channels);

//         // 读取失败则异常
//         if (m_pixels == nullptr)
//         {
//             YUTREL_ASSERT(false, "Unable to claim image data from: {0}", filepath)
//         }
//     }

//     bool Image::IsHDR() const
//     {
//         return m_is_hdr;
//     }

//     uint32_t Image::GetWidth() const
//     {
//         return static_cast<uint32_t>(m_width);
//     }

//     uint32_t Image::GetHeight() const
//     {
//         return static_cast<uint32_t>(m_height);
//     }

//     GLenum Image::GetFormat() const
//     {
//         if (m_is_hdr)
//         {
//             return GL_RGBA;
//         }

//         switch (m_channels)
//         {
//         case 1:
//             return GL_RED; // greyscale
//         case 2:
//             return GL_RG; // greyscale + alpha
//         case 3:
//             return GL_RGB;
//         case 4:
//             return GL_RGBA;
//         default:
//             return 0;
//         }
//     }

//     GLenum Image::GetInternalFormat() const
//     {
//         if (m_is_hdr)
//         {
//             return GL_RGBA16F;
//         }

//         switch (m_channels)
//         {
//         case 1:
//             return GL_R8; // greyscale
//         case 2:
//             return GL_RG8; // greyscale + alpha
//         case 3:
//             return GL_RGB8;
//         case 4:
//             return GL_RGBA8;
//         default:
//             return 0;
//         }
//     }

//     template <typename T>
//     const T* Image::GetPixels() const
//     {
//         return reinterpret_cast<const T*>(m_pixels.get());
//     }

//     // explicit template function instantiation
//     template const uint8_t* Image::GetPixels<uint8_t>() const;
//     template const float* Image::GetPixels<float>() const;

// } // namespace Yutrel