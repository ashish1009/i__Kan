// ******************************************************************************
// File         : Application.cpp
// Description  : Implentation of Core Application functionality
// Project      : iKan : Core
//
// Created on   : 12/04/21.
// ******************************************************************************

#include "Application.h"

namespace iKan {

    // ******************************************************************************
    // Application Constructor
    // ******************************************************************************
    Application::Application()
    {
        IK_CORE_INFO("Constructing the application");
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
