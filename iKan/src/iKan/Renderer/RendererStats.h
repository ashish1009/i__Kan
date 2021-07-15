// ******************************************************************************
// File         : RendererStats.h
// Description  : Store the statistics for Renderer
// Projecte     : iKan : Renderer
//
// Created by Ashish on 02/05/21.
// Copyright © 2021 Ashish. All rights reserved.
// ******************************************************************************

#pragma once

namespace iKan {
    
    // ******************************************************************************
    // Stores the renderer stats
    // ******************************************************************************
    struct RendererStatistics
    {
    public:
        static uint32_t DrawCalls;
        static uint32_t VertexCount;
        static uint32_t IndexCount;
        static uint32_t TextureCount;
        static uint32_t LineCount;
        
        static void Reset();
    };

}
