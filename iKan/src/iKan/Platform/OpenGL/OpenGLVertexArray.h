// ******************************************************************************
// File         : OpenGLVertexArray.h
// Project      : iKan : Platform
// Description  : Implementation of Open GL Vertex Array
//
//  Created by Ashish . on 27/04/21.
// ******************************************************************************

#pragma once

#include <iKan/Renderer/VertexArray.h>

// ******************************************************************************
// Implementation of Open GL Vertex Array
// ******************************************************************************
namespace iKan {
    
    class OpenGLVertexArray : public VertexArray
    {
    public:
        OpenGLVertexArray();
        virtual ~OpenGLVertexArray();
        
        virtual void Bind() const override;
        virtual void Unbind() const override;
        
        virtual void AddVertexBuffer(const Ref<VertexBuffer>& vertexBuffer) override;
        virtual void SetIndexBuffer(const Ref<IndexBuffer>& indexBuffer) override;
        
        virtual const std::vector<Ref<VertexBuffer>>& GetVertexBuffers() const override { return m_VertexBuffers; }
        virtual const Ref<IndexBuffer>& GetIndexBuffer() const override { return m_IndexBuffer; }
        
    private:
        uint32_t m_RendererId;
        std::vector<Ref<VertexBuffer>> m_VertexBuffers;
        Ref<IndexBuffer> m_IndexBuffer;
    };
    
}
