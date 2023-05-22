#include "yutrel_pch.h"

#include "buffer.h"

#include "runtime/platform/OpenGL/opengl_buffer.h"

namespace Yutrel
{
    std::shared_ptr<VertexBuffer> VertexBuffer::create(float *vertices, uint32_t size)
    {
        return std::make_shared<OpenGLVertexBuffer>(vertices, size);
    }

    std::shared_ptr<IndexBuffer> IndexBuffer::create(uint32_t *indices, uint32_t count)
    {
        return std::make_shared<OpenGLIndexBuffer>(indices, count);
    }
} // namespace Yutrel