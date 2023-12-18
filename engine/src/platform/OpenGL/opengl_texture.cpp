
// #include "yutrel_pch.hpp"

// #include "opengl_texture.hpp"

// #include "core/application/application.hpp"
// #include "core/path/path.hpp"
// #include "function/render/renderer.hpp"
// #include "resource/asset/framebuffer.hpp"
// #include "resource/asset/shader.hpp"
// #include "resource/component/model/model.hpp"

// #include <filesystem>
// #include <glm/ext/matrix_clip_space.hpp>
// #include <glm/ext/matrix_transform.hpp>
// #include <memory>
// #include <stb_image.h>
// #include <stdint.h>

// namespace Yutrel
// {
//     // 避免重复绑定
//     constexpr int n_texture_units = 32;

//     // 拷贝纹理
//     void OpenGLTexture::Copy(const Ref<OpenGLTexture> fr, uint32_t fr_level, const Ref<OpenGLTexture> to, uint32_t to_level)
//     {
//         YUTREL_ASSERT(fr_level < fr->m_levels, "Mipmap level {0} is not valid in texture {1}!", fr_level, fr->m_id);
//         YUTREL_ASSERT(to_level < to->m_levels, "Mipmap level {0} is not valid in texture {1}!", to_level, to->m_id);

//         uint32_t fr_scale = static_cast<uint32_t>(std::pow(2, fr_level));
//         uint32_t to_scale = static_cast<uint32_t>(std::pow(2, to_level));

//         uint32_t fw = fr->m_width / fr_scale;
//         uint32_t fh = fr->m_height / fr_scale;
//         uint32_t fd = fr->m_depth;

//         uint32_t tw = to->m_width / to_scale;
//         uint32_t th = to->m_height / to_scale;
//         uint32_t td = to->m_depth;

//         if (fw != tw || fh != th || fd != td)
//         {
//             LOG_ERROR("Unable to copy image data, mismatch width, height or depth!");
//             return;
//         }

//         if (fr->m_target != to->m_target)
//         {
//             LOG_ERROR("Unable to copy image data, incompatible targets!");
//             return;
//         }

//         glCopyImageSubData(fr->m_id, fr->m_target, fr_level, 0, 0, 0, to->m_id, to->m_target, to_level, 0, 0, 0, fw, fh, fd);
//     }

//     // 读取图片创建纹理
//     OpenGLTexture::OpenGLTexture(const std::string& img_path, bool flip, uint32_t levels)
//         : m_target(GL_TEXTURE_2D), m_depth(1), m_levels(levels)
//     {
//         auto image = Image(img_path, flip);

//         this->m_width           = image.GetWidth();
//         this->m_height          = image.GetHeight();
//         this->m_format          = image.GetFormat();
//         this->m_internal_format = image.GetInternalFormat();

//         // level为0 则计算需要的mipmap级数
//         if (levels == 0)
//         {
//             m_levels = 1 + static_cast<uint32_t>(floor(std::log2(std::max(m_width, m_width))));
//         }

//         glCreateTextures(GL_TEXTURE_2D, 1, &m_id);
//         glTextureStorage2D(m_id, m_levels, m_internal_format, m_width, m_height);

//         if (image.IsHDR())
//         {
//             glTextureSubImage2D(m_id, 0, 0, 0, m_width, m_height, m_format, GL_FLOAT, image.GetPixels<float>());
//         }
//         else
//         {
//             glTextureSubImage2D(m_id, 0, 0, 0, m_width, m_height, m_format, GL_UNSIGNED_BYTE, image.GetPixels<uint8_t>());
//         }

//         if (m_levels > 1)
//         {
//             glGenerateTextureMipmap(m_id);
//         }

//         SetSampleState();
//     }

//     // 读取图片创建cubemap
//     OpenGLTexture::OpenGLTexture(const std::string& img_path, uint32_t resolution, uint32_t levels)
//         : m_target(GL_TEXTURE_CUBE_MAP), m_width(resolution), m_height(resolution), m_depth(6), m_levels(levels)
//     {
//         // resolution必须是2的幂
//         if (resolution == 0 || (resolution & (resolution - 1)) != 0)
//         {
//             LOG_ERROR("Attempting to build a cubemap whose resolution is not a power of 2");
//             return;
//         }

//         // cubemap应当从hdr图创建
//         if (auto path = std::filesystem::path(img_path); path.extension() != ".hdr" && path.extension() != ".exr")
//         {
//             LOG_WARN("Attempting to build a cubemap from a non-HDR image");
//             LOG_WARN("Visual quality might drop seriously after tone mapping");
//         }

//         this->m_format          = GL_RGBA;
//         this->m_internal_format = GL_RGBA16F;

//         if (levels == 0)
//         {
//             m_levels = 1 + static_cast<uint32_t>(floor(std::log2(std::max(m_width, m_height))));
//         }

//         // 等距柱状投影图加载到一个临时的2D纹理中
//         GLuint equirectangle = 0;
//         glCreateTextures(GL_TEXTURE_2D, 1, &equirectangle);

//         if (equirectangle > 0)
//         {
//             auto image = Image(img_path, false, 3);

//             GLuint im_w    = image.GetWidth();
//             GLuint im_h    = image.GetHeight();
//             GLuint im_fmt  = image.GetFormat();
//             GLenum im_ifmt = image.GetInternalFormat();

//             glTextureParameteri(equirectangle, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
//             glTextureParameteri(equirectangle, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
//             glTextureParameteri(equirectangle, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//             glTextureParameteri(equirectangle, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

//             if (image.IsHDR())
//             {
//                 glTextureStorage2D(equirectangle, 1, im_ifmt, im_w, im_h);
//                 glTextureSubImage2D(equirectangle, 0, 0, 0, im_w, im_h, im_fmt, GL_FLOAT, image.GetPixels<float>());
//             }
//             else
//             {
//                 glTextureStorage2D(equirectangle, 1, im_ifmt, im_w, im_h);
//                 glTextureSubImage2D(equirectangle, 0, 0, 0, im_w, im_h, im_fmt, GL_UNSIGNED_BYTE, image.GetPixels<uint8_t>());
//             }
//         }

//         // 创建cubemap
//         glCreateTextures(GL_TEXTURE_CUBE_MAP, 1, &m_id);
//         glTextureStorage2D(m_id, m_levels, m_internal_format, m_width, m_height);

//         // 投影到六个面上
//         LOG_INFO("Creating cubemap from {0}", img_path);
//         // todo compute shader
//         auto resource       = Resources(Application::GetInstance().GetWorld());
//         auto paths          = resource.Get<Paths>();
//         auto convert_shader = ComputeShader::Create(paths.asset + "shader\\core\\equirect_2_cube.glsl");

//         if (convert_shader->Bind(); true)
//         {
//             glBindTextureUnit(0, equirectangle);
//             glBindImageTexture(0, m_id, 0, GL_TRUE, 0, GL_WRITE_ONLY, m_internal_format);
//             glDispatchCompute(resolution / 32, resolution / 32, 6); // six faces
//             glMemoryBarrier(GL_ALL_BARRIER_BITS);                   // sync wait
//             glBindTextureUnit(0, 0);
//             glBindImageTexture(0, 0, 0, GL_TRUE, 0, GL_WRITE_ONLY, m_internal_format);
//             convert_shader->Unbind();
//         }

//         glDeleteTextures(1, &equirectangle); // delete the temporary 2D equirectangle texture

//         if (m_levels > 1)
//         {
//             glGenerateTextureMipmap(m_id);
//         }

//         SetSampleState();
//     }

//     // 创建空纹理
//     OpenGLTexture::OpenGLTexture(GLenum target, uint32_t width, uint32_t height, uint32_t depth, GLenum i_format, uint32_t levels)
//         : m_target(target), m_width(width), m_height(height), m_depth(depth),
//           m_levels(levels), m_format(0), m_internal_format(i_format)
//     {
//         if (levels == 0)
//         {
//             m_levels = 1 + static_cast<GLuint>(floor(std::log2(std::max(width, height))));
//         }

//         // TODO: deduce format from i_format

//         glCreateTextures(target, 1, &m_id);

//         switch (target)
//         {
//         case GL_TEXTURE_2D:
//         case GL_TEXTURE_CUBE_MAP:
//         { // depth must = 6
//             glTextureStorage2D(m_id, m_levels, i_format, width, height);
//             break;
//         }
//         case GL_TEXTURE_2D_MULTISAMPLE:
//         {
//             glTextureStorage2DMultisample(m_id, 4, i_format, width, height, GL_TRUE);
//             break;
//         }
//         case GL_TEXTURE_2D_ARRAY:
//         case GL_TEXTURE_CUBE_MAP_ARRAY:
//         { // depth must = 6 * n_layers
//             glTextureStorage3D(m_id, m_levels, i_format, width, height, depth);
//             break;
//         }
//         case GL_TEXTURE_2D_MULTISAMPLE_ARRAY:
//         {
//             glTextureStorage3DMultisample(m_id, 4, i_format, width, height, depth, GL_TRUE);
//             break;
//         }
//         default:
//         {
//             YUTREL_ASSERT(false, "Unsupported texture target");
//         }
//         }

//         SetSampleState();
//     }

//     void OpenGLTexture::SetData(const void* data) const
//     {
//         // todo 设置不同类型
//         // 目前只是float
//         glTextureSubImage2D(m_id, 0, 0, 0, m_width, m_height, m_format, GL_FLOAT, data);
//     }

//     OpenGLTexture::~OpenGLTexture()
//     {
//         if (m_id == 0)
//             return;

//         glDeleteTextures(1, &m_id);
//     }

//     void OpenGLTexture::Bind(GLuint index) const
//     {
//         glBindTextureUnit(index, m_id);
//     }

//     void OpenGLTexture::Unbind(GLuint index) const
//     {

//         glBindTextureUnit(index, 0);
//     }

//     void OpenGLTexture::BindILS(GLuint level, GLuint index, GLenum access) const
//     {
//         YUTREL_ASSERT(level < m_levels, "Mipmap level {0} is not valid in the texture...", level);
//         glBindImageTexture(index, m_id, level, GL_TRUE, 0, access, m_internal_format);
//     }

//     void OpenGLTexture::UnbindILS(GLuint index) const
//     {
//         glBindImageTexture(index, 0, 0, GL_TRUE, 0, GL_READ_ONLY, m_internal_format);
//     }

//     void OpenGLTexture::GenerateMipmap() const
//     {
//         YUTREL_ASSERT(m_levels > 1, "Unable to generate mipmaps, levels must be greater than 1...");
//         glGenerateTextureMipmap(m_id);
//     }

//     void OpenGLTexture::Clear(GLuint level) const
//     {
//         switch (m_internal_format)
//         {
//         case GL_RG16F:
//         case GL_RGB16F:
//         case GL_RGBA16F:
//         case GL_RG32F:
//         case GL_RGB32F:
//         case GL_RGBA32F:
//         {
//             glClearTexSubImage(m_id, level, 0, 0, 0, m_width, m_height, m_depth, m_format, GL_FLOAT, NULL);
//             break;
//         }
//         default:
//         {
//             glClearTexSubImage(m_id, level, 0, 0, 0, m_width, m_height, m_depth, m_format, GL_UNSIGNED_BYTE, NULL);
//             return;
//         }
//         }
//     }

//     void OpenGLTexture::Invalidate(GLuint level) const
//     {
//         glInvalidateTexSubImage(m_id, level, 0, 0, 0, m_width, m_height, m_depth);
//     }

//     // 设置纹理的采样情况
//     void OpenGLTexture::SetSampleState() const
//     {
//         GLint mag_filter = GL_LINEAR;
//         GLint min_filter = m_levels > 1 ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR;

//         // anisotropic filtering requires OpenGL 4.6, where maximum anisotropy is implementation-defined
//         static GLfloat anisotropy = -1.0f;
//         if (anisotropy < 0)
//         {
//             glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY, &anisotropy);
//             anisotropy = std::clamp(anisotropy, 1.0f, 8.0f); // limit anisotropy to 8
//         }

//         switch (m_target)
//         {
//         case GL_TEXTURE_2D:
//         case GL_TEXTURE_2D_ARRAY:
//         {
//             if (m_internal_format == GL_RG16F)
//             { // 2D BRDF LUT, inverse LUT, fake BRDF maps, etc
//                 glTextureParameteri(m_id, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
//                 glTextureParameteri(m_id, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
//             }
//             else if (m_internal_format == GL_RGB16F)
//             { // 3D BRDF LUT, cloth DFG LUT, etc
//                 glTextureParameteri(m_id, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
//                 glTextureParameteri(m_id, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
//                 glTextureParameteri(m_id, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
//             }
//             else if (m_internal_format == GL_RGBA16F)
//             { // 3D BRDF DFG LUT used as ILS (uniform image2D)
//                 glTextureParameteri(m_id, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
//                 glTextureParameteri(m_id, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
//                 glTextureParameteri(m_id, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
//             }
//             else if (m_internal_format == GL_DEPTH_COMPONENT)
//             { // depth texture and shadow maps
//                 glTextureParameteri(m_id, GL_TEXTURE_WRAP_S, GL_REPEAT);
//                 glTextureParameteri(m_id, GL_TEXTURE_WRAP_T, GL_REPEAT);
//                 glTextureParameteri(m_id, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
//                 glTextureParameteri(m_id, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
//             }
//             else if (m_internal_format == GL_DEPTH_COMPONENT32F)
//             {
//                 glTextureParameteri(m_id, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
//                 glTextureParameteri(m_id, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
//                 glTextureParameteri(m_id, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
//                 glTextureParameteri(m_id, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

//                 constexpr float borderColor[] = {1.0f, 1.0f, 1.0f, 1.0f};
//                 glTextureParameterfv(m_id, GL_TEXTURE_BORDER_COLOR, borderColor);
//             }
//             else
//             {
//                 // the rest of 2D textures are mostly normal and seamless so just repeat, but be aware
//                 // that some of those with a GL_RGBA format are intended for alpha blending so must be
//                 // clamped to edge instead. However, checking `format == GL_RGBA` and alpha < 1 is not
//                 // enough to conclude, it all depends, in that case we need to set wrap mode manually
//                 glTextureParameteri(m_id, GL_TEXTURE_WRAP_S, GL_REPEAT);
//                 glTextureParameteri(m_id, GL_TEXTURE_WRAP_T, GL_REPEAT);
//                 glTextureParameteri(m_id, GL_TEXTURE_MIN_FILTER, min_filter);
//                 glTextureParameteri(m_id, GL_TEXTURE_MAG_FILTER, mag_filter);
//                 glTextureParameterf(m_id, GL_TEXTURE_MAX_ANISOTROPY, anisotropy);
//             }
//             break;
//         }
//         case GL_TEXTURE_2D_MULTISAMPLE:
//         case GL_TEXTURE_2D_MULTISAMPLE_ARRAY:
//         {
//             // multisampled textures are not filtered at all, there's nothing we need to do here because
//             // we'll never sample them, the hardware takes care of all the multisample operations for us
//             // in fact, trying to set any of the sampler states will cause a `GL_INVALID_ENUM` error.
//             return;
//         }
//         case GL_TEXTURE_CUBE_MAP: // skybox and IBL maps
//         case GL_TEXTURE_CUBE_MAP_ARRAY:
//         {
//             glTextureParameteri(m_id, GL_TEXTURE_MIN_FILTER, min_filter);
//             glTextureParameteri(m_id, GL_TEXTURE_MAG_FILTER, mag_filter);
//             glTextureParameteri(m_id, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
//             glTextureParameteri(m_id, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
//             glTextureParameteri(m_id, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_BORDER);
//             const float border[] = {0.0f, 0.0f, 0.0f, 1.0f};
//             glTextureParameterfv(m_id, GL_TEXTURE_BORDER_COLOR, border);
//             break;
//         }
//         default:
//         {
//             YUTREL_ASSERT(false, "Unsupported texture target");
//         }
//         }
//     }

// } // namespace Yutrel