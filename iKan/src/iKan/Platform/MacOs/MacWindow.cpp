// ******************************************************************************
// File         : MacWindow.cpp
// Description  : Mac Window Class
// Project      : iKan : Platform
//
// Created by Ashish on 13/04/21.
// Copyright © 2021 Ashish. All rights reserved.
// ******************************************************************************

#include "MacWindow.h"

#include <iKan/Core/Events/MouseEvent.h>
#include <iKan/Core/Events/KeyEvent.h>
#include <iKan/Core/Events/ApplicationEvent.h>

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

        // Creating Context according to API
        m_Context = GraphicsContext::CreateContext(m_Window);

        // Set the User defined pointer to GLFW Window, this pointer will be retrieved when
        // an interrupt will be triggered
        glfwSetWindowUserPointer(m_Window, &m_Data);
        
        // Set GLFW Callbacks
        SetCallBacks();
    }
    
    // ******************************************************************************
    // Restisters the functions that will be called then GLFW interrupt triggers
    // last argument of each function calls in this funciton it the lamda that will
    // be called when interrupt is triggered. So function arugument need to be used
    // according to event
    // ******************************************************************************
    void MacWindow::SetCallBacks()
    {
        glfwSetWindowSizeCallback(m_Window, [](GLFWwindow* window, int32_t width, int32_t height)
                                  {
            MacWindow::Data& data = *(MacWindow::Data*)glfwGetWindowUserPointer(window);
            
            data.Width  = width;
            data.Height = height;
            
            WindowResizeEvent event(width, height);
            data.EventCallback(event);
        });
        
        glfwSetWindowCloseCallback(m_Window, [](GLFWwindow* window)
                                   {
            MacWindow::Data& data = *(MacWindow::Data*)glfwGetWindowUserPointer(window);
            WindowCloseEvent event;
            data.EventCallback(event);
        });
        
        glfwSetWindowFocusCallback(m_Window, [](GLFWwindow* window, int icontified)
                                   {
            MacWindow::Data& data = *(MacWindow::Data*)glfwGetWindowUserPointer(window);
            WindowFocusEvent event(icontified);
            data.EventCallback(event);
        });
        
        glfwSetKeyCallback(m_Window, [](GLFWwindow* window, int32_t key, int32_t scancode, int32_t action, int32_t mods)
                           {
            MacWindow::Data& data = *(MacWindow::Data*)glfwGetWindowUserPointer(window);

            switch (action)
            {
                case GLFW_PRESS:
                {
                    KeyPressedEvent event(static_cast<KeyCode>(key), 0);
                    data.EventCallback(event);
                    break;
                }
                case GLFW_RELEASE:
                {
                    KeyReleasedEvent event(static_cast<KeyCode>(key));
                    data.EventCallback(event);
                    break;
                }
                case GLFW_REPEAT:
                {
                    KeyPressedEvent event(static_cast<KeyCode>(key), 1);
                    data.EventCallback(event);
                    break;
                }
            }
        });
        
        glfwSetCharCallback(m_Window, [](GLFWwindow* window, uint32_t keycode)
                            {
            MacWindow::Data& data = *(MacWindow::Data*)glfwGetWindowUserPointer(window);

            KeyTypedEvent event(static_cast<KeyCode>(keycode));
            data.EventCallback(event);
        });
        
        glfwSetMouseButtonCallback(m_Window, [](GLFWwindow* window, int32_t button, int32_t action, int32_t mods)
                                   {
            MacWindow::Data& data = *(MacWindow::Data*)glfwGetWindowUserPointer(window);

            switch (action)
            {
                case GLFW_PRESS:
                {
                    MouseButtonPressedEvent event(static_cast<MouseCode>(button));
                    data.EventCallback(event);
                    break;
                }
                case GLFW_RELEASE:
                {
                    MouseButtonReleasedEvent event(static_cast<MouseCode>(button));
                    data.EventCallback(event);
                    break;
                }
            }
        });
        
        glfwSetScrollCallback(m_Window, [](GLFWwindow* window, double xOffset, double yOffset)
                              {
            MacWindow::Data& data = *(MacWindow::Data*)glfwGetWindowUserPointer(window);

            MouseScrolledEvent event((float)xOffset, (float)yOffset);
            data.EventCallback(event);
        });
        
        glfwSetCursorPosCallback(m_Window, [](GLFWwindow* window, double xPos, double yPos)
                                 {
            MacWindow::Data& data = *(MacWindow::Data*)glfwGetWindowUserPointer(window);

            MouseMovedEvent event((float)xPos, (float)yPos);
            data.EventCallback(event);
        });
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
        m_Context->SwapBuffers();
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
