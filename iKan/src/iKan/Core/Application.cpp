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
    : m_Property(props)
    {
        IK_CORE_ASSERT(!s_Instance, "Application already exists !!!");
        IK_CORE_INFO("Constructing the application");
        
        s_Instance = this;
        Init();
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
    void Application::Init()
    {
        IK_CORE_INFO("Initialising the application");
        // Setiing Renderer API for iKan Engine
        // NOTE: This API should be set in very begining as Window creates context acc to the API
        Renderer::SetAPI(m_Property.Api);
        
        // Creating Window from Applicaition
        m_Window = Window::Create(Window::Property(m_Property.Title, m_Property.Width, m_Property.Height));
    }

    // ******************************************************************************
    // Runs the game loop. Handle rendering for each frame
    // ******************************************************************************
    void Application::Run()
    {
        IK_CORE_INFO("Entering Game loop");
        while (m_IsRunning)
        {
            m_Window->Update();
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
