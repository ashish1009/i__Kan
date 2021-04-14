// ******************************************************************************
// File         : Application.cpp
// Description  : Implentation of Core Application functionality
// Project      : iKan : Core
//
// Created on   : 12/04/21.
// ******************************************************************************

#include "Application.h"

namespace iKan {
    
    Application* Application::s_Instance = nullptr;

    // ******************************************************************************
    // Application Constructor
    // ******************************************************************************
    Application::Application(const Application::Property& props)
    {
        IK_CORE_ASSERT(!s_Instance, "Application already exists !!!");
        IK_CORE_INFO("Constructing the application");
        
        s_Instance = this;
        Init(props);
    }
    
    // ******************************************************************************
    // Application Desstructor
    // ******************************************************************************
    Application::~Application()
    {
        IK_CORE_WARN("Destroying the application");
    }
    
    // ******************************************************************************
    // Initialise the Application
    // ******************************************************************************
    void Application::Init(const Application::Property& props)
    {
        IK_CORE_INFO("Initialising the application");
        
        // Creating Window from Applicaition
        m_Window = Window::Create(Window::Property(props.Title, props.Width, props.Height));
    }

    // ******************************************************************************
    // Runs the game loop. Handle rendering for each frame
    // ******************************************************************************
    void Application::Run()
    {
        IK_CORE_INFO("Entering Game loop");
        while (m_IsRunning)
        {
            
        }
    }

    // ******************************************************************************
    // Handle event triggered from window
    // ******************************************************************************
    void Application::EventHandler()
    {

    }

    // ******************************************************************************
    // Renderer Imgui for each frame
    // ******************************************************************************
    void Application::ImguiRenderer()
    {

    }

    // ******************************************************************************
    // Close the active application
    // ******************************************************************************
    void Application::Close()
    {
        IK_CORE_WARN("Application::Close(), Clossing the current application");
    }

}
