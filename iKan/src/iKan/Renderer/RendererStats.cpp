// ******************************************************************************
// File         : RendererStats.cpp
// Description  : Store the statistics for Renderer
// Projecte     : iKan : Renderer
//
// Created by Ashish on 02/05/21.
// Copyright © 2021 Ashish. All rights reserved.
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
