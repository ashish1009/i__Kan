// ******************************************************************************
// File         : EditorApp.cpp
// Description  : Client side Application
// Project      : Editor
//
// Created on   : 12/04/21.
// ******************************************************************************

#include <iKan.h>
#include <iKan/Entrypoint.h>

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
        
    }
    
    // ******************************************************************************
    // Virtual destructor of Editor application
    // ******************************************************************************
    virtual ~EditorApp()
    {
        
    }
    
private:
};

// ******************************************************************************
// Client should decide which Application should be created for game loop
// ******************************************************************************
iKan::Scope<iKan::Application> iKan::CreateApplication()
{
    return nullptr;
}
