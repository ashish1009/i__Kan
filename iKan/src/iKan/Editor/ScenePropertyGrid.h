// ******************************************************************************
// File         : ScenePropertyGrid.h
// Description  : Funcions for Imgui porperty grid
// Project      : i-Kan : Editor
//
// Created by Ashish on 02/05/21.
// Copyright © 2021 Ashish. All rights reserved.
// ******************************************************************************

#pragma once

#include <imgui_internal.h>

// ******************************************************************************
// Wrapper for imgui interface for various operations
// ******************************************************************************
namespace iKan {
    
    class PropertyGrid
    {
    public:
        static bool String(const char* label, std::string& value, bool modifiable = true, bool error = false);

        static bool Float(const char* label, float& value, bool* checkboxFlag, float delta = 0.1f, float resetValue = 0.0f, float columnWidth = 100.0f);
        static bool Float2(const char* label, glm::vec2& value, bool* checkboxFlag, float delta = 0.1f, float resetValue = 0.0f, float columnWidth = 100.0f);
        static bool Float3(const char* label, glm::vec3& value, bool* checkboxFlag, float delta = 0.1f, float resetValue = 0.0f, float columnWidth = 100.0f );

        static bool CheckBox(const char* label, bool& value);
        
        static void CounterI(const std::string& name, uint32_t& counter);
        static void CounterF(const std::string& name, float& counter);
        
        static void ColorEdit(glm::vec4& colorRef);

    };

}
