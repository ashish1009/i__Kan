// ******************************************************************************
// File         : OpenGLVertexArray.cpp
// Project      : iKan : Platform
// Description  : Implementation of Open GL Vertex Array
//
//  Created by Ashish . on 27/04/21.
// ******************************************************************************

#include "OpenGLVertexArray.h"
#include <glad/glad.h>

namespace iKan {
    
    // ******************************************************************************
    // Get the Open GL Data type from user defined Shader data type
    // ******************************************************************************
    static GLenum ShaderDataTypeToOpenGLBaseType(ShaderDataType type)
    {
        switch (type)
        {
            case ShaderDataType::NoType:   return 0;
            case ShaderDataType::Float:    return GL_FLOAT;
            case ShaderDataType::Float2:   return GL_FLOAT;
            case ShaderDataType::Float3:   return GL_FLOAT;
            case ShaderDataType::Float4:   return GL_FLOAT;
            case ShaderDataType::Mat3:     return GL_FLOAT;
            case ShaderDataType::Mat4:     return GL_FLOAT;
            case ShaderDataType::Int:      return GL_INT;
            case ShaderDataType::Int2:     return GL_INT;
            case ShaderDataType::Int3:     return GL_INT;
            case ShaderDataType::Int4:     return GL_INT;
            case ShaderDataType::Bool:     return GL_BOOL;
        }
        
        IK_CORE_ASSERT(false, "Unknown ShaderDataType!");
        return 0;
    }
    
    // ******************************************************************************
    // Open GL Vertex Array Constructor
    // ******************************************************************************
    OpenGLVertexArray::OpenGLVertexArray()
    {
        IK_CORE_INFO("Open GL Vertex Array Constructor called ");

        glGenVertexArrays(1, &m_RendererId);
        glBindVertexArray(m_RendererId);
    }
    
    // ******************************************************************************
    // Open GL Vertex Array Destructed
    // ******************************************************************************
    OpenGLVertexArray::~OpenGLVertexArray()
    {
        IK_CORE_WARN("Open GL Vertex array destroyed ");

        glDeleteVertexArrays(1, &m_RendererId);
    }
    
    // ******************************************************************************
    // Bind Open GL Vertex Array
    // ******************************************************************************
    void OpenGLVertexArray::Bind() const
    {
        glBindVertexArray(m_RendererId);
    }
    
    // ******************************************************************************
    // Unbind Open GL Vertex Array
    // ******************************************************************************
    void OpenGLVertexArray::Unbind() const
    {
        glBindVertexArray(0);
    }
    
    // ******************************************************************************
    // Add the vertex array in the Open GL Vertex Array.
    // ******************************************************************************
    void OpenGLVertexArray::AddVertexBuffer(const Ref<VertexBuffer>& vertexBuffer)
    {
        IK_CORE_INFO("Storing the Vertex Buffer into Vertex Array");

        glBindVertexArray(m_RendererId);
        m_VertexBuffers.push_back(vertexBuffer);
        
        uint32_t index = 0;
        const auto& layout = vertexBuffer->GetLayout();
        for (const auto& element : layout.GetElements())
        {
            switch (element.Type)
            {
                case ShaderDataType::Int:
                case ShaderDataType::Int2:
                case ShaderDataType::Int3:
                case ShaderDataType::Int4:
                case ShaderDataType::Bool:
                {
                    glEnableVertexAttribArray(index);
                    glVertexAttribIPointer(index,
                                          element.Count,
                                          ShaderDataTypeToOpenGLBaseType(element.Type),
                                          layout.GetStride(),
                                          (const void*)element.Offset);
                    index++;
                    break;
                }

                case ShaderDataType::Float:
                case ShaderDataType::Float2:
                case ShaderDataType::Float3:
                case ShaderDataType::Float4:
                {
                    glEnableVertexAttribArray(index);
                    glVertexAttribPointer(index,
                                          element.Count,
                                          ShaderDataTypeToOpenGLBaseType(element.Type),
                                          element.Normalized ? GL_TRUE : GL_FALSE,
                                          layout.GetStride(),
                                          (const void*)element.Offset);
                    index++;
                    break;
                }
                case ShaderDataType::Mat3:
                case ShaderDataType::Mat4:
                {
                    uint8_t count = element.Count;
                    for (uint8_t i = 0; i < count; i++)
                    {
                        glEnableVertexAttribArray(index);
                        glVertexAttribPointer(index,
                                              count,
                                              ShaderDataTypeToOpenGLBaseType(element.Type),
                                              element.Normalized ? GL_TRUE : GL_FALSE,
                                              layout.GetStride(),
                                              (const void*)(sizeof(float) * count * i));
                        glVertexAttribDivisor(index, 1);
                        index++;
                    }
                    break;
                }
                default:
                {
                    IK_CORE_ASSERT(false, "Unknown ShaderDataType!");
                }
            }
        }
    }
    
    // ******************************************************************************
    // Setting Open GL Vertex Array Constructor
    // ******************************************************************************
    void OpenGLVertexArray::SetIndexBuffer(const Ref<IndexBuffer>& indexBuffer)
    {
        IK_CORE_INFO("Setting up the Index Buffer into Vertex Array");

        glBindVertexArray(m_RendererId);
        indexBuffer->Bind();
        
        m_IndexBuffer = indexBuffer;
    }

}