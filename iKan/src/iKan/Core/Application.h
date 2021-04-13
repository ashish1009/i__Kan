// ******************************************************************************
// File         : Application.h
// Description  : Core application class to store all the functionality of Engine
// Project      : iKan : Core
//
//  Created on  : 12/04/21.
// ******************************************************************************

#pragma once

#include <iKan/Core/Layerstack.h>

namespace iKan {
    
    // ******************************************************************************
    // Base class for Application. Client application will derive this class.
    // External layers will be inserted/deleted/modifying in client side
    // ******************************************************************************
    class Application
    {
    public:
        Application();
        virtual ~Application();
        
        void Init();
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
        bool m_IsRunning = true;
        
        Layerstack m_Layerstack;
    };
    
    // ******************************************************************************
    // Declaration for crating applicaiton instance at Client. Its body must be
    // defined in client side. This will retrun the main application instance on
    // which game loop is defined
    // ******************************************************************************
    Scope<Application> CreateApplication();
}

