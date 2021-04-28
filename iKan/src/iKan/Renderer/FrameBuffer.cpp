// ******************************************************************************
// File         : Framebuffer.cpp
// Project      : i-Kan : Renderer
// Description  : Interface for Frame buffer class
//
// Created on   : 27/04/21.
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
