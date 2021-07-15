// ******************************************************************************
// File         : OpenGLRendererAPI.h
// Description  : Implementation of Open GL Granphic Renderer API
// Project      : iKan : Platform
//
// Created by Ashish on 16/04/21.
// Copyright © 2021 Ashish. All rights reserved.
// ******************************************************************************

#pragma once

#include <iKan/Renderer/RendererAPI.h>

namespace iKan {

    // ******************************************************************************
    // Implementation of Open GL Rendereing API
    // ******************************************************************************
    class OpenGlRendererAPI : public RendererAPI
    {
    public:
        virtual void Init() override;
        virtual void SetClearColor(const glm::vec4& color) override;
        virtual void Clear() override;
        virtual void Depth(bool state) override;
        virtual void Blend(bool state) override;
        virtual void SetViewPort(uint32_t widht, uint32_t height) override;
        virtual void DrawIndexed(const Ref<VertexArray>& vertexArray, uint32_t count = 0) override;
        virtual void DrawIndexed(uint32_t count) override;
        virtual void SetStencils() override;
        virtual void StencilMask(uint32_t mask) override;
        virtual void DrawIndexed(uint32_t count, PrimitiveType type, bool depthTest) override;
        virtual void DrawElementBaseVertex(uint32_t indexCount, void* baseIndex, uint32_t baseVertex, bool depth) override;
        virtual void SetLineThickness(float thickness) override;
    };
    
}
