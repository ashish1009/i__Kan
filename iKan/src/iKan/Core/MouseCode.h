// ******************************************************************************
// File         : MouseCode.h
// Description  : Mouse Code for GLFW MAC
// Project      : i-Kan : Core
//
// Created by Ashish on 17/04/21.
// Copyright © 2021 Ashish. All rights reserved.
// ******************************************************************************

#pragma once

namespace iKan
{
    // ******************************************************************************
    // Mouse button codes
    // ******************************************************************************
    enum class MouseCode : uint8_t
    {
        /* From glfw3.h */
        Button0                = 0,
        Button1                = 1,
        Button2                = 2,
        Button3                = 3,
        Button4                = 4,
        Button5                = 5,
        Button6                = 6,
        Button7                = 7,
        
        ButtonLast             = Button7,
        ButtonLeft             = Button0,
        ButtonRight            = Button1,
        ButtonMiddle           = Button2
    };

}
