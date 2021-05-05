// ******************************************************************************
// File         : Viewport.h
// Description  : stores all the information of Viewport
// Project      : iKan
//
// Created by Ashish on 02/05/21.
// Copyright © 2021 Ashish. All rights reserved.
// ******************************************************************************

#pragma once

#include <iKan/Renderer/FrameBuffer.h>

namespace iKan {
    
    class Viewport
    {
    private:
        Viewport() = default;
    
    public:
        ~Viewport() = default;
        
        Viewport(const Viewport& other) = delete;
        Viewport(Viewport&& other) = delete;
        
        Viewport& operator =(const Viewport& other) = delete;
        Viewport& operator =(Viewport&& other) = delete;
        
        void UpdateBounds();
        void OnUpdate();
        void UpdateMousePos();
        
        static Viewport& Get()
        {
            static Viewport viewport;
            return viewport;
        }
        
    public:
        // Flag for the presence of Viewport
        bool Present = true;
        
        // Viewport focused or hovered
        bool Focused = false, Hovered = false;
        
        // Dimentsion of Viewport. Height of viewport is including the Title Tab height too
        float Width  = 0.0f, Height = 0.0f;
        
        // Position of Mouse inside Viewport
        int32_t MousePosX = -1, MousePosY = -1;

        // Size of Viewport Height of this size is only view port Area
        glm::vec2 Size = { 0.0f, 0.0f };
        
        // position of Top left and Bottom right edges with respect to screen
        // 0 -> Top left, 1 -> bottom right
        glm::vec2 Bounds[2];
        
        ImVec2 CursorPos;
        
        Ref<Framebuffer> FrameBuffer;
    };
    
}
