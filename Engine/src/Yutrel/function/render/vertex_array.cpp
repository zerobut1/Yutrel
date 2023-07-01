#include "yutrel_pch.hpp"

#include "vertex_array.hpp"

#include "Yutrel/platform/OpenGL/opengl_vertex_array.hpp"

namespace Yutrel
{
    VertexArray* VertexArray::Create()
    {
        return new OpenGLVertexArray();
    }
} // namespace Yutrel