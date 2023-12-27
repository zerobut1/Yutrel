// #include "yutrel_pch.hpp"

// #include "opengl_vertex_array.hpp"

// #include <glad/glad.h>

// namespace Yutrel
// {
//     static GLenum ShaderDataTypeToOpenGLBaseType(ShaderDataType type)
//     {
//         switch (type)
//         {
//         case ShaderDataType::Float:
//             return GL_FLOAT;
//         case ShaderDataType::Float2:
//             return GL_FLOAT;
//         case ShaderDataType::Float3:
//             return GL_FLOAT;
//         case ShaderDataType::Float4:
//             return GL_FLOAT;
//         case ShaderDataType::Mat3:
//             return GL_FLOAT;
//         case ShaderDataType::Mat4:
//             return GL_FLOAT;
//         case ShaderDataType::Int:
//             return GL_INT;
//         case ShaderDataType::Int2:
//             return GL_INT;
//         case ShaderDataType::Int3:
//             return GL_INT;
//         case ShaderDataType::Int4:
//             return GL_INT;
//         case ShaderDataType::Bool:
//             return GL_BOOL;
//         default:
//             YUTREL_ASSERT(false, "Unknown ShaderDataType!");
//             break;
//         }
//         return 0;
//     }

//     OpenGLVertexArray::OpenGLVertexArray()
//     {
//         glCreateVertexArrays(1, &m_id);
//     }

//     OpenGLVertexArray::~OpenGLVertexArray()
//     {
//         glDeleteVertexArrays(1, &m_id);
//     }

//     void OpenGLVertexArray::Bind() const
//     {
//         glBindVertexArray(m_id);
//     }

//     void OpenGLVertexArray::Unbind() const
//     {
//         glBindVertexArray(0);
//     }

//     void OpenGLVertexArray::AddVertexBuffer(Ref<VertexBuffer> vertexBuffer)
//     {
//         // ENGINE_CORE_ASSERT(vertexBuffer->GetLayout().GetElements().size(), "Vertex Buffer has no layout!");

//         glBindVertexArray(m_id);
//         vertexBuffer->Bind();

//         const auto& layout = vertexBuffer->GetLayout();
//         for (const auto& element : layout)
//         {
//             if (ShaderDataTypeToOpenGLBaseType(element.Type) == GL_INT)
//             {
//                 glEnableVertexAttribArray(m_VertexBufferIndex);
//                 glVertexAttribIPointer(m_VertexBufferIndex,
//                                        element.getComponentCount(),
//                                        GL_INT,
//                                        layout.GetStride(),
//                                        (const void*)element.Offset);
//                 m_VertexBufferIndex++;
//             }
//             else
//             {
//                 glEnableVertexAttribArray(m_VertexBufferIndex);
//                 glVertexAttribPointer(m_VertexBufferIndex,
//                                       element.getComponentCount(),
//                                       ShaderDataTypeToOpenGLBaseType(element.Type),
//                                       element.Normalized ? GL_TRUE : GL_FALSE,
//                                       layout.GetStride(),
//                                       (const void*)element.Offset);
//                 m_VertexBufferIndex++;
//             }
//         }
//         glEnableVertexAttribArray(0);
//         m_VertexBuffers.emplace_back(vertexBuffer);
//     }

//     void OpenGLVertexArray::SetIndexBuffer(Ref<IndexBuffer> indexBuffer)
//     {
//         glBindVertexArray(m_id);
//         indexBuffer->Bind();

//         m_IndexBuffer = indexBuffer;
//     }

// } // namespace Yutrel