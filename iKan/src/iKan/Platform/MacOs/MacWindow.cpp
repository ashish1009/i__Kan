// ******************************************************************************
// File         : MacWindow.cpp
// Description  : Mac Window Class
// Project      : iKan : Platform
//
// Created on   : 13/04/21.
// ******************************************************************************

#include "MacWindow.h"

namespace iKan {
    
    // ******************************************************************************
    // Constructor of MAC Window
    // ******************************************************************************
    MacWindow::MacWindow(const Window::Property& prop)
    {
        IK_CORE_INFO("Constructing the MAC Window : {0} ({1} x {2})", prop.Title.c_str(), prop.Width, prop.Height);

        m_Data.Title  = prop.Title;
        m_Data.Width  = prop.Width;
        m_Data.Height = prop.Height;
        
       Init();
    }
    
    // ******************************************************************************
    // Destrucot of MAC Window
    // ******************************************************************************
    MacWindow::~MacWindow()
    {
        IK_CORE_WARN("Destroying MAC Window : {0} ({1} x {2})", m_Data.Title.c_str(), m_Data.Width, m_Data.Height);

        Shutdown();
    }
    
    // ******************************************************************************
    // Initializing of MAC Window
    // ******************************************************************************
    void MacWindow::Init()
    {
        // Initialize the library
        bool initSuccess = glfwInit();
        IK_CORE_ASSERT(initSuccess, "GLFW Init fails!!!!");
        
        // Configure GLFW Context Version
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        
#ifdef IK_PLATFORM_MAC
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

        // Create a windowed mode window and its OpenGL context
        m_Window = glfwCreateWindow(m_Data.Width, m_Data.Height, m_Data.Title.c_str(), NULL, NULL);
        if (!m_Window)
        {
            IK_CORE_ASSERT(false, "Unable to create Window!!!!");
            glfwTerminate();
            return;
        }
        IK_CORE_INFO("GLFW Window : {0} ({1}x{2}) created", m_Data.Title, m_Data.Width, m_Data.Height);
        
        // Set the pointer of GLFW Window
        glfwSetWindowUserPointer(m_Window, &m_Data);
    }
    
    // ******************************************************************************
    // Shutting down MAC Window
    // ******************************************************************************
    void MacWindow::Shutdown()
    {
        IK_CORE_WARN("Shutting down MAC Window : {0} ({1} x {2})", m_Data.Title.c_str(), m_Data.Width, m_Data.Height);
        
        glfwDestroyWindow(m_Window);
        IK_CORE_WARN("GLFW Window : {0} is destroyed Destroyed", m_Data.Title);
        glfwTerminate();
    }
    
    // ******************************************************************************
    // Constructor of MAC Window
    // ******************************************************************************
    void MacWindow::Update()
    {
        glfwPollEvents();
    }
    
    // ******************************************************************************
    // Set VSynch
    // ******************************************************************************
    void MacWindow::SetVSync(bool enabled)
    {
        IK_CORE_INFO("Setting VSynch : {0}", enabled);
        (true == enabled) ? glfwSwapInterval(1) : glfwSwapInterval(0);
        m_Data.VSync = enabled;
    }
    
    // ******************************************************************************
    // Get VSynch
    // ******************************************************************************
    bool MacWindow::IsVSync() const
    {
        IK_CORE_INFO("VSynch is set to : {0}", m_Data.VSync);
        return m_Data.VSync;
    }

}
