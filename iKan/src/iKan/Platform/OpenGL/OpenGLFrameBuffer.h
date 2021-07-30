// ******************************************************************************
// File         : OpenGLFramebuffer.h
// Description  : Implementation of Open GL Frame buffer
// Project      : iKan : Platform
//
// Created by Ashish on 27/04/21.
// Copyright © 2021 Ashish. All rights reserved.
// ******************************************************************************

#pragma once

#include <iKan/Renderer/FrameBuffer.h>

namespace iKan {
    
    // ******************************************************************************
    // Implementation of Open GL frame buffers
    // ******************************************************************************
    class OpenGLFrameBuffer : public Framebuffer
    {
    public:
        OpenGLFrameBuffer(const Framebuffer::Specification& specs);
        virtual ~OpenGLFrameBuffer();
        
        virtual void Bind() override;
        virtual void Unbind() override;
        
        virtual const Framebuffer::Specification& GetSpecification() const override { return m_Specifications; }
        virtual uint32_t GetColorAttachmentRendererID(uint32_t index = 0) const override
        {
            IK_CORE_ASSERT((index < m_ColorAttachments.size()), "Invalid color attachment");
            return m_ColorAttachments[index];
        }

        virtual void Resize(uint32_t width, uint32_t height) override;

    private:
        void Invalidate();
        
    private:
        uint32_t                    m_RendererId = 0;
        Framebuffer::Specification  m_Specifications;

        std::vector<Framebuffer::TextureSpecification> m_ColorAttachmentSpecifications;
        Framebuffer::TextureSpecification              m_DepthAttachmentSpecification = Framebuffer::TextureSpecification::TextureFormat::None;

        std::vector<uint32_t> m_ColorAttachments;
        uint32_t m_DepthAttachment = 0;
    };
    
}
