// ******************************************************************************
// File         : Buffers.cpp
// Project      : iKan : Renderer
// Description  : Interface for Vertex and Index Buffer class
//
// Created on   : 26/04/21.
// ******************************************************************************

#include "Buffers.h"
#include <iKan/Renderer/Renderer.h>
#include <iKan/Platform/OpenGL/OpenGLBuffers.h>

namespace iKan {
    
    // ******************************************************************************
    // Creating instance of Vertex Buffer
    // ******************************************************************************
    Ref<VertexBuffer> VertexBuffer::Create(uint32_t size, float* data)
    {
        switch (Renderer::GetAPI())
        {
            case Renderer::API::None   : IK_CORE_ASSERT(false, "Render API not Supporting");
            case Renderer::API::OpenGL : return CreateRef<OpenGLVertexBuffer>(size, data); break;
        }
        IK_CORE_ASSERT(false, "Unknown RendererAPI!");
        return nullptr;
    }
    
    // ******************************************************************************
    // Creating instance of Vertex Buffer
    // ******************************************************************************
    Ref<VertexBuffer> VertexBuffer::Create(uint32_t size)
    {
        switch (Renderer::GetAPI())
        {
            case Renderer::API::None:    IK_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
            case Renderer::API::OpenGL:  return CreateRef<OpenGLVertexBuffer>(size);
        }
        
        IK_CORE_ASSERT(false, "Unknown RendererAPI!");
        return nullptr;
    }

    // ******************************************************************************
    // Creating instance of Index Buffer
    // ******************************************************************************
    Ref<IndexBuffer> IndexBuffer::Create(uint32_t size, uint32_t* data)
    {
        switch (Renderer::GetAPI())
        {
            case Renderer::API::None   : IK_CORE_ASSERT(false, "Render API not Supporting");
            case Renderer::API::OpenGL : return CreateRef<OpenGLIndexBuffer>(size, data); break;
        }
        IK_CORE_ASSERT(false, "Unknown RendererAPI!");
        return nullptr;
    }
    
}
