// #include "yutrel_pch.hpp"

// #include "opengl_framebuffer.hpp"

// #include "core/application/application.hpp"
// #include "function/render/renderer.hpp"
// #include "platform/OpenGL/opengl_vertex_array.hpp"

// #include <glad/glad.h>
// #include <memory>
// #include <stdint.h>

// namespace Yutrel
// {
//     // 记录当前绑定的缓冲 避免重复绑定
//     static uint32_t cur_bind_renderbuffer = 0;
//     static uint32_t cur_bind_framebuffer  = 0;

//     //////////////////////////////////////////////
//     // Renderbuffer
//     //////////////////////////////////////////////

//     OpenGLRenderBuffer::OpenGLRenderBuffer(uint32_t width, uint32_t height, bool multisample)
//     {
//         glCreateRenderbuffers(1, &m_id);

//         // 默认使用 depth24 stencil8
//         if (multisample)
//         {
//             glNamedRenderbufferStorageMultisample(m_id, 4, GL_DEPTH24_STENCIL8, width, height);
//         }
//         else
//         {
//             glNamedRenderbufferStorage(m_id, GL_DEPTH24_STENCIL8, width, height);
//         }
//     }

//     OpenGLRenderBuffer::~OpenGLRenderBuffer()
//     {
//         glDeleteRenderbuffers(1, &m_id);
//     }

//     void OpenGLRenderBuffer::Bind() const
//     {
//         if (m_id != cur_bind_renderbuffer)
//         {
//             glBindRenderbuffer(GL_RENDERBUFFER, m_id);
//             cur_bind_renderbuffer = m_id;
//         }
//     }

//     void OpenGLRenderBuffer::Unbind() const
//     {
//         if (cur_bind_renderbuffer == m_id)
//         {
//             cur_bind_renderbuffer = 0;
//             glBindRenderbuffer(GL_RENDERBUFFER, 0);
//         }
//     }

//     //////////////////////////////////////////////
//     // Framebuffer
//     //////////////////////////////////////////////

//     // 拷贝颜色纹理
//     void OpenGLFramebuffer::CopyColor(const Ref<OpenGLFramebuffer> fr, uint32_t fr_idx, const Ref<OpenGLFramebuffer> to, uint32_t to_idx)
//     {
//         YUTREL_ASSERT(fr_idx < fr->m_color_attachments.size(), "Color buffer index {0} out of bound...", fr_idx);
//         YUTREL_ASSERT(to_idx < to->m_color_attachments.size(), "Color buffer index {0} out of bound...", to_idx);

//         // if the source and target rectangle areas differ in size, interpolation will be applied
//         GLuint fw = fr->m_width, fh = fr->m_height;
//         GLuint tw = to->m_width, th = to->m_height;

//         glNamedFramebufferReadBuffer(fr->m_id, GL_COLOR_ATTACHMENT0 + fr_idx);
//         glNamedFramebufferDrawBuffer(to->m_id, GL_COLOR_ATTACHMENT0 + to_idx);
//         glBlitNamedFramebuffer(fr->m_id, to->m_id, 0, 0, fw, fh, 0, 0, tw, th, GL_COLOR_BUFFER_BIT, GL_NEAREST);
//     }

//     void OpenGLFramebuffer::CopyDepth(const Ref<OpenGLFramebuffer> fr, const Ref<OpenGLFramebuffer> to)
//     {
//         // make sure that GL_FRAMEBUFFER_SRGB is globally disabled when calling this function!
//         // if colorspace correction is enabled, depth values will be gamma encoded during blits...
//         GLuint fw = fr->m_width, fh = fr->m_height;
//         GLuint tw = to->m_width, th = to->m_height;
//         glBlitNamedFramebuffer(fr->m_id, to->m_id, 0, 0, fw, fh, 0, 0, tw, th, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
//     }

//     void OpenGLFramebuffer::CopyStencil(const Ref<OpenGLFramebuffer> fr, const Ref<OpenGLFramebuffer> to)
//     {
//         // make sure that GL_FRAMEBUFFER_SRGB is globally disabled when calling this function!
//         // if colorspace correction is enabled, stencil values will be gamma encoded during blits...
//         GLuint fw = fr->m_width, fh = fr->m_height;
//         GLuint tw = to->m_width, th = to->m_height;
//         glBlitNamedFramebuffer(fr->m_id, to->m_id, 0, 0, fw, fh, 0, 0, tw, th, GL_STENCIL_BUFFER_BIT, GL_NEAREST);
//     }

//     OpenGLFramebuffer::OpenGLFramebuffer(uint32_t width, uint32_t height)
//         : m_width(width), m_height(height)
//     {
//         // 关闭sRGB编码
//         glDisable(GL_FRAMEBUFFER_SRGB);

//         glCreateFramebuffers(1, &m_id);
//     }

//     OpenGLFramebuffer::~OpenGLFramebuffer()
//     {
//         Unbind();
//         glDeleteFramebuffers(1, &m_id);
//     }

//     void OpenGLFramebuffer::Bind() const
//     {
//         if (m_id != cur_bind_framebuffer)
//         {
//             YUTREL_ASSERT(m_status == GL_FRAMEBUFFER_COMPLETE, "Incomplete framebuffer status: {0}", m_status);
//             if (m_depst_renderbuffer)
//             {
//                 m_depst_renderbuffer->Bind();
//             }
//             glBindFramebuffer(GL_FRAMEBUFFER, m_id);
//             cur_bind_framebuffer = m_id;
//         }
//     }

//     void OpenGLFramebuffer::Unbind() const
//     {
//         if (cur_bind_framebuffer == m_id)
//         {
//             cur_bind_framebuffer = 0;
//             glBindFramebuffer(GL_FRAMEBUFFER, 0);
//         }
//     }

//     // 添加颜色纹理
//     void OpenGLFramebuffer::AddColorTexture(uint32_t count, bool multisample)
//     {
//         size_t n_color_buffs = m_color_attachments.size();

//         // m_color_attachments.reserve(n_color_buffs + count); // allocate storage upfront

//         for (int i = 0; i < count; i++)
//         {
//             GLenum target = multisample ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D;
//             // 创建纹理并存入vector
//             // todo texture
//             auto texture      = m_color_attachments.emplace_back(Texture::Create(target, m_width, m_height, 1, GL_RGBA16F, 1));
//             GLuint texture_id = texture->GetRendererID();

//             if (!multisample)
//             {
//                 glTextureParameteri(texture_id, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
//                 glTextureParameteri(texture_id, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//                 glTextureParameteri(texture_id, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
//                 glTextureParameteri(texture_id, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

//                 // 把边界颜色设为黑色
//                 static const float border_color[] = {0.0f, 0.0f, 0.0f, 1.0f};
//                 glTextureParameterfv(texture_id, GL_TEXTURE_BORDER_COLOR, border_color);
//             }

//             glNamedFramebufferTexture(m_id, GL_COLOR_ATTACHMENT0 + n_color_buffs + i, texture_id, 0);
//         }

//         SetDrawBuffers();
//         m_status = glCheckNamedFramebufferStatus(m_id, GL_FRAMEBUFFER);
//     }

//     // 设置某个颜色纹理
//     void OpenGLFramebuffer::SetColorTexture(GLenum index, uint32_t texture_2d)
//     {
//         size_t n_color_buffs = m_color_attachments.size();
//         YUTREL_ASSERT(index >= n_color_buffs, "Color attachment {0} is already occupied!", index);

//         // 把texture绑定到帧缓冲
//         glNamedFramebufferTexture(m_id, GL_COLOR_ATTACHMENT0 + index, texture_2d, 0);

//         SetDrawBuffers();
//         m_status = glCheckNamedFramebufferStatus(m_id, GL_FRAMEBUFFER);
//     }

//     // 设置cubemap纹理
//     void OpenGLFramebuffer::SetColorTexture(GLenum index, uint32_t texture_cubemap, uint32_t face)
//     {
//         size_t n_color_buffs = m_color_attachments.size();

//         YUTREL_ASSERT(index >= n_color_buffs, "Color attachment {0} is already occupied!", index);
//         YUTREL_ASSERT(face < 6, "Invalid cubemap face id, must be a number between 0 and 5!");

//         if constexpr (true)
//         {
//             // some Intel drivers do not support this DSA function
//             glNamedFramebufferTextureLayer(m_id, GL_COLOR_ATTACHMENT0 + index, texture_cubemap, 0, face);
//         }
//         else
//         {
//             glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + index, GL_TEXTURE_CUBE_MAP_POSITIVE_X + face, texture_cubemap, 0);
//         }

//         SetDrawBuffers();
//         m_status = glCheckNamedFramebufferStatus(m_id, GL_FRAMEBUFFER);
//     }

//     // 获取颜色纹理
//     const Ref<Texture> OpenGLFramebuffer::GetColorTexture(GLenum index) const
//     {
//         YUTREL_ASSERT(index < m_color_attachments.size(), "Invalid color attachment index: {0}", index);
//         return m_color_attachments[index];
//     }

//     // 添加深度模板纹理
//     void OpenGLFramebuffer::AddDepStTexture()
//     {
//         // 只能有一个深度模板缓冲
//         YUTREL_ASSERT(!m_depst_renderbuffer, "The framebuffer already has a depth stencil renderbuffer...");
//         YUTREL_ASSERT(!m_depst_texture, "Only one depth stencil texture can be attached to the framebuffer...");

//         // 设置深度缓冲纹理
//         m_depst_texture = Texture::Create(GL_TEXTURE_2D, m_width, m_height, 1, GL_DEPTH24_STENCIL8, 1);
//         glTextureParameteri(m_depst_texture->GetRendererID(), GL_DEPTH_STENCIL_TEXTURE_MODE, GL_DEPTH_COMPONENT);

//         GLint immutable_format;
//         glGetTextureParameteriv(m_depst_texture->GetRendererID(), GL_TEXTURE_IMMUTABLE_FORMAT, &immutable_format);
//         YUTREL_ASSERT(immutable_format == GL_TRUE, "Unable to attach an immutable depth stencil texture...");

//         // 设置模板缓冲纹理view
//         // to access the stencil values in GLSL, we need a separate texture view
//         glTextureView(m_stencil_view, GL_TEXTURE_2D, m_depst_texture->GetRendererID(), m_depst_texture->GetInternalFormat(), 0, 1, 0, 1);
//         glTextureParameteri(m_stencil_view, GL_DEPTH_STENCIL_TEXTURE_MODE, GL_STENCIL_INDEX);

//         glNamedFramebufferTexture(m_id, GL_DEPTH_STENCIL_ATTACHMENT, m_depst_texture->GetRendererID(), 0);
//         m_status = glCheckNamedFramebufferStatus(m_id, GL_FRAMEBUFFER);
//     }

//     // 添加深度模板3D纹理
//     void OpenGLFramebuffer::AddDepthTexture3D()
//     {
//         // 只能有一个深度模板缓冲
//         YUTREL_ASSERT(!m_depst_renderbuffer, "The framebuffer already has a depth stencil renderbuffer...");
//         YUTREL_ASSERT(!m_depst_texture, "Only one depth stencil texture can be attached to the framebuffer...");

//         // 设置深度缓冲纹理
//         // 这里默认五层级联阴影映射
//         m_depst_texture = Texture::Create(GL_TEXTURE_2D_ARRAY, m_width, m_height, 5, GL_DEPTH_COMPONENT32F, 1);
//         glTextureParameteri(m_depst_texture->GetRendererID(), GL_DEPTH_STENCIL_TEXTURE_MODE, GL_DEPTH_COMPONENT);

//         // GLint immutable_format;
//         // glGetTextureParameteriv(m_depst_texture->GetRendererID(), GL_TEXTURE_IMMUTABLE_FORMAT, &immutable_format);
//         // YUTREL_ASSERT(immutable_format == GL_TRUE, "Unable to attach an immutable depth stencil texture...");

//         glNamedFramebufferTexture(m_id, GL_DEPTH_ATTACHMENT, m_depst_texture->GetRendererID(), 0);
//         m_status = glCheckNamedFramebufferStatus(m_id, GL_FRAMEBUFFER);
//     }

//     // 添加深度模板缓冲
//     void OpenGLFramebuffer::AddDepStRenderBuffer(bool multisample)
//     {
//         // 只能有一个深度模板缓冲
//         YUTREL_ASSERT(!m_depst_texture, "The framebuffer already has a depth stencil texture...");
//         YUTREL_ASSERT(!m_depst_renderbuffer, "Only one depth stencil renderbuffer can be attached to the framebuffer...");

//         // 创建RBO
//         m_depst_renderbuffer = RenderBuffer::Create(m_width, m_height, multisample);
//         m_depst_renderbuffer->Bind();
//         glNamedFramebufferRenderbuffer(m_id, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_depst_renderbuffer->GetRendererID());

//         m_status = glCheckNamedFramebufferStatus(m_id, GL_FRAMEBUFFER);
//     }

//     // 添加深度cubemap纹理
//     void OpenGLFramebuffer::AddDepthCubemap()
//     {
//         // 只能有一个深度模板缓冲
//         YUTREL_ASSERT(!m_depst_renderbuffer, "The framebuffer already has a depth stencil renderbuffer...");
//         YUTREL_ASSERT(!m_depst_texture, "Only one depth stencil texture can be attached to the framebuffer...");

//         // 创建深度纹理
//         m_depst_texture   = Texture::Create(GL_TEXTURE_CUBE_MAP, m_width, m_height, 6, GL_DEPTH_COMPONENT24, 1);
//         GLuint texture_id = m_depst_texture->GetRendererID();

//         glTextureParameteri(texture_id, GL_DEPTH_STENCIL_TEXTURE_MODE, GL_DEPTH_COMPONENT);
//         glTextureParameteri(texture_id, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
//         glTextureParameteri(texture_id, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
//         glTextureParameteri(texture_id, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
//         glTextureParameteri(texture_id, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
//         glTextureParameteri(texture_id, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

//         glNamedFramebufferTexture(m_id, GL_DEPTH_ATTACHMENT, texture_id, 0);
//         const GLenum null[] = {GL_NONE};
//         glNamedFramebufferReadBuffer(m_id, GL_NONE);
//         glNamedFramebufferDrawBuffers(m_id, 1, null);

//         m_status = glCheckNamedFramebufferStatus(m_id, GL_FRAMEBUFFER);
//     }

//     // 获取深度纹理
//     const Ref<Texture> OpenGLFramebuffer::GetDepthTexture() const
//     {
//         YUTREL_ASSERT(m_depst_texture, "The framebuffer does not have a depth texture...");
//         return m_depst_texture;
//     }

//     // 获取模板纹理的view
//     const uint32_t OpenGLFramebuffer::GetStencilTexView() const
//     {
//         YUTREL_ASSERT(m_stencil_view, "The framebuffer does not have a stencil texture view...");
//         return m_stencil_view;
//     }

//     // 绘制到单一纹理
//     void OpenGLFramebuffer::SetDrawBuffer(uint32_t index) const
//     {
//         YUTREL_ASSERT(index < m_color_attachments.size(), "Color buffer index out of bound!");
//         const GLenum buffers[] = {GL_COLOR_ATTACHMENT0 + index};
//         glNamedFramebufferDrawBuffers(m_id, 1, buffers);
//     }

//     // 绘制到多个纹理
//     void OpenGLFramebuffer::SetDrawBuffers(std::vector<uint32_t> indices) const
//     {
//         size_t n_buffs  = m_color_attachments.size();
//         size_t n_index  = indices.size();
//         GLenum* buffers = new GLenum[n_index];

//         for (GLenum i = 0; i < n_index; i++)
//         {
//             // layout(location = i)
//             GLuint index = indices[i];
//             YUTREL_ASSERT(index < n_buffs, "Color buffer index {0} out of bound!", index);
//             *(buffers + i) = GL_COLOR_ATTACHMENT0 + index;
//         }

//         glNamedFramebufferDrawBuffers(m_id, n_index, buffers);
//         delete[] buffers;
//     }

//     // 绘制到所有纹理
//     void OpenGLFramebuffer::SetDrawBuffers() const
//     {
//         if (size_t n = m_color_attachments.size(); n > 0)
//         {
//             GLenum* attachments = new GLenum[n];

//             for (GLenum i = 0; i < n; i++)
//             {
//                 *(attachments + i) = GL_COLOR_ATTACHMENT0 + i;
//             }

//             glNamedFramebufferDrawBuffers(m_id, n, attachments);
//             delete[] attachments;
//         }
//     }

//     // 绘制某个纹理的内容
//     void OpenGLFramebuffer::Draw(uint32_t index) const
//     {
//         // Resources resources  = Resources(Application::GetInstance().GetWorld());
//         // auto internal_shader = resources.Get<Ref<Renderer>>()->GetRenderData()->framebuffer_shader;
//         auto internal_vao = VertexArray::Create();

//         // internal_shader->Bind();
//         internal_vao->Bind();

//         GLuint subroutine_index = 0;

//         // visualize one of the color attachments
//         if (index >= 0 && index < m_color_attachments.size())
//         {
//             subroutine_index = 0;
//             m_color_attachments[index]->Bind(0);
//         }
//         // visualize the linearized depth buffer
//         else if (index == -1)
//         {
//             subroutine_index = 1;
//             if (m_depst_texture != nullptr)
//             {
//                 m_depst_texture->Bind(0);
//             }
//             else
//             {
//                 LOG_ERROR("Unable to visualize the depth buffer, depth texture not available!");
//             }
//         }
//         // visualize the stencil buffer
//         else if (index == -2)
//         {
//             subroutine_index = 2;
//             if (m_stencil_view != 0)
//             {
//                 glBindTextureUnit(index, m_stencil_view);
//             }
//             else
//             {
//                 LOG_ERROR("Unable to visualize the stencil buffer, stencil view not available!");
//             }
//         }
//         else
//         {
//             LOG_ERROR("Buffer index {0} is not valid in the framebuffer!", index);
//             LOG_ERROR("Valid indices: 0-{0} (colors), -1 (depth), -2 (stencil)", m_color_attachments.size() - 1);
//         }

//         glUniformSubroutinesuiv(GL_FRAGMENT_SHADER, 1, &subroutine_index);

//         glDrawArrays(GL_TRIANGLES, 0, 3);
//     }

//     // 清除指定缓冲
//     void OpenGLFramebuffer::Clear(uint32_t index, glm::vec4 clear_color) const
//     {
//         const GLfloat clear_depth = 1.0f;
//         const GLint clear_stencil = 0;

//         // 清除纹理
//         if (index >= 0)
//         {
//             glClearNamedFramebufferfv(m_id, GL_COLOR, index, &clear_color[0]);
//         }
//         // 清除深度缓冲
//         else if (index == -1)
//         {
//             glClearNamedFramebufferfv(m_id, GL_DEPTH, 0, &clear_depth);
//         }
//         // 清除模板缓冲
//         else if (index == -2)
//         {
//             glClearNamedFramebufferiv(m_id, GL_STENCIL, 0, &clear_stencil);
//         }
//         else
//         {
//             LOG_ERROR("Buffer index {0} is not valid in the framebuffer!", index);
//         }
//     }

//     // 清除所有缓冲
//     void OpenGLFramebuffer::Clear() const
//     {
//         for (int i = 0; i < m_color_attachments.size(); i++)
//         {
//             Clear(i);
//         }

//         Clear(-1);
//         Clear(-2);
//     }

// } // namespace Yutrel