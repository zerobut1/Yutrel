#pragma once

#include "Yutrel/function/render/buffer.hpp"

#include <memory>

namespace Yutrel
{
    // 顶点数组类
    class VertexArray
    {
    public:
        static VertexArray* Create();

        virtual ~VertexArray() = default;

        virtual void Bind() const   = 0;
        virtual void Unbind() const = 0;

        virtual void AddVertexBuffer(VertexBuffer*& vertexBuffer) = 0;
        virtual void SetIndexBuffer(IndexBuffer* indexBuffer)     = 0;

        virtual const std::vector<VertexBuffer*>& GetVectorBuffers() const = 0;
        virtual const IndexBuffer* GetIndexBuffer() const                  = 0;
    };
} // namespace Yutrel