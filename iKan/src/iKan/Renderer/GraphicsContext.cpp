// ******************************************************************************
// File         : GraphicsContext.cpp
// Description  : Implementation Graphics context class interface
// Project      : iKan : Renderer
//
// Created by Ashish on 16/04/21.
// Copyright © 2021 Ashish. All rights reserved.
// ******************************************************************************

#include "GraphicsContext.h"
#include <iKan/Renderer/Renderer.h>
#include <iKan/Platform/OpenGL/OpenGLGraphicsContext.h>

namespace iKan {

    // ******************************************************************************
    // Creates instance of Graphics Context, this instance depends on the API
    // Supported by engine
    // ******************************************************************************
    Scope<GraphicsContext> GraphicsContext::CreateContext(GLFWwindow *window)
    {
        switch(Renderer::GetAPI())
        {
            case RendererAPI::API::None:
            {
                IK_CORE_ASSERT(false, "Invalid Renderer API (None) which is not supported by iKan");
                break;
            }
            case RendererAPI::API::OpenGL:
            {
                IK_CORE_INFO("Creating Instance of Opne GL Graphics context");
                return CreateScope<OpenGLGraphicsContext>(window);
                break;
            }
        }
        return nullptr;
    }
    
}
