// ******************************************************************************
// File         : OpenGLGraphicsContext.h
// Description  : Open GL Graphics context class implementation
// Project      : iKan : Platform
//
// Created by Ashish on 16/04/21.
// Copyright © 2021 Ashish. All rights reserved.
// ******************************************************************************

#pragma once

#include <iKan/Renderer/GraphicsContext.h>

namespace iKan {
    
    class OpenGLGraphicsContext : public GraphicsContext
    {
    public:
        OpenGLGraphicsContext(GLFWwindow* window);
        virtual ~OpenGLGraphicsContext();
        
        virtual void Init() override;
        virtual void SwapBuffers() override;
        
    private:
        GLFWwindow* m_Window;
    };
    
}
