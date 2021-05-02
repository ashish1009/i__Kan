// ******************************************************************************
// File         : ImguiAPI.h
// Project      : iKan : Imgui
// Description  : Contains API that would be used for Imgui interface
//
// Created on   : 02/05/21.
// ******************************************************************************

#pragma once

namespace iKan {
    
    class ImGuiAPI
    {
    public:
        static void StartDcocking();
        static void EndDcocking();
        
        static void RendererVersion(bool *pIsOpen = nullptr);
        static void FrameRate(bool *pIsOpen = nullptr);
        static void RendererStats(bool *pIsOpen = nullptr);
    };
    
}
