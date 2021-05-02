// ******************************************************************************
// File         : RendererStats.cpp
// Projecte     : iKan : Renderer
// Description  : Store the statistics for Renderer
//
// Created on   : 02/05/21.
// ******************************************************************************

#include "RendererStats.h"

namespace iKan {

    uint32_t RendererStatistics::DrawCalls   = 0;
    uint32_t RendererStatistics::VertexCount = 0;
    uint32_t RendererStatistics::IndexCount  = 0;
    uint32_t RendererStatistics::TextureCount = 0;
    
    // ******************************************************************************
    // Reset the renderer stats
    // ******************************************************************************
    void RendererStatistics::Reset()
    {
        DrawCalls    = 0;
        VertexCount  = 0;
        IndexCount   = 0;
        TextureCount = 0;
    }

}
