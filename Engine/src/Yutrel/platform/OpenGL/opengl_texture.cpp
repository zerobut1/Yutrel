
#include "yutrel_pch.hpp"

#include "opengl_texture.h"

#include "Yutrel/function/global/global_context.h"
#include "Yutrel/function/render/framebuffer.h"
#include "Yutrel/function/render/model.h"
#include "Yutrel/function/render/shader.h"
#include "Yutrel/core/log/log.hpp"


#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <memory>
#include <stb_image.h>

namespace Yutrel
{
    OpenGLTexture2D::OpenGLTexture2D(uint32_t width, uint32_t height)
        : m_width(width), m_height(height)
    {
        m_internal_format = GL_RGB8;
        m_data_format     = GL_RGB;

        glCreateTextures(GL_TEXTURE_2D, 1, &m_rendererID);
        glTextureStorage2D(m_rendererID, 1, m_internal_format, m_width, m_height);

        glTextureParameteri(m_rendererID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTextureParameteri(m_rendererID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTextureParameteri(m_rendererID, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTextureParameteri(m_rendererID, GL_TEXTURE_WRAP_T, GL_REPEAT);
    }

    OpenGLTexture2D::OpenGLTexture2D(const std::string &path)
        : m_path(path)
    {
        int width, height, channels;
        stbi_set_flip_vertically_on_load(true);
        stbi_uc *data = nullptr;
        {
            data = stbi_load(path.c_str(), &width, &height, &channels, 0);
        }
        // ENGINE_CORE_ASSERT(data, "Failed to load image!");
        m_width  = width;
        m_height = height;

        // 确认是RGB还是RGBA格式
        GLenum internalFormat = 0, dataFormat = 0;
        if (channels == 4)
        {
            internalFormat = GL_RGBA8;
            dataFormat     = GL_RGBA;
        }
        else if (channels == 3)
        {
            internalFormat = GL_RGB8;
            dataFormat     = GL_RGB;
        }
        m_internal_format = internalFormat;
        m_data_format     = dataFormat;

        glCreateTextures(GL_TEXTURE_2D, 1, &m_rendererID);
        glTextureStorage2D(m_rendererID, 1, internalFormat, m_width, m_height);

        glTextureSubImage2D(m_rendererID, 0, 0, 0, m_width, m_height, dataFormat, GL_UNSIGNED_BYTE, data);

        glTextureParameteri(m_rendererID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTextureParameteri(m_rendererID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTextureParameteri(m_rendererID, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTextureParameteri(m_rendererID, GL_TEXTURE_WRAP_T, GL_REPEAT);

        stbi_image_free(data);
    }

    OpenGLTexture2D::~OpenGLTexture2D()
    {
        glDeleteTextures(1, &m_rendererID);
    }

    void OpenGLTexture2D::setData(void *data, uint32_t size)
    {
        uint32_t bpp = m_data_format == GL_RGBA ? 4 : 3;
        // ENGINE_CORE_ASSERT(size == m_Width * m_Height * bpp, "Data must be entire texture!");

        glTextureSubImage2D(m_rendererID, 0, 0, 0, m_width, m_height, m_data_format, GL_UNSIGNED_BYTE, data);
    }

    void OpenGLTexture2D::Bind(uint32_t slot) const
    {
        glBindTextureUnit(slot, m_rendererID);
    }

    /**
     *这里的实现是比较早的opengl版本的实现
     *后面可能会更新
     */
    OpenGLTextureCubemaps::OpenGLTextureCubemaps(const std::vector<std::string> &paths)
    // : m_paths(paths)
    {
        // glCreateTextures(GL_TEXTURE_CUBE_MAP, 1, &m_rendererID);
        glGenTextures(1, &m_rendererID);
        glBindTexture(GL_TEXTURE_CUBE_MAP, m_rendererID);

        for (unsigned int i = 0; i < paths.size(); i++)
        {
            int width, height, channels;
            stbi_uc *data = stbi_load(paths[i].c_str(), &width, &height, &channels, 0);

            m_width  = width;
            m_height = height;

            // 确认是RGB还是RGBA格式
            GLenum internalFormat = 0, dataFormat = 0;
            if (channels == 4)
            {
                internalFormat = GL_RGBA8;
                dataFormat     = GL_RGBA;
            }
            else if (channels == 3)
            {
                internalFormat = GL_RGB8;
                dataFormat     = GL_RGB;
            }

            m_internal_format = internalFormat;
            m_data_format     = dataFormat;

            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);

            stbi_image_free(data);
        }
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    }

    OpenGLTextureCubemaps::OpenGLTextureCubemaps(const std::string &path)
    {
        //---------读取hdr图片------------
        stbi_set_flip_vertically_on_load(true);
        int width, height, nrComponents;
        float *data = stbi_loadf(path.c_str(), &width, &height, &nrComponents, 0);
        unsigned int hdrTexture;
        if (data)
        {
            glGenTextures(1, &hdrTexture);
            glBindTexture(GL_TEXTURE_2D, hdrTexture);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, data);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

            stbi_image_free(data);
        }
        else
        {
            LOG_ERROR("Failed to load HDR image");
        }
        //--------------生成立方体贴图-----------------
        glGenTextures(1, &m_rendererID);
        glBindTexture(GL_TEXTURE_CUBE_MAP, m_rendererID);
        for (unsigned int i = 0; i < 6; ++i)
        {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, 1024, 1024, 0, GL_RGB, GL_FLOAT, nullptr);
        }
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        //------------渲染到立方体贴图------------
        glm::mat4 captureProjection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
        glm::mat4 captureViews[] =
            {
                glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)),
                glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)),
                glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)),
                glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f)),
                glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f)),
                glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, -1.0f, 0.0f))};
        // 创建framebuffer、shader、model
        FramebufferSpecification fbSpec;
        fbSpec.Attachments = {FramebufferTextureFormat::DEPTH32};
        fbSpec.Width       = 1024;
        fbSpec.Height      = 1024;
        std::shared_ptr<Framebuffer> m_hdr_framebuffer;
        m_hdr_framebuffer = Framebuffer::Create(fbSpec);
        std::shared_ptr<Shader> m_hdr_shader;
        m_hdr_shader = Shader::Create("../Engine/asset/shader/hdr_to_cubemap.vert", "../Engine/asset/shader/hdr_to_cubemap.frag");
        std::shared_ptr<Model> m_hdr_model;
        m_hdr_model = Model::Create("../resource/object/cube/cube.obj");
        m_hdr_shader->Use();
        m_hdr_shader->setInt("equirectangularMap", 0);
        m_hdr_shader->setMat4("projection", captureProjection);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, hdrTexture);

        // 渲染
        m_hdr_framebuffer->Bind();
        glViewport(0, 0, 1024, 1024);
        for (unsigned int i = 0; i < 6; ++i)
        {
            m_hdr_shader->setMat4("view", captureViews[i]);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, m_rendererID, 0);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            m_hdr_model->Render();
        }
        m_hdr_framebuffer->Unbind();
        // 清除
        m_hdr_framebuffer.reset();
        m_hdr_shader.reset();
        m_hdr_model.reset();
    }

    OpenGLTextureCubemaps::~OpenGLTextureCubemaps()
    {
        glDeleteTextures(1, &m_rendererID);
    }

    void OpenGLTextureCubemaps::Bind(uint32_t slot) const
    {
        glBindTextureUnit(slot, m_rendererID);
    }
} // namespace Yutrel