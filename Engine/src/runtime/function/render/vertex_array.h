#pragma once

#include "runtime/function/render/buffer.h"

#include <memory>

namespace Yutrel
{
    // 顶点数组类
    class VertexArray
    {
    public:
        static std::shared_ptr<VertexArray> create();

        virtual ~VertexArray() = default;

        virtual void Bind() const   = 0;
        virtual void Unbind() const = 0;

        virtual void addVertexBuffer(const std::shared_ptr<VertexBuffer> &vertexBuffer) = 0;
        virtual void setIndexBuffer(const std::shared_ptr<IndexBuffer> &indexBuffer)    = 0;

        virtual const std::vector<std::shared_ptr<VertexBuffer>> &getVectorBuffers() const = 0;
        virtual const std::shared_ptr<IndexBuffer> &getIndexBuffer() const                 = 0;
    };
} // namespace Yutrel