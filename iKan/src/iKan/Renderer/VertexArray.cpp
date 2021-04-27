// ******************************************************************************
// File         : VertexArray.cpp
// Project      : iKan : Renderer
// Description  : Creating Instance of vertex Array
//
// Created on   : 27/04/21.
// ******************************************************************************

#include "VertexArray.h"
#include <iKan/Renderer/Renderer.h>
#include <iKan/Platform/OpenGL/OpenGLVertexArray.h>

namespace iKan {
    
    // ******************************************************************************
    // creating instance of Verytex Array
    // ******************************************************************************
    Ref<VertexArray> VertexArray::Create()
    {
        switch (Renderer::GetAPI())
        {
            case Renderer::API::None   : IK_CORE_ASSERT(false, "Render API not Supporting");
            case Renderer::API::OpenGL : return CreateRef<OpenGLVertexArray>(); break;
        }
        IK_CORE_ASSERT(false, "Invalid Render API ");
        return nullptr;
    }
    
}
