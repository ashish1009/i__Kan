// ******************************************************************************
// File         : EditorApp.cpp
// Description  : Client side Application
// Project      : Editor
//
// Created on   : 12/04/21.
// ******************************************************************************

#include <iKan.h>
#include <iKan/Entrypoint.h>

#include "EditorLayer.h"

// ******************************************************************************
// Client side Applicaiton. This will derive the ikan Application. Here different
// layers can be pushed in which rendering can be performed
// ******************************************************************************
class EditorApp : public iKan::Application
{
public:
    // ******************************************************************************
    // Constructor of Editor layer
    // ******************************************************************************
    EditorApp()
    {
        IK_INFO("Constructing Editor Application");

        m_ActiveLayer = std::make_shared<EditorLayer>();
        PushLayer(m_ActiveLayer);
    }
    
    // ******************************************************************************
    // Virtual destructor of Editor application
    // ******************************************************************************
    virtual ~EditorApp()
    {
        IK_INFO("Destroying Editor Application");
        
        PopLayer(m_ActiveLayer);
    }
    
private:
    std::shared_ptr<iKan::Layer> m_ActiveLayer;
};

// ******************************************************************************
// Client should decide which Application should be created for game loop
// ******************************************************************************
iKan::Scope<iKan::Application> iKan::CreateApplication()
{
    return iKan::CreateScope<EditorApp>();
}
