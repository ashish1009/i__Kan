// ******************************************************************************
// File         : Application.h
// Description  : Core application class to store all the functionality of Engine
// Project      : iKan : Core
//
//  Created on  : 12/04/21.
// ******************************************************************************

#pragma once

#include <iKan/Core/Layerstack.h>
#include <iKan/Core/Window.h>

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
            uint32_t Width = Window::Property::DefaultWidth, Height = Window::Property::DefaultHeight;
            
            Property(const std::string& title = "iKan", uint32_t width = Window::Property::DefaultWidth, uint32_t height = Window::Property::DefaultHeight)
            : Title(title), Width(width), Height(height)
            {
                
            }
            
            ~Property() = default;
        };
        
    public:
        Application(const Application::Property& props = Application::Property());
        virtual ~Application();
        
        void Init(const Application::Property& props);
        void Run();
        void EventHandler();
        void ImguiRenderer();
        void Close();
        
        void PushLayer(Ref<Layer> layer) { m_Layerstack.PushLayer(layer); };
        void PopLayer(Ref<Layer> layer) { m_Layerstack.PopLayer(layer); };

    private:
        void PushOverlay(Ref<Layer> overlay) { m_Layerstack.PushOverlay(overlay); };
        void PopOverlay(Ref<Layer> overlay) { m_Layerstack.PopOverlay(overlay); };
        
    private:
        // ******************************************************************************
        // Make sure the Instance of Window should be at first as in default destructor
        // of Application. Destructor of Window should called at last after Detaching all
        // Layers, as In Window Destructor we are TERMINATING the GLFW Window which might
        // have an GLFW Error while detaching few layers
        // ******************************************************************************
        Scope<Window> m_Window;
        Layerstack    m_Layerstack;
        
        bool m_IsRunning = true;
    };
    
    // ******************************************************************************
    // Declaration for crating applicaiton instance at Client. Its body must be
    // defined in client side. This will retrun the main application instance on
    // which game loop is defined
    // ******************************************************************************
    Scope<Application> CreateApplication();
}

