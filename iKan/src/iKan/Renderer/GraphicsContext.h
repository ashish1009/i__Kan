// ******************************************************************************
// File         : GraphicsContext.h
// Description  : Graphics context class interface
// Project      : iKan : Renderer
//
// Created by Ashish on 16/04/21.
// Copyright © 2021 Ashish. All rights reserved.
// ******************************************************************************

#pragma once

#include <GLFW/glfw3.h>

namespace iKan {
    
    // ******************************************************************************
    // Interface for Graphics context
    // ******************************************************************************
    class GraphicsContext
    {
    public:
        virtual ~GraphicsContext() = default;
        
        virtual void Init() = 0;
        virtual void SwapBuffers() = 0;
        
        static Scope<GraphicsContext> CreateContext(GLFWwindow* window);
    };
    
}
