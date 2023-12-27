// #include "yutrel_pch.hpp"

// #include "opengl_buffer.hpp"

// #include <glad/glad.h>

// namespace Yutrel
// {
//     ///////////////////////////
//     // VertexBuffer
//     ///////////////////////////

//     OpenGLVertexBuffer::OpenGLVertexBuffer(void* vertices, uint32_t size)
//     {
//         glCreateBuffers(1, &m_id);
//         glBindBuffer(GL_ARRAY_BUFFER, m_id);
//         glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_STATIC_DRAW);
//     }

//     OpenGLVertexBuffer::~OpenGLVertexBuffer()
//     {
//         glDeleteBuffers(1, &m_id);
//     }

//     void OpenGLVertexBuffer::Bind() const
//     {
//         glBindBuffer(GL_ARRAY_BUFFER, m_id);
//     }

//     void OpenGLVertexBuffer::Unbind() const
//     {
//         glBindBuffer(GL_ARRAY_BUFFER, 0);
//     }

//     ///////////////////////////
//     // IndexBuffer
//     ///////////////////////////

//     OpenGLIndexBuffer::OpenGLIndexBuffer(uint32_t* indices, uint32_t count)
//         : m_count(count)
//     {
//         glCreateBuffers(1, &m_id);

//         // OpenGL4.6之后的新规范，EBO不需要绑定到GL_ELEMENT_ARRAY_BUFFER?
//         // 不是很理解，但是这样写的确不会报错
//         glBindBuffer(GL_ARRAY_BUFFER, m_id);
//         glBufferData(GL_ARRAY_BUFFER, count * sizeof(uint32_t), indices, GL_STATIC_DRAW);
//     }

//     OpenGLIndexBuffer::~OpenGLIndexBuffer()
//     {
//         glDeleteBuffers(1, &m_id);
//     }

//     void OpenGLIndexBuffer::Bind() const
//     {
//         glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_id);
//     }

//     void OpenGLIndexBuffer::Unbind() const
//     {
//         glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
//     }

//     ///////////////////////////
//     // UniformBuffer
//     ///////////////////////////

//     OpenGLUniformBuffer::OpenGLUniformBuffer(uint32_t size, uint32_t binding)
//     {
//         glCreateBuffers(1, &m_id);
//         glNamedBufferData(m_id, size, nullptr, GL_DYNAMIC_DRAW); // TODO: investigate usage hint
//         glBindBufferBase(GL_UNIFORM_BUFFER, binding, m_id);
//     }

//     OpenGLUniformBuffer::~OpenGLUniformBuffer()
//     {
//         glDeleteBuffers(1, &m_id);
//     }

//     void OpenGLUniformBuffer::SetData(const void* data, uint32_t size, uint32_t offset)
//     {
//         glNamedBufferSubData(m_id, offset, size, data);
//     }

// } // namespace Yutrel