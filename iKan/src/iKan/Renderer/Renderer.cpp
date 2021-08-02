// ******************************************************************************
// File         : Renderer.cpp
// Description  : Renderer API Visible to Core and client
// Project      : iKan : Renderer
//
// Created by Ashish on 16/04/21.
// Copyright © 2021 Ashish. All rights reserved.
// ******************************************************************************

#include "Renderer.h"
#include <iKan/Renderer/SceneRenderer.h>
#include <iKan/Renderer/RendererStats.h>

namespace iKan {
 
    Scope<RendererAPI> Renderer::s_RendererAPI = RendererAPI::Create();
    
    struct RendererData
    {
        RenderCommandQueue m_CommandQueue;
    };
    static RendererData s_Data;

    // ******************************************************************************
    // Initialise all renderer
    // ******************************************************************************
    void Renderer::Init()
    {
        IK_CORE_INFO("Initialising all Renderers");

        s_RendererAPI->Init();
        SceneRenderer::Init();
    }
    
    // ******************************************************************************
    // IClear color and buffer
    // ******************************************************************************
    void Renderer::Clear(glm::vec4 color)
    {
        s_RendererAPI->Clear();
        s_RendererAPI->SetClearColor(color);
    }
    
    // ******************************************************************************
    // Set the view port
    // ******************************************************************************
    void Renderer::SetViewport(float width, float height)
    {
        s_RendererAPI->SetViewPort(width, height);
    }
    
    // ******************************************************************************
    // Draw commanr
    // ******************************************************************************
    void Renderer::DrawIndexed(uint32_t count)
    {
        RendererStatistics::DrawCalls++;
        s_RendererAPI->DrawIndexed(count);
    }
    
    // ******************************************************************************
    // Draw command
    // ******************************************************************************
    void Renderer::DrawIndexed(const Ref<VertexArray> &vertexArray, uint32_t count)
    {
        RendererStatistics::DrawCalls++;
        s_RendererAPI->DrawIndexed(vertexArray, count);
    }
    
    // ******************************************************************************
    // Shutdown all renderer
    // ******************************************************************************
    void Renderer::Shutdown()
    {
        SceneRenderer::Shutdown();
    }
    
    // ******************************************************************************
    // Return the Renderer Command Queue instance
    // ******************************************************************************
    RenderCommandQueue& Renderer::GetRenderCommandQueue()
    {
        return s_Data.m_CommandQueue;
    }

    // ******************************************************************************
    // Run all the commands
    // ******************************************************************************
    void Renderer::WaitAndRender()
    {
        s_Data.m_CommandQueue.Execute();
    }
    
}
    
