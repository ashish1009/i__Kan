// ******************************************************************************
// File         : RendererStats.h
// Projecte     : iKan : Renderer
// Description  : Store the statistics for Renderer
//
// Created on   : 02/05/21.
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
        
        static void Reset();
    };

}
