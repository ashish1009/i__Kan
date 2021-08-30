// ******************************************************************************
// File         : OpenGLBuffers.cpp
// Description  : Implementation of Open GL Buffer class
// Project      : iKan : Plaftform
//
// Created by Ashish on 26/04/21.
// Copyright © 2021 Ashish. All rights reserved.
// ******************************************************************************

#include "OpenGLBuffers.h"
#include <iKan/Renderer/Renderer.h>
#include <glad/glad.h>

namespace iKan {

    // ******************************************************************************
    // Open GL Vertex Buffer Constructor without data
    // ******************************************************************************
    OpenGLVertexBuffer::OpenGLVertexBuffer(uint32_t size)
    : m_Size(size)
    {
        Renderer::Submit([this]()
                         {
            IK_CORE_INFO("Open GL Vertex Buffer constructed without data of size {0}", m_Size);

            glGenBuffers(1, &m_RendererId);
            glBindBuffer(GL_ARRAY_BUFFER, m_RendererId);
            glBufferData(GL_ARRAY_BUFFER, m_Size, nullptr, GL_DYNAMIC_DRAW);
        });
    }

    // ******************************************************************************
    // Open GL Vertex Buffer Constructor with data
    // ******************************************************************************
    OpenGLVertexBuffer::OpenGLVertexBuffer(uint32_t size, float* data)
    : m_Size(size)
    {
        m_Data.Clear();
        m_Data = Buffer::Copy(data, m_Size);
        Renderer::Submit([this]()
                         {
            IK_CORE_INFO("Open GL Vertex Buffer constructed with data of size {0}", m_Size);

            glGenBuffers(1, &m_RendererId);
            glBindBuffer(GL_ARRAY_BUFFER, m_RendererId);
            glBufferData(GL_ARRAY_BUFFER, m_Size, m_Data.Data, GL_STATIC_DRAW);
        });
    }
    
    // ******************************************************************************
    // Open GL Vertex Buffer Destructor with data
    // ******************************************************************************
    OpenGLVertexBuffer::~OpenGLVertexBuffer()
    {
        Renderer::Submit([this]()
                         {
            IK_CORE_WARN("Open GL Vertex Buffer destroyed ...");
            glDeleteBuffers(1, &m_RendererId);
        });
    }
    
    // ******************************************************************************
    // Bind Open GL Vertex Buffer
    // ******************************************************************************
    void OpenGLVertexBuffer::Bind() const
    {
        Renderer::Submit([this]() { glBindBuffer(GL_ARRAY_BUFFER, m_RendererId); });
    }
    
    // ******************************************************************************
    // Unbind Open GL Vertex Buffer
    // ******************************************************************************
    void OpenGLVertexBuffer::Unbind() const
    {
        Renderer::Submit([]() { glBindBuffer(GL_ARRAY_BUFFER, 0); });
    }
    
    // ******************************************************************************
    // Set the Open GL Vertex Buffer data dynamycally
    // ******************************************************************************
    void OpenGLVertexBuffer::SetData(void* data, uint32_t size)
    {
        m_Data.Clear();
        m_Data = Buffer::Copy(data, size);
        m_Size = size;
        Renderer::Submit([this]()
                         {
            glBindBuffer(GL_ARRAY_BUFFER, m_RendererId);
            glBufferSubData(GL_ARRAY_BUFFER, 0, m_Size, m_Data.Data);
        });
    }
        
    // ******************************************************************************
    // Open GL Index Buffer Constructor
    // ******************************************************************************
    OpenGLIndexBuffer::OpenGLIndexBuffer(uint32_t count, uint32_t* data)
    : m_Count(count)
    {
        m_Data.Clear();
        m_Data = Buffer::Copy(data, count * sizeof(uint32_t));
        Renderer::Submit([this]()
                         {
            IK_CORE_INFO("Open GL Index Buffer constructed with count {0}", m_Count);
            glGenBuffers(1, &m_RendererId);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RendererId);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint32_t) * m_Count, m_Data.Data, GL_STATIC_DRAW);
        });
    }
    
    // ******************************************************************************
    // Open GL Index Buffer Destructor
    // ******************************************************************************
    OpenGLIndexBuffer::~OpenGLIndexBuffer()
    {
        Renderer::Submit([this]()
                         {
            IK_CORE_WARN("Open GL Index Buffer destroyed");
            glDeleteBuffers(1, &m_RendererId);
        });
    }
    
    // ******************************************************************************
    // Open GL Index Buffer Binding
    // ******************************************************************************
    void OpenGLIndexBuffer::Bind() const
    {
        Renderer::Submit([this]() { glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RendererId); });
    }
    
    // ******************************************************************************
    // Open GL Index Buffer Unbinding
    // ******************************************************************************
    void OpenGLIndexBuffer::Unbind() const
    {
        Renderer::Submit([this]() { glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); });
    }
    
}
