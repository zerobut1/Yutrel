#include "yutrel_pch.hpp"

#include "buffer.hpp"

#include "platform/OpenGL/opengl_buffer.hpp"

namespace Yutrel
{
    Ref<VertexBuffer> VertexBuffer::Create(void* vertices, uint32_t size)
    {
        return CreateRef<OpenGLVertexBuffer>(vertices, size);
    }

    Ref<IndexBuffer> IndexBuffer::Create(uint32_t* indices, uint32_t count)
    {
        return CreateRef<OpenGLIndexBuffer>(indices, count);
    }

    Ref<UniformBuffer> UniformBuffer::Create(uint32_t size, uint32_t binding)
    {
        return CreateRef<OpenGLUniformBuffer>(size, binding);
    }

} // namespace Yutrel