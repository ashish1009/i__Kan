// ******************************************************************************
// File         : VertexArray.cpp
// Description  : Creating Instance of vertex Array
// Project      : iKan : Renderer
//
// Created by Ashish on 27/04/21.
// Copyright © 2021 Ashish. All rights reserved.
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
            case RendererAPI::API::None   : IK_CORE_ASSERT(false, "Render API not Supporting");
            case RendererAPI::API::OpenGL : return CreateRef<OpenGLVertexArray>(); break;
        }
        IK_CORE_ASSERT(false, "Invalid Render API ");
        return nullptr;
    }
    
}
