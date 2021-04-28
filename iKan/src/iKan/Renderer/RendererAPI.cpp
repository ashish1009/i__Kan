// ******************************************************************************
// File         : RendererAPI.h
// Description  : Interface for Graphics Renderer API
// Project      : iKan : Renderer
//
// Created on   : 28/04/21.
// ******************************************************************************

#include "RendererAPI.h"
#include <iKan/Platform/OpenGL/OpenGlRendererAPI.h>

namespace iKan {
    
    RendererAPI::API RendererAPI::s_API = RendererAPI::API::None;
    
    // ******************************************************************************
    // Create instacne of Renderer API
    // ******************************************************************************
    Scope<RendererAPI> RendererAPI::Create()
    {
        switch (s_API)
        {
            case API::None   : IK_CORE_ASSERT(false, "Render API not Supporting");
            case API::OpenGL : return CreateScope<OpenGlRendererAPI>(); break;
        }
        IK_CORE_ASSERT(false, "Invalid Render API ");
        return nullptr;
    }
    
}
