// ******************************************************************************
// File         : MarioApp.cpp
// Project      : Mario
// Descripyion  : Client side Application
//
// Created by Ashish on 05/05/21.
// Copyright © 2021 Ashish. All rights reserved.
// ******************************************************************************

#include <iKan.h>
#include <iKan/Entrypoint.h>

#include "MarioLayer.h"

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
        iKan::Ref<MarioLayer> marioLayer = iKan::CreateRef<MarioLayer>();
        PushLayer(marioLayer);
    }
    
    // ******************************************************************************
    // Virtual destructor of Mario application
    // ******************************************************************************
    virtual ~MarioApp()
    {
        IK_WARN("Destroying Mario Application");
    }
};

// ******************************************************************************
// Client should decide which Application should be created for game loop
// ******************************************************************************
iKan::Scope<iKan::Application> iKan::CreateApplication()
{
    auto appProp  = iKan::Application::Property("iKan Mario", 800, 450, RendererAPI::API::OpenGL );
    auto marioApp = iKan::CreateScope<MarioApp>(appProp);
    return marioApp;
}
