// ******************************************************************************
// File         : RendererAPI.h
// Description  : Interface for Graphics Renderer API
// Project      : iKan : Renderer
//
// Created by Ashish on 28/04/21.
// Copyright © 2021 Ashish. All rights reserved.
// ******************************************************************************

#pragma once

#include <iKan/Renderer/VertexArray.h>

namespace iKan {
            
    // ******************************************************************************
    // Stores the renderer API type, or graphics type and call the renderer function
    // accordingly
    // ******************************************************************************
    class RendererAPI
    {
    public:
        // ******************************************************************************
        // Stores the capability of renderer
        // ******************************************************************************
        struct Capabilities
        {
            std::string Vendor;
            std::string Renderer;
            std::string Version;
        };
        
        // ******************************************************************************
        // Type of Renderer iKan supports
        // ******************************************************************************
        enum class API { None = 0, OpenGL = 1 };
        
    public:
        virtual ~RendererAPI() = default;
        
        virtual void Init() = 0;
        virtual void SetClearColor(const glm::vec4& color) = 0;
        virtual void Clear() = 0;
        virtual void Depth(bool state) = 0;
        virtual void Blend(bool state) = 0;
        virtual void SetViewPort(uint32_t widht, uint32_t height) = 0;
        virtual void DrawIndexed(const Ref<VertexArray>& vertexArray, uint32_t count = 0) = 0;
        virtual void DrawIndexed(uint32_t count) = 0;
        
        // NOTE: This API should be set before creating Windown instance as
        // Window creates context acc to the API
        static void SetAPI(API api) { s_API = api; }
        static API GetAPI() { return s_API; }
        static Scope<RendererAPI> Create();

        static Capabilities& GetCapabilities()
        {
            static Capabilities capabilities;
            return capabilities;
        }
        
    private:
        static API s_API;
    };
    
}
