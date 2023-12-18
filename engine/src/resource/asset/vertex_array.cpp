#include "yutrel_pch.hpp"

#include "vertex_array.hpp"

#include "platform/OpenGL/opengl_vertex_array.hpp"

namespace Yutrel
{
    Ref<VertexArray> VertexArray::Create()
    {
        return CreateRef<OpenGLVertexArray>();
    }
} // namespace Yutrel