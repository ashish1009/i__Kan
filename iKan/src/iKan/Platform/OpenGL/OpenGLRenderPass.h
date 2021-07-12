// ******************************************************************************
// File         : OpenGLRenderPass.h
// Description  : Implementation of Open GL RendererPass
// Project      : iKan : Platform
//
// Created by Ashish on 12/07/21.
// Copyright © 2021 Ashish. All rights reserved.
// ******************************************************************************

#pragma once

#include <iKan/Renderer/RenderPass.h>

namespace iKan {

    // ******************************************************************************
    // Implementation of Open GL RendererPass
    // ******************************************************************************
    class OpenGLRenderPass : public RenderPass
    {
    public:
        OpenGLRenderPass(const RenderPassSpecification& spec);
        virtual ~OpenGLRenderPass();

        virtual RenderPassSpecification& GetSpecification() override { return m_Specification; }
        virtual const RenderPassSpecification& GetSpecification() const override { return m_Specification; }
        
    private:
        RenderPassSpecification m_Specification;
    };

}
