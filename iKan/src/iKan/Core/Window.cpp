// ******************************************************************************
// File         : Window.cpp
// Description  : Instancing the window depending on platform
// Project      : iKan : Core
//
// Created on   : 13/04/21.
// ******************************************************************************

#include "Window.h"
#include <iKan/Platform/MacOs/MacWindow.h>

namespace iKan {
    
    // ******************************************************************************
    // Creating instance for window depnding on platform
    // ******************************************************************************
    Scope<Window> Window::Create(const Window::Property& props)
    {
        // TODO: Currently hardocing for MAC. Need to find proper way to get the platform
#ifdef IK_PLATFORM_MAC
        return CreateScope<MacWindow>(props);
#else
        IK_CORE_ASSERT(false, "Unknown platform!");
        return nullptr;
#endif
    }
    
}
