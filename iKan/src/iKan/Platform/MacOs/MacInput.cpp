// ******************************************************************************
// File         : MacInput.cpp
// Description  : Handling the implementation for Iput handling at MAC
// Project      : iKan : Platform
//
// Created by Ashish on 18/04/21.
// Copyright © 2021 Ashish. All rights reserved.
// ******************************************************************************

#include <iKan/Core/Application.h>
#include <iKan/Core/Input.h>
#include <GLFW/glfw3.h>

// ******************************************************************************
// Implementation for handling MAC Input
// ******************************************************************************
namespace iKan {

    // ******************************************************************************
    // Pool the key pressed input
    // ******************************************************************************
    bool Input::IsKeyPressed(KeyCode key)
    {
        auto window  = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
        auto state   = glfwGetKey(window, static_cast<int32_t>(key));
        return state == GLFW_PRESS || state == GLFW_REPEAT;
    }
    
    // ******************************************************************************
    // Pool the mouse button pressed input
    // ******************************************************************************
    bool Input::IsMouseButtonPressed(MouseCode button)
    {
        auto window  = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
        auto state   = glfwGetMouseButton(window, static_cast<int32_t>(button));
        return state == GLFW_PRESS;
    }
    
    // ******************************************************************************
    // Pool the key released input
    // ******************************************************************************
    bool Input::IsKeyReleased(KeyCode key)
    {
        auto window  = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
        auto state   = glfwGetKey(window, static_cast<int32_t>(key));
        return state == GLFW_RELEASE;
    }
    
    // ******************************************************************************
    // Pool the mouse button released input
    // ******************************************************************************
    bool Input::IsMouseButtonReleased(MouseCode button)
    {
        auto window  = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
        auto state   = glfwGetMouseButton(window, static_cast<int32_t>(button));
        return state == GLFW_RELEASE;
    }
    
    // ******************************************************************************
    // Get the mouse position
    // ******************************************************************************
    std::pair<float, float> Input::GetMousePosition()
    {
        auto window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
        double xpos, ypos;
        glfwGetCursorPos(window, &xpos, &ypos);
        
        return { (float)xpos, (float)ypos };
    }
    
    // ******************************************************************************
    // get the x position of mouse
    // ******************************************************************************
    float Input::GetMouseX()
    {
        auto[x, y] = GetMousePosition();
        return x;
    }
    
    // ******************************************************************************
    // get the y position of mouse
    // ******************************************************************************
    float Input::GetMouseY()
    {
        auto[x, y] = GetMousePosition();
        return y;
    }
    
}

