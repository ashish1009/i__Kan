// ******************************************************************************
// File         : RendererPass.h
// Description  : Renderer API Visible to Core and client
// Project      : iKan : Renderer
//
// Created by Ashish on 12/07/21.
// Copyright © 2021 Ashish. All rights reserved.
// ******************************************************************************

#pragma once

#include <iKan/Renderer/FrameBuffer.h>

namespace iKan {

    struct RenderPassSpecification
    {
        Ref<Framebuffer> TargetFramebuffer;
    };

    class RenderPass
    {
    public:
        virtual ~RenderPass() = default;

        virtual RenderPassSpecification& GetSpecification() = 0;
        virtual const RenderPassSpecification& GetSpecification() const = 0;

        static Ref<RenderPass> Create(const RenderPassSpecification& spec);
    };


}

