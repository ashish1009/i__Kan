// ******************************************************************************
// File         : Application.h
// Description  : Core application class to store all the functionality of Engine
// Project      : iKan : Core
//
// Created by Ashish on 12/04/21.
// Copyright © 2021 Ashish. All rights reserved.
// ******************************************************************************

#pragma once

#include <iKan/Core/Layerstack.h>
#include <iKan/Core/Window.h>
#include <iKan/Core/Events/ApplicationEvent.h>
#include <iKan/Renderer/Renderer.h>
#include <iKan/Imgui/ImguiLayer.h>

namespace iKan {
    
    // ******************************************************************************
    // Base class for Application. Client application will derive this class.
    // External layers will be inserted/deleted/modifying in client side
    // ******************************************************************************
    class Application
    {
    public:
        // ******************************************************************************
        // Wrapper for application Properties
        // ******************************************************************************
        struct Property
        {
            std::string Title = "iKan Engine";
            
            uint32_t Width  = Window::Property::DefaultWidth;
            uint32_t Height = Window::Property::DefaultHeight;
            
            RendererAPI::API Api = RendererAPI::API::OpenGL;
            
            Property(const std::string& title = "iKan",
                     uint32_t width        = Window::Property::DefaultWidth,
                     uint32_t height       = Window::Property::DefaultHeight,
                     RendererAPI::API api  = RendererAPI::API::OpenGL)
            : Title(title), Width(width), Height(height), Api(api) { }
            
            ~Property() = default;
        };
        
    public:
        Application(const Application::Property& props = Application::Property());
        virtual ~Application();
        
        void Run();
        void EventHandler(Event& event);
        void ImguiRenderer();
        void Close();
        
        void PushLayer(Ref<Layer> layer) { m_Layerstack.PushLayer(layer); };
        void PopLayer(Ref<Layer> layer) { m_Layerstack.PopLayer(layer); };
        
        ImguiLayer* GetImGuiLayer() { return m_ImguiLayer.get(); }
        Window& GetWindow() { return *m_Window; }
        
        static Application& Get() { return *s_Instance; }

    private:
        void Init();
        void PushOverlay(Ref<Layer> overlay) { m_Layerstack.PushOverlay(overlay); };
        void PopOverlay(Ref<Layer> overlay) { m_Layerstack.PopOverlay(overlay); };
        
        bool OnWindowClose(WindowCloseEvent& event);
        bool OnWindowResize(WindowResizeEvent& event);
        
    private:
        // ******************************************************************************
        // Make sure the Instance of Window should be at first as in default destructor
        // of Application. Destructor of Window should called at last after Detaching all
        // Layers, as In Window Destructor we are TERMINATING the GLFW Window which might
        // have an GLFW Error while detaching few layers
        // ******************************************************************************
        Scope<Window>   m_Window;
        Ref<ImguiLayer> m_ImguiLayer;
        
        Layerstack m_Layerstack;
        Property   m_Property;

        bool  m_IsRunning = true;

        static Application* s_Instance;
    };
    
    // ******************************************************************************
    // Declaration for crating applicaiton instance at Client. Its body must be
    // defined in client side. This will retrun the main application instance on
    // which game loop is defined
    // ******************************************************************************
    Scope<Application> CreateApplication();
}

