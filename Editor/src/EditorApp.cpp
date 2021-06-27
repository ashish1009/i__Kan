// ******************************************************************************
// File         : EditorApp.cpp
// Description  : Client side Application
// Project      : Editor
//
// Created by Ashish on 12/04/21.
// Copyright © 2021 Ashish. All rights reserved.
// ******************************************************************************

#include <iKan.h>
#include <iKan/Entrypoint.h>

#include "IKanEditor.h"

// ******************************************************************************
// Client side Applicaiton. This will derive the ikan Application. Here different
// layers can be pushed in which rendering can be performed
// ******************************************************************************
class IKanEditorApp : public iKan::Application
{
public:
    // ******************************************************************************
    // Constructor of Mario Application
    // ******************************************************************************
    IKanEditorApp(const iKan::Application::Property& props)
    : iKan::Application(props)
    {
        IK_INFO("Constructing IKanEditor Application");
        m_ActiveLayer = std::make_shared<IKanEditor>();
        PushLayer(m_ActiveLayer);
    }

    // ******************************************************************************
    // Virtual destructor of Mario application
    // ******************************************************************************
    virtual ~IKanEditorApp()
    {
        IK_WARN("Destroying IKanEditor Application");
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
    auto appProp        = iKan::Application::Property("Editor", 1800, 950, RendererAPI::API::OpenGL );
    auto iKanEditorApp  = iKan::CreateScope<IKanEditorApp>(appProp);
    return iKanEditorApp;
}
