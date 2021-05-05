// ******************************************************************************
// File         : OpenGLGraphicsContext.cpp
// Description  : Implementation of Open GL Graphics context class implementation
// Project      : iKan : Platform
//
// Created by Ashish on 16/04/21.
// Copyright © 2021 Ashish. All rights reserved.
// ******************************************************************************

#include <glad/glad.h>
#include "OpenGLGraphicsContext.h"

namespace iKan {
    
    // ******************************************************************************
    // Construcitng Open GL Context
    // ******************************************************************************
    OpenGLGraphicsContext::OpenGLGraphicsContext(GLFWwindow* window)
    : m_Window(window)
    {
        IK_CORE_INFO("Open GL Graohics Context Constructor called ");
        Init();
    }

    OpenGLGraphicsContext::~OpenGLGraphicsContext()
    {
        IK_CORE_WARN("Open GL Graohics Context Destroyed ");
    }
    
    // ******************************************************************************
    // Initialize Open GL Context
    // ******************************************************************************
    void OpenGLGraphicsContext::Init()
    {
        IK_CORE_INFO("Initialize Open GL Graohics context ");

        // make GLFW Window Context
        glfwMakeContextCurrent(m_Window);

        // ******************************************************************************
        // We pass GLAD the function to load the address of the OpenGL function
        // pointers which is OS-specific. GLFW gives us glfwGetProcAddress that
        // defines the correct function based on which OS we're compiling for
        // ******************************************************************************
        bool success = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
        IK_CORE_ASSERT(success, "Can not initialize the Glad");
        
        IK_CORE_INFO("Created Context of OpenGl :");
        IK_CORE_INFO("--------------------------------------------------");
        IK_CORE_INFO("  Vendor   : {0} ", glGetString(GL_VENDOR));
        IK_CORE_INFO("  Renderer : {0} ", glGetString(GL_RENDERER));
        IK_CORE_INFO("  Version  : {0} ", glGetString(GL_VERSION));
        IK_CORE_INFO("--------------------------------------------------");
    }
    
    // ******************************************************************************
    // Swap Open GL Buffers each frame
    // ******************************************************************************
    void OpenGLGraphicsContext::SwapBuffers()
    {
        glfwSwapBuffers(m_Window);
    }

    
}
