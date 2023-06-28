#include "yutrel_pch.hpp"

#include "buffer.h"

#include "Yutrel/platform/OpenGL/opengl_buffer.h"

namespace Yutrel
{
    std::shared_ptr<VertexBuffer> VertexBuffer::Create(float *vertices, uint32_t size)
    {
        return std::make_shared<OpenGLVertexBuffer>(vertices, size);
    }

    std::shared_ptr<IndexBuffer> IndexBuffer::Create(uint32_t *indices, uint32_t count)
    {
        return std::make_shared<OpenGLIndexBuffer>(indices, count);
    }
} // namespace Yutrel