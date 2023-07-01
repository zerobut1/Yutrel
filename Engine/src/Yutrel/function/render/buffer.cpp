#include "yutrel_pch.hpp"

#include "buffer.hpp"

#include "Yutrel/platform/OpenGL/opengl_buffer.hpp"

namespace Yutrel
{
    VertexBuffer* VertexBuffer::Create(float* vertices, uint32_t size)
    {
        return new OpenGLVertexBuffer(vertices, size);
    }

    IndexBuffer* IndexBuffer::Create(uint32_t* indices, uint32_t count)
    {
        return new OpenGLIndexBuffer(indices, count);
    }
} // namespace Yutrel