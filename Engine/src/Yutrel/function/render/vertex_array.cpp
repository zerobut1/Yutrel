#include "yutrel_pch.h"

#include "vertex_array.h"

#include "Yutrel/platform/OpenGL/opengl_vertex_array.h"

namespace Yutrel
{
    std::shared_ptr<VertexArray> VertexArray::create()
    {
        return std::make_shared<OpenGLVertexArray>();
    }
} // namespace Yutrel