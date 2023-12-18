#pragma once

#include "resource/asset/buffer.hpp"

namespace Yutrel
{
    class OpenGLVertexBuffer : public VertexBuffer
    {
    public:
        OpenGLVertexBuffer(void* vertices, uint32_t size);
        virtual ~OpenGLVertexBuffer();

        virtual void Bind() const override;
        virtual void Unbind() const override;

        virtual void SetLayout(const BufferLayout& layout) override { m_layout = layout; }
        virtual const BufferLayout& GetLayout() const override { return m_layout; }

    private:
        BufferLayout m_layout;
    };

    class OpenGLIndexBuffer : public IndexBuffer
    {
    public:
        OpenGLIndexBuffer(uint32_t* indices, uint32_t count);
        virtual ~OpenGLIndexBuffer();

        virtual void Bind() const override;
        virtual void Unbind() const override;

        virtual uint32_t GetCount() const override { return m_count; }

    private:
        uint32_t m_count;
    };

    class OpenGLUniformBuffer : public UniformBuffer
    {
    public:
        OpenGLUniformBuffer(uint32_t size, uint32_t binding);
        virtual ~OpenGLUniformBuffer();

        virtual void SetData(const void* data, uint32_t size, uint32_t offset = 0) override;
        virtual uint32_t GetBinding() const override { return m_binding; }

    private:
        uint32_t m_size          = 0;
        uint32_t m_binding       = 0;
        uint8_t* m_local_storage = nullptr;
    };

} // namespace Yutrel