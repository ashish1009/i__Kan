// ******************************************************************************
// File         : main.cpp
// Project      : Mario
// Descripyion  : Client side Application
//
// Created on   : 05/05/21.
// ******************************************************************************

#include <iKan.h>
#include <iKan/Entrypoint.h>

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
