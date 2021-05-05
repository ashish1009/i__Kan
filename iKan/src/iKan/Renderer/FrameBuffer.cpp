// ******************************************************************************
// File         : Framebuffer.cpp
// Description  : Interface for Frame buffer class
// Project      : i-Kan : Renderer
//
// Created by Ashish on 27/04/21.
// Copyright © 2021 Ashish. All rights reserved.
// ******************************************************************************

#include "FrameBuffer.h"
#include <iKan/Renderer/Renderer.h>
#include <iKan/Platform/OpenGL/OpenGLFrameBuffer.h>

namespace iKan {
    
    // ******************************************************************************
    // Create instance for Framebuffer
    // ******************************************************************************
    Ref<Framebuffer> Framebuffer::Create(const Framebuffer::Specification& spec)
    {
        switch (Renderer::GetAPI())
        {
            case RendererAPI::API::None:    IK_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
            case RendererAPI::API::OpenGL:  return CreateRef<OpenGLFrameBuffer>(spec);
        }
        IK_CORE_ASSERT(false, "Unknown RendererAPI!");
        return nullptr;
    }
    
}
