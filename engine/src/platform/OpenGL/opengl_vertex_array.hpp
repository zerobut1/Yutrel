#pragma once

#include "resource/asset/vertex_array.hpp"

namespace Yutrel
{
    class OpenGLVertexArray : public VertexArray
    {
    public:
        OpenGLVertexArray();
        virtual ~OpenGLVertexArray();

        virtual void Bind() const override;
        virtual void Unbind() const override;

        virtual void AddVertexBuffer(Ref<VertexBuffer> vertexBuffer) override;
        virtual void SetIndexBuffer(Ref<IndexBuffer> indexBuffer) override;

        virtual const std::vector<Ref<VertexBuffer>>& GetVectorBuffers() const override { return m_VertexBuffers; }
        virtual const Ref<IndexBuffer> GetIndexBuffer() const override { return m_IndexBuffer; }

    private:
        uint32_t m_VertexBufferIndex = 0;
        std::vector<Ref<VertexBuffer>> m_VertexBuffers;
        Ref<IndexBuffer> m_IndexBuffer;
    };
} // namespace Yutrel