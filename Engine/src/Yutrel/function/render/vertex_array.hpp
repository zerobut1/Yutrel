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

        virtual void addVertexBuffer(VertexBuffer*& vertexBuffer) = 0;
        virtual void setIndexBuffer(IndexBuffer* indexBuffer)     = 0;

        virtual const std::vector<VertexBuffer*>& getVectorBuffers() const = 0;
        virtual const IndexBuffer* getIndexBuffer() const                  = 0;
    };
} // namespace Yutrel