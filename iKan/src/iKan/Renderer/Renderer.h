// ******************************************************************************
// File         : Renderer.h
// Description  : Renderer API Visible to Core and client
// Project      : iKan : Renderer
//
// Created by Ashish on 16/04/21.
// Copyright © 2021 Ashish. All rights reserved.
// ******************************************************************************

#pragma once

#include <iKan/Renderer/RendererAPI.h>
#include <iKan/Renderer/RenderCommandQueue.h>

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
        static void WaitAndRender();

        static void GetEntityIdFromPixels(int32_t mx, int32_t my, int32_t& pixelData);

        static RendererAPI::API GetAPI() { return RendererAPI::GetAPI(); }

        // NOTE: This API should be set before creating Windown instance as
        // Window creates context acc to the API
        static void SetAPI(RendererAPI::API api)
        {
            RendererAPI::SetAPI(api);
            s_RendererAPI = RendererAPI::Create();
        }

        // ******************************************************************************
        // Submit the renderer API in the Renderer Command Queue. Later will be executed
        // one by one. FunT is type of funcion
        // ******************************************************************************
        template<typename FuncT>
        static void Submit(FuncT&& func)
        {
            auto renderCmd = [](void* ptr) {
                auto pFunc = (FuncT*)ptr;
                (*pFunc)();

                // NOTE: Instead of destroying we could try and enforce all items to be trivally destructible
                // however some items like uniforms which contain std::strings still exist for now
                // static_assert(std::is_trivially_destructible_v<FuncT>, "FuncT must be trivially destructible");
                pFunc->~FuncT();
            };
            auto storageBuffer = GetRenderCommandQueue().Allocate(renderCmd, sizeof(func));
            new (storageBuffer) FuncT(std::forward<FuncT>(func));
        }
        
    private:
        static Scope<RendererAPI> s_RendererAPI;
        static RenderCommandQueue& GetRenderCommandQueue();
    };
    
}
