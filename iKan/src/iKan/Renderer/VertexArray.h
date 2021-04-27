// ******************************************************************************
// File         : VertexArray.h
// Project      : iKan : Renderer
// Description  : Interface class for vertex Array
//
//  Created by Ashish . on 27/04/21.
// ******************************************************************************

#pragma once

#include <iKan/Renderer/Buffers.h>

namespace iKan {
    
    // ******************************************************************************
    // Interface of Vertex Array Class
    // ******************************************************************************
    class VertexArray
    {
    public:
        virtual ~VertexArray() = default;
        
        virtual void Bind() const = 0;
        virtual void Unbind() const = 0;
        virtual void AddVertexBuffer(const Ref<VertexBuffer>& vertexBuffer) = 0;
        virtual void SetIndexBuffer(const Ref<IndexBuffer>& indexBuffer) = 0;

        virtual const std::vector<Ref<VertexBuffer>>& GetVertexBuffers() const = 0;
        virtual const Ref<IndexBuffer>& GetIndexBuffer() const = 0;

        static Ref<VertexArray> Create();
    };
    
}
