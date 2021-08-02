// ******************************************************************************
// File         : OpenGLBuffers.h
// Description  : Implementation of Open GL Buffer class
// Project      : iKan : Plaftform
//
// Created by Ashish on 26/04/21.
// Copyright © 2021 Ashish. All rights reserved.
// ******************************************************************************

#pragma once

#include <iKan/Renderer/Buffers.h>

namespace iKan {
    
    // ******************************************************************************
    // Implementation of Open GL Vertex Buffer
    // ******************************************************************************
    class OpenGLVertexBuffer : public VertexBuffer
    {
    public:
        OpenGLVertexBuffer(uint32_t size);
        OpenGLVertexBuffer(uint32_t size, float* data);
        virtual ~OpenGLVertexBuffer();
        
        virtual void Bind() const override;
        virtual void Unbind() const override;

        virtual void SetData(const void* data, uint32_t size) override;

        virtual void AddLayout(const BufferLayout& layout) override { m_Layout = layout; }
        virtual const BufferLayout& GetLayout() const override { return m_Layout; }

        virtual uint32_t GetRendererID() const override { return m_RendererId; }
        
    private:
        uint32_t     m_RendererId;
        uint32_t     m_Size;
        BufferLayout m_Layout;
        Buffer       m_Data;
    };

    // ******************************************************************************
    // Implementation of Open GL Index Buffer
    // ******************************************************************************
    class OpenGLIndexBuffer : public IndexBuffer
    {
    public:
        OpenGLIndexBuffer(uint32_t count, uint32_t* data);
        virtual ~OpenGLIndexBuffer();
        
        virtual void Bind() const override;
        virtual void Unbind() const override;
        
        virtual uint32_t GetCount() const override { return m_Count; }
        virtual uint32_t GetRendererID() const override { return m_RendererId; }
        
    private:
        Buffer   m_Data;
        uint32_t m_RendererId;
        uint32_t m_Count;
    };

}
