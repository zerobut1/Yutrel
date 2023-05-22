#pragma once

#include "runtime/function/render/vertex_array.h"

namespace Yutrel
{
    class OpenGLVertexArray : public VertexArray
    {
    public:
        OpenGLVertexArray();
        virtual ~OpenGLVertexArray();

        virtual void Bind() const override;
        virtual void Unbind() const override;

        virtual void addVertexBuffer(const std::shared_ptr<VertexBuffer> &vertexBuffer) override;
        virtual void setIndexBuffer(const std::shared_ptr<IndexBuffer> &indexBuffer) override;

        virtual const std::vector<std::shared_ptr<VertexBuffer>> &getVectorBuffers() const override { return m_VertexBuffers; }
        virtual const std::shared_ptr<IndexBuffer> &getIndexBuffer() const override { return m_IndexBuffer; }

    private:
        uint32_t m_RendererID;
        uint32_t m_VertexBufferIndex = 0;
        std::vector<std::shared_ptr<VertexBuffer>> m_VertexBuffers;
        std::shared_ptr<IndexBuffer> m_IndexBuffer;
    };
} // namespace Yutrel