/**
 * 顶点数组
 * OpenGL的一种概念
 * 貌似其他Api没有
 */
#pragma once

#include "resource/asset/buffer.hpp"

#include <memory>

namespace Yutrel
{
    // 顶点数组类
    class VertexArray : public Asset
    {
    public:
        static Ref<VertexArray> Create();

        virtual ~VertexArray() = default;

        virtual void Bind() const   = 0;
        virtual void Unbind() const = 0;

        virtual void AddVertexBuffer(Ref<VertexBuffer> vertexBuffer) = 0;
        virtual void SetIndexBuffer(Ref<IndexBuffer> indexBuffer)    = 0;

        virtual const std::vector<Ref<VertexBuffer>>& GetVectorBuffers() const = 0;
        virtual const Ref<IndexBuffer> GetIndexBuffer() const                  = 0;
    };
} // namespace Yutrel