// ******************************************************************************
// File         : ImguiAPI.h
// Description  : Contains API that would be used for Imgui interface
// Project      : iKan : Imgui
//
// Created by Ashish on 02/05/21.
// Copyright © 2021 Ashish. All rights reserved.
// ******************************************************************************

#pragma once

namespace iKan {
    
    class ImGuiAPI
    {
    public:
        static void StartDcocking();
        static void EndDcocking();
        
        static void RendererVersion(bool *pIsOpen = nullptr);
        static void FrameRate(float ts, bool *pIsOpen = nullptr);
        static void RendererStats(bool *pIsOpen = nullptr);

        static void ColorEdit(glm::vec4& colorRef);
        
        static void SetGreyThemeColors();
        static void SetLightThemeColors();

        static const ImVec4& GetBgColor() { return s_BgColor; }

    private:
        static ImVec4 s_BgColor;
    };
    
}
