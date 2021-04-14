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
    // Constructor of Editor Application
    // ******************************************************************************
    EditorApp(const iKan::Application::Property& props)
    : iKan::Application(props)
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
// Mario Applicaiton. This will derive the ikan Application. Here different
// layers can be pushed in which rendering can be performed for Mario Game
// ******************************************************************************
class MarioApp : public iKan::Application
{
public:
    // ******************************************************************************
    // Constructor of Mario Application
    // ******************************************************************************
    MarioApp(const iKan::Application::Property& props)
    : iKan::Application(props)
    {
        IK_INFO("Constructing Mario Application");
    }
    
    // ******************************************************************************
    // Virtual destructor of Mario application
    // ******************************************************************************
    virtual ~MarioApp()
    {
        IK_INFO("Destroying Mario Application");
    }
};

// ******************************************************************************
// Client should decide which Application should be created for game loop
// ******************************************************************************
iKan::Scope<iKan::Application> iKan::CreateApplication()
{
    auto editorApp = iKan::CreateScope<EditorApp>(iKan::Application::Property("Editor", 800, 450 ));
    return editorApp;
}
