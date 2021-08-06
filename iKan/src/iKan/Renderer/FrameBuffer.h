// ******************************************************************************
// File         : Framebuffer.h
// Description  : Interface for Frame buffer class
// Project      : i-Kan : Renderer
//
// Created by Ashish on 27/04/21.
// Copyright © 2021 Ashish. All rights reserved.
// ******************************************************************************

#pragma once

#include <iKan/Core/Window.h>

namespace iKan {
    
    // ******************************************************************************
    // Interface for Frame buffer class
    // ******************************************************************************
    class Framebuffer
    {
    public:
        // ******************************************************************************
        // Frame buffer Texture specification. Stores the texture format.
        // ******************************************************************************
        struct TextureSpecification
        {
            // ******************************************************************************
            // Enum for texture format. ONly interface, real conversion will be done
            // ******************************************************************************
            enum class TextureFormat
            {
                None = 0, RGBA8, DEPTH24STENCIL8, R32I
            };

        public:
            TextureSpecification() = default;
            TextureSpecification(TextureSpecification::TextureFormat format)
            : Format(format) { }

            TextureSpecification::TextureFormat Format = TextureSpecification::TextureFormat::None;
        };

        // ******************************************************************************
        // Frame buffer attachment specification. Stores all the attachment in a vector
        // ******************************************************************************
        struct Attachments
        {
            Attachments() = default;
            Attachments(std::initializer_list<Framebuffer::TextureSpecification> attachments)
            : TextureFormats(attachments) { }

            std::vector<Framebuffer::TextureSpecification> TextureFormats;
        };
        
        // ******************************************************************************
        // Frame buffer specification. Store all the FB specificiaction
        // ******************************************************************************
        struct Specification
        {
            bool SwapChainTarget = false;

            uint32_t Width   = Window::Property::DefaultWidth;
            uint32_t Height  = Window::Property::DefaultHeight;
            uint32_t Samples = 1;

            Framebuffer::Attachments Attachments;
            
            Specification() = default;
            ~Specification() = default;
        };
        
    public:
        virtual ~Framebuffer() = default;
        
        virtual void Bind() = 0;
        virtual void Unbind() = 0;
        virtual void Resize(uint32_t width, uint32_t height) = 0;

        virtual const Specification& GetSpecification() const = 0;

        virtual uint32_t GetColorAttachmentRendererID(uint32_t index = 0) const = 0;
        virtual uint32_t GetRendererId() const = 0;

        static Ref<Framebuffer> Create(const Specification& spec);
    };
    
    
}
