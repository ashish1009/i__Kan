// ******************************************************************************
// File         : ScenePropertyGrid.cpp
// Description  : Funcions for Imgui porperty grid
// Project      : i-Kan : Editor
//
// Created by Ashish on 02/05/21.
// Copyright © 2021 Ashish. All rights reserved.
// ******************************************************************************

#include "ScenePropertyGrid.h"

// ******************************************************************************
// Wrapper for imgui interface for various operations
// ******************************************************************************
namespace iKan {

    // ******************************************************************************
    // Read and write the Number as String. Value can not be modified
    // ******************************************************************************
    bool PropertyGrid::String(const char* label, uint32_t value, float columnWidth, float column2Width, const char* hint, bool modifiable, bool multiple, int numLines, bool error)
    {
        std::string idStr;
        std::stringstream ss;
        ss << value;
        ss >> idStr;

        return String(label, idStr, columnWidth, column2Width, " ", false, multiple, numLines); // No need to add any hint as this is non modifiable
    }

    // ******************************************************************************
    // Read and write the String. Modify the value
    // if Modifiable is true then we can modify the value
    // Hint will be printed to String path
    // Multiple : flag to check multiple lines needed in strimg
    // numlines : if multiple line supported then number of rows
    // ******************************************************************************
    bool PropertyGrid::String(const char* label, std::string& value, float columnWidth, float column2Width, const char* hint, bool modifiable, bool multiple, int numLines, bool error)
    {
        bool modified = false;
        
        ImGui::Columns(2);
        ImGui::SetColumnWidth(0, columnWidth);

        ImGui::Text(label);
        ImGui::NextColumn();

        ImGui::SetColumnWidth(1, column2Width);
        ImGui::PushItemWidth(-1);
        
        // Copy the Name of entity to buffer that will be dumy text in property pannel
        char buffer[256];
        strcpy(buffer, value.c_str());
        
        std::string UIContextId = "##" + (std::string)label;
        
        // To make string Red in case error flag is true
        if (error)
        {
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.9f, 0.2f, 0.2f, 1.0f));
        }
        
        if (modifiable)
        {
            // Take input text from User in Property pannel. that will be name(Tag) of Selected Entity
            if (multiple)
            {
                if (ImGui::InputTextEx(UIContextId.c_str(), hint, buffer, IM_ARRAYSIZE(buffer), ImVec2(column2Width, numLines * 20.0f), ImGuiInputTextFlags_Multiline))
                {
                    value    = buffer;
                    modified = true;
                }
            }
            else
            {
                if (ImGui::InputTextWithHint(UIContextId.c_str(), hint, buffer, IM_ARRAYSIZE(buffer)))
                {
                    value    = buffer;
                    modified = true;
                }
            }
        }
        else
        {
            ImGui::InputText(UIContextId.c_str(), (char*)value.c_str(), 256, ImGuiInputTextFlags_ReadOnly);
        }
        
        // Pop red color if error is enabled
        if (error)
        {
            ImGui::PopStyleColor();
        }
        
        ImGui::PopItemWidth();
        ImGui::NextColumn();
        ImGui::Columns(1);

        return modified;
    }
    
    // ******************************************************************************
    // Drag Float
    // Lable        : is string to be printed
    // Value        : is float pointer that will be modified with drag
    //                slider
    // CheckboxFlag : Flag to have checkbox (conditional slider)
    // Delta        : Delta value of modification
    // Resetvalue   : Value after pressing the reset button
    // Column width : width of column
    // ******************************************************************************
    bool PropertyGrid::Float(const char* label, float& value, bool* checkboxFlag, float delta, float resetValue, float columnWidth)
    {
        bool modified = false;
        ImGuiIO& io   = ImGui::GetIO();
        auto boldFont = io.Fonts->Fonts[0];
                
        ImGui::PushID(label);
        
        ImGui::Columns(2);
        ImGui::SetColumnWidth(0, columnWidth);

        // this flag is to check we need to have slider float or not
        bool propFlag;
        if (checkboxFlag)
        {
            ImGui::Checkbox(label, checkboxFlag);
            propFlag = *checkboxFlag;
        }
        else
        {
            ImGui::Text(label);
            propFlag = true;
        }
        if (propFlag)
        {
            ImGui::NextColumn();
        
            ImGui::PushMultiItemsWidths(1, ImGui::CalcItemWidth());
            ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0, 0 });
            
            float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
            ImVec2 buttonSize = { lineHeight + 3.0f, lineHeight };
            
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.2f, 0.2f, 0.2f, 1.0f });
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.9f, 0.2f, 0.2f, 1.0f });
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
            
            ImGui::PushFont(boldFont);
            if (ImGui::Button("X", buttonSize))
            {
                value = resetValue;
            }
            ImGui::PopStyleColor(3);
            ImGui::PopFont();
            
            ImGui::SameLine();
            if (ImGui::DragFloat("##X", &value, delta, 0.0f, 0.0f, "%.2f"))
            {
                modified = true;
            }
            
            ImGui::PopItemWidth();
            
            ImGui::PopStyleVar();
        }
        
        ImGui::Columns(1);
        
        ImGui::PopID();
        
        return modified;
    }
    
    // ******************************************************************************
    // Drag Float X 2
    // Lable        : is string to be printed
    // Value        : is glm::vec2 (2 float pointer) that will be modified with drag
    //                slider
    // CheckboxFlag : Flag to have checkbox (conditional slider)
    // Delta        : Delta value of modification
    // Resetvalue   : Value after pressing the reset button
    // Column width : width of column

    // ******************************************************************************
    bool PropertyGrid::Float2(const char* label, glm::vec2& value, bool* checkboxFlag, float delta, float resetValue, float columnWidth)
    {
        bool modified   = false;
        ImGuiIO& io     = ImGui::GetIO();
        auto boldFont   = io.Fonts->Fonts[0];
        
        ImGui::PushID(label);
        
        ImGui::Columns(2);
        ImGui::SetColumnWidth(0, columnWidth);
        
        // this flag is to check we need to have slider float or not
        bool propFlag;
        if (checkboxFlag)
        {
            ImGui::Checkbox(label, checkboxFlag);
            propFlag = *checkboxFlag;
        }
        else
        {
            ImGui::Text(label);
            propFlag = true;
        }
        if (propFlag)
        {
            ImGui::NextColumn();
            
            ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth());
            ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0, 0 });
            
            float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
            ImVec2 buttonSize = { lineHeight + 3.0f, lineHeight };
            
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.2f, 0.2f, 0.2f, 1.0f });
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.9f, 0.2f, 0.2f, 1.0f });
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
            
            ImGui::PushFont(boldFont);
            if (ImGui::Button("X", buttonSize))
                value.x = resetValue;
            ImGui::PopStyleColor(3);
            ImGui::PopFont();
            
            ImGui::SameLine();
            if (ImGui::DragFloat("##X", &value.x, delta, 0.0f, 0.0f, "%.2f"))
                modified = true;
            
            ImGui::PopItemWidth();
            ImGui::SameLine();
            
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.2f, 0.2f, 0.2f, 1.0f });
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.3f, 0.8f, 0.3f, 1.0f });
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });
            ImGui::PushFont(boldFont);
            if (ImGui::Button("Y", buttonSize))
                value.y = resetValue;
            ImGui::PopStyleColor(3);
            ImGui::PopFont();
            
            ImGui::SameLine();
            if (ImGui::DragFloat("##Y", &value.y, delta, 0.0f, 0.0f, "%.2f"))
                modified = true;
            
            ImGui::PopItemWidth();
            
            ImGui::PopStyleVar();
        }
        
        ImGui::Columns(1);
        
        ImGui::PopID();
        
        return modified;
    }
    
    // ******************************************************************************
    // Drag Float X 3 with Check box
    // Lable        : is string to be printed
    // Value        : is glm::vec3 (3 float pointer) that will be modified with drag
    //                slider
    // CheckboxFlag : Flag to have checkbox (conditional slider)
    // Delta        : Delta value of modification
    // Resetvalue   : Value after pressing the reset button
    // Column width : width of column
    // ******************************************************************************
    bool PropertyGrid::Float3(const char* label, glm::vec3& value, bool* checkboxFlag, float delta, float resetValue, float columnWidth)
    {
        bool modified = false;
        ImGuiIO& io   = ImGui::GetIO();
        auto boldFont = io.Fonts->Fonts[0];
        
        ImGui::PushID(label);
        
        ImGui::Columns(2);
        ImGui::SetColumnWidth(0, columnWidth);
        
        // this flag is to check we need to have slider float or not
        bool propFlag;
        if (checkboxFlag)
        {
            ImGui::Checkbox(label, checkboxFlag);
            propFlag = *checkboxFlag;
        }
        else
        {
            ImGui::Text(label);
            propFlag = true;
        }
        
        if (propFlag)
        {
            ImGui::NextColumn();
        
            ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth());
            ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0, 0 });
            
            float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
            ImVec2 buttonSize = { lineHeight + 3.0f, lineHeight };
            
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.2f, 0.2f, 0.2f, 1.0f });
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.9f, 0.2f, 0.2f, 1.0f });
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
            
            ImGui::PushFont(boldFont);
            if (ImGui::Button("X", buttonSize))
                value.x = resetValue;
            ImGui::PopStyleColor(3);
            ImGui::PopFont();
            
            ImGui::SameLine();
            if (ImGui::DragFloat("##X", &value.x, delta, 0.0f, 0.0f, "%.2f"))
                modified = true;
            
            ImGui::PopItemWidth();
            ImGui::SameLine();
            
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.2f, 0.2f, 0.2f, 1.0f });
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.3f, 0.8f, 0.3f, 1.0f });
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });
            ImGui::PushFont(boldFont);
            if (ImGui::Button("Y", buttonSize))
                value.y = resetValue;
            ImGui::PopStyleColor(3);
            ImGui::PopFont();
            
            ImGui::SameLine();
            if (ImGui::DragFloat("##Y", &value.y, delta, 0.0f, 0.0f, "%.2f"))
                modified = true;
            
            ImGui::PopItemWidth();
            ImGui::SameLine();
            
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.2f, 0.2f, 0.2f, 1.0f });
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.2f, 0.35f, 0.9f, 1.0f });
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.1f, 0.25f, 0.8f, 1.0f });
            ImGui::PushFont(boldFont);
            if (ImGui::Button("Z", buttonSize))
                value.z = resetValue;
            ImGui::PopStyleColor(3);
            ImGui::PopFont();
            
            ImGui::SameLine();
            if (ImGui::DragFloat("##Z", &value.z, delta, 0.0f, 0.0f, "%.2f"))
                modified = true;
            
            ImGui::PopItemWidth();
            
            ImGui::PopStyleVar();
        }
        
        ImGui::Columns(1);
        
        ImGui::PopID();
        
        return modified;
    }

    // ******************************************************************************
    // Checkbox
    // ******************************************************************************
    bool PropertyGrid::CheckBox(const char* label, bool& value, float columnWidth)
    {
        bool modified = false;
        
        ImGui::Columns(2);
        ImGui::SetColumnWidth(0, columnWidth);

        ImGui::Text(label);
        ImGui::NextColumn();
        ImGui::PushItemWidth(-1);
        
        std::string UIContextId = "##" + (std::string)label;
        
        if (ImGui::Checkbox(UIContextId.c_str(), &value))
            modified = true;
        
        ImGui::PopItemWidth();
        ImGui::NextColumn();
        ImGui::Columns(1);

        return modified;
    }

    // ******************************************************************************
    // Integer counter
    // ******************************************************************************
    void PropertyGrid::CounterI(const std::string& name, uint32_t& counter)
    {
        ImGui::InputInt(name.c_str(), (int32_t*)&counter);
    }

    // ******************************************************************************
    // Float counter
    // ******************************************************************************
    void PropertyGrid::CounterF(const std::string& name, float& value)
    {
        uint32_t counter = (uint32_t)value;
        CounterI(name, counter);
        value = (uint32_t)counter;
    }
    
    // ******************************************************************************
    // Color Edit
    // ******************************************************************************
    void PropertyGrid::ColorEdit(glm::vec4& colorRef)
    {
        static ImVec4 color         = ImVec4(colorRef.r, colorRef.g, colorRef.b, colorRef.a);
        static ImVec4 refColorValue = color;
        
        static bool alphaPreview = true, alphaHalfPreview = true;
        ImGui::Checkbox("Alpha", &alphaPreview);  ImGui::SameLine(); ImGui::Checkbox("Half Alpha", &alphaHalfPreview);
        ImGuiColorEditFlags miscFlags = ImGuiColorEditFlags_PickerHueWheel | (alphaHalfPreview ? ImGuiColorEditFlags_AlphaPreviewHalf : (alphaPreview ? ImGuiColorEditFlags_AlphaPreview : 0));
        if (alphaPreview || alphaHalfPreview) miscFlags |= ImGuiColorEditFlags_AlphaBar; else miscFlags |= ImGuiColorEditFlags_NoAlpha;
        
        static bool sidePreview = true, refColor = false;
        ImGui::Checkbox("Side Preview", &sidePreview);
        if (sidePreview)
        {
            ImGui::SameLine();
            ImGui::Checkbox("Ref Color", &refColor);
            if (refColor)
            {
                ImGui::SameLine();
                ImGui::ColorEdit4("##RefColor", &refColorValue.x, ImGuiColorEditFlags_NoInputs | miscFlags);
            }
        }
        if (!sidePreview)
            miscFlags |= ImGuiColorEditFlags_NoSidePreview;
        
        ImGui::ColorPicker4("Back Ground##4", (float*)&color, miscFlags, refColor ? &refColorValue.x : NULL);
        
        colorRef = { color.x, color.y, color.z, color.w };
    }
}
