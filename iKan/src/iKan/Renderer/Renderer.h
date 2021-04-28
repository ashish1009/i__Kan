// ******************************************************************************
// File         : Renderer.h
// Description  : Renderer API Visible to Core and client
// Project      : iKan : Renderer
//
// Created on   : 16/04/21.
// ******************************************************************************

#pragma once

#include <iKan/Renderer/RendererAPI.h>

namespace iKan {
    
    class Renderer
    {
    public:
        static void Init();
        static void Clear(glm::vec4 color);
        static void Depth(bool state) { s_RendererAPI->Depth(state); }
        static void Blend(bool state) { s_RendererAPI->Blend(state); }
        
        static void SetViewport(float width, float height);
        static void DrawIndexed(const Ref<VertexArray>& vertexArray, uint32_t count = 0);
        static void DrawIndexed(uint32_t count);
        static void Shutdown();

        // NOTE: This API should be set before creating Windown instance as
        // Window creates context acc to the API
        static void SetAPI(RendererAPI::API api) { RendererAPI::SetAPI(api); }
        static RendererAPI::API GetAPI() { return RendererAPI::GetAPI(); }
        
    private:
        static Scope<RendererAPI> s_RendererAPI;
    };
    
}
