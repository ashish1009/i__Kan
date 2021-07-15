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
#include <iKan/Renderer/RenderPass.h>
#include <iKan/Renderer/Mesh.h>

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
        static void DrawIndexed(uint32_t count, RendererAPI::PrimitiveType type, bool depthTest = true);
        static void DrawElementBaseVertex(uint32_t IndexCount, void* BaseIndex, uint32_t BaseVertex, bool depth);
        static void Shutdown();

        static void BeginRenderPass(Ref<RenderPass> renderPass, bool clear = false);
        static void EndRenderPass();

        static RendererAPI::API GetAPI() { return RendererAPI::GetAPI(); }

        // NOTE: This API should be set before creating Windown instance as
        // Window creates context acc to the API
        static void SetAPI(RendererAPI::API api)
        {
            RendererAPI::SetAPI(api);
            s_RendererAPI = RendererAPI::Create();
        }

        static const Scope<RendererAPI>& GetRenderer() { return s_RendererAPI; }
        static void StencilMask(uint32_t mask) { return s_RendererAPI->StencilMask(mask); };

        static void SubmitQuad(Ref<MaterialInstance> material, const glm::mat4& transform = glm::mat4(1.0f));
        static void SubmitFullscreenQuad(Ref<MaterialInstance> material);
        static void SubmitMesh(Ref<Mesh> mesh, const glm::mat4& transform, Ref<MaterialInstance> overrideMaterial = nullptr);

        static void DrawAABB(const AABB& aabb, const glm::mat4& transform, const glm::vec4& color = glm::vec4(1.0f));
        static void DrawAABB(Ref<Mesh> mesh, const glm::mat4& transform, const glm::vec4& color = glm::vec4(1.0f));

        static void SetLineThickness(float thickness);
        
    private:
        static Scope<RendererAPI> s_RendererAPI;
    };
    
}
