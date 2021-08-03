// ******************************************************************************
// File         : Application.cpp
// Description  : Implentation of Core Application functionality
// Project      : iKan : Core
//
// Created by Ashish on 12/04/21.
// Copyright © 2021 Ashish. All rights reserved.
// ******************************************************************************

#include "Application.h"
#include <iKan/Imgui/ImguiAPI.h>

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
        Renderer::WaitAndRender();
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
        
        // Register Application Event Handler to GLFW Window. Interrupt in GLFW Window decides the
        // argiment for this function (which is event type) which should be derived from Base Event
        m_Window->SetEventCallback(IK_BIND_EVENT_FN(Application::EventHandler));
        
        // Attaching the Imgui layer at the end
        m_ImguiLayer = CreateRef<ImguiLayer>();
        PushOverlay(m_ImguiLayer);

        ImGuiAPI::SetGreyThemeColors();

        // Initialising the renderers
        Renderer::Init();
        Renderer::WaitAndRender();
    }

    // ******************************************************************************
    // Runs the game loop. Handle rendering for each frame
    // ******************************************************************************
    void Application::Run()
    {
        // Finish all pending init time render commands
        Renderer::WaitAndRender();
        
        IK_CORE_INFO("-----------------------------     Entering Game loop   ---------------------------------");
        while (m_IsRunning)
        {
            // Updating all the attached layer
            for (Ref<Layer> layer : m_Layerstack)
                layer->OnUpdate(m_Window->GetTime());

            // Render ImGui on render thread
            Application* app = this;
            Renderer::Submit([app]() { app->ImguiRenderer(); });

            Renderer::WaitAndRender();

            // Update the Window
            m_Window->Update();
        }
        IK_CORE_WARN("-----------------------------     Exiting Game loop    ---------------------------------");
    }

    // ******************************************************************************
    // Handle event triggered from window. Calls from GLFW interrupt, Base event is
    // argument which can be dynamically insanciate according to interrupt triggered
    // in GLFW Window.
    // ******************************************************************************
    void Application::EventHandler(Event& event)
    {
        EventDispatcher dispather(event);
        dispather.Dispatch<WindowCloseEvent>(IK_BIND_EVENT_FN(Application::OnWindowClose));
        dispather.Dispatch<WindowResizeEvent>(IK_BIND_EVENT_FN(Application::OnWindowResize));

        // Events handling for all layers
        for (auto it = m_Layerstack.rbegin(); it != m_Layerstack.rend(); it++)
            (*it)->OnEvent(event);
    }
    
    // ******************************************************************************
    // Application window close
    // ******************************************************************************
    bool Application::OnWindowClose(WindowCloseEvent& event)
    {
        IK_CORE_WARN("Application is closing");
        m_IsRunning = false;
        return false;
    }
    
    // ******************************************************************************
    // Application window resize
    // ******************************************************************************
    bool Application::OnWindowResize(WindowResizeEvent& event)
    {
        IK_CORE_INFO("Window is resized to {0} x {1}", event.GetWidth(), event.GetHeight());
        Renderer::SetViewport(event.GetWidth(),  event.GetHeight());
        return false;
    }

    // ******************************************************************************
    // Renderer Imgui for each frame
    // ******************************************************************************
    void Application::ImguiRenderer()
    {
        m_ImguiLayer->Begin();

        // Rendering ImGui for all the layers
        for (auto layer : m_Layerstack)
            layer->OnImguiRender(m_Window->GetTime());

        m_ImguiLayer->End();
    }

    // ******************************************************************************
    // Close the active application
    // ******************************************************************************
    void Application::Close()
    {
        IK_CORE_WARN("Application::Close(), Clossing the current application");
        m_IsRunning = false;
    }

}
