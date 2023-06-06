#pragma once

#include "Yutrel/function/render/framebuffer.h"

namespace Yutrel
{
    class OpenGLFramebuffer : public Framebuffer
    {
    public:
        OpenGLFramebuffer(const FramebufferSpecification &spec);
        virtual ~OpenGLFramebuffer();

        void Invalidate();

        virtual void Bind() override;
        virtual void Unbind() override;

        virtual void resize(uint32_t width, uint32_t height) override;
        virtual int readPixel(uint32_t attachmentIndex, int x, int y) override;

        virtual void clearAttachment(uint32_t attachmentIndex, int value) override;

        virtual uint32_t getColorAttachmentRendererID(uint32_t index = 0) const override
        {
            // HZ_CORE_ASSERT(index < m_ColorAttachments.size());
            return m_ColorAttachments[index];
        }

        virtual const FramebufferSpecification &getSpecification() const override { return m_Specification; }

    private:
        uint32_t m_RendererID = 0;
        FramebufferSpecification m_Specification;

        std::vector<FramebufferTextureSpecification> m_ColorAttachmentSpecifications;
        FramebufferTextureSpecification m_DepthAttachmentSpecification = FramebufferTextureFormat::None;

        std::vector<uint32_t> m_ColorAttachments;
        uint32_t m_DepthAttachment = 0;
    };

} // namespace Yutrel
