// ******************************************************************************
// File         : Renderer.cpp
// Description  : Renderer API Visible to Core and client
// Project      : iKan : Renderer
//
// Created on   : 16/04/21.
// ******************************************************************************

#include "Renderer.h"
#include <iKan/Renderer/SceneRenderer.h>

namespace iKan {
 
    Scope<RendererAPI> Renderer::s_RendererAPI = RendererAPI::Create();
    
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
        s_RendererAPI->DrawIndexed(count);
    }
    
    // ******************************************************************************
    // Draw command
    // ******************************************************************************
    void Renderer::DrawIndexed(const Ref<VertexArray> &vertexArray, uint32_t count)
    {
        s_RendererAPI->DrawIndexed(vertexArray, count);
    }
    
    // ******************************************************************************
    // Shutdown all renderer
    // ******************************************************************************
    void Renderer::Shutdown()
    {
        SceneRenderer::Shutdown();
    }
    
}
    
