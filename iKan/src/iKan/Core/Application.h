// ******************************************************************************
// File         : Application.h
// Description  : Core application class to store all the functionality of Engine
// Project      : iKan : Core
//
//  Created on  : 12/04/21.
// ******************************************************************************

#pragma once

namespace iKan {
    
    // ******************************************************************************
    // Base class for Application. Client application will derive this class.
    // External layers will be inserted/deleted/modifying in client side
    // ******************************************************************************
    class Application
    {
    public:
        Application();
        ~Application();
        
    private:
    };
    
    // ******************************************************************************
    // Declaration for crating applicaiton instance at Client. Its body must be
    // defined in client side. This will retrun the main application instance on
    // which game loop is defined
    // ******************************************************************************
    std::unique_ptr<Application> CreateApplication();
}

