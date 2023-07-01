#pragma once

#include "Yutrel/function/render/vertex_array.hpp"

namespace Yutrel
{
    class OpenGLVertexArray : public VertexArray
    {
    public:
        OpenGLVertexArray();
        virtual ~OpenGLVertexArray();

        virtual void Bind() const override;
        virtual void Unbind() const override;

        virtual void addVertexBuffer(VertexBuffer*& vertexBuffer) override;
        virtual void setIndexBuffer(IndexBuffer* indexBuffer) override;

        virtual const std::vector<VertexBuffer*>& getVectorBuffers() const override { return m_VertexBuffers; }
        virtual const IndexBuffer* getIndexBuffer() const override { return m_IndexBuffer; }

    private:
        uint32_t m_RendererID;
        uint32_t m_VertexBufferIndex = 0;
        std::vector<VertexBuffer*> m_VertexBuffers;
        IndexBuffer* m_IndexBuffer;
    };
} // namespace Yutrel