// ******************************************************************************
// File         : Input.h
// Description  : Input class for handling keyboard and mouse event
// Project      : iKan : Core
//
// Created on   : 18/04/21.
// ******************************************************************************

#pragma once

#include <iKan/Core/KeyCode.h>
#include <iKan/Core/MouseCode.h>

namespace iKan {
    
    // ******************************************************************************
    // Interface for handling the Input
    // ******************************************************************************
    class Input
    {
    public:
        static bool IsKeyPressed(KeyCode key);
        static bool IsMouseButtonPressed(MouseCode button);
        
        static bool IsKeyReleased(KeyCode key);
        static bool IsMouseButtonReleased(MouseCode button);
        
        static std::pair<float, float> GetMousePosition();
        static float GetMouseX();
        static float GetMouseY();
    };
    
}
