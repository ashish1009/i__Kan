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
        static void HelpMarker(const char* desc);
        static bool String(const char* label, uint32_t value, float columnWidth, float column2Width = 300.0f, const char* hint = nullptr, bool modifiable = true, bool multiple = false, int numLines = 1, bool error = false);
        static bool String(const char* label, std::string& value, float columnWidth, float column2Width = 300.0f, const char* hint = nullptr, bool modifiable = true, bool multiple = false, int numLines = 1, bool error = false);
        static bool String(const char* label, const std::string& value, const char* hint, float columnWidth);

        static bool Float(const char* label, float& value, bool* checkboxFlag, float delta = 0.1f, float resetValue = 0.0f, float columnWidth = 100.0f);
        static bool Float2(const char* label, glm::vec2& value, bool* checkboxFlag, float delta = 0.1f, float resetValue = 0.0f, float columnWidth = 100.0f);
        static bool Float3(const char* label, glm::vec3& value, bool* checkboxFlag, float delta = 0.1f, float resetValue = 0.0f, float columnWidth = 100.0f );

        static bool CheckBox(const char* label, bool& value, float columnWidth);
        
        static void CounterI(const std::string& name, uint32_t& counter);
        static void CounterF(const std::string& name, float& counter);
        
        static void ColorEdit(glm::vec4& colorRef);

        static bool ImageButton(const std::string& lableId, uint32_t texId, ImVec2 size);
        static bool ImageButton(const int32_t lableId, uint32_t texId, ImVec2 size);

    };

}
