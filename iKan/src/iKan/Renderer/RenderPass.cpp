// ******************************************************************************
// File         : RendererPass.h
// Description  : Renderer API Visible to Core and client
// Project      : iKan : Renderer
//
// Created by Ashish on 12/07/21.
// Copyright © 2021 Ashish. All rights reserved.
// ******************************************************************************

#include "RenderPass.h"
#include <iKan/Renderer/Renderer.h>
#include <iKan/Platform/OpenGL/OpenGLRenderPass.h>

namespace iKan {

    Ref<RenderPass> RenderPass::Create(const RenderPassSpecification& spec)
    {
        switch (Renderer::GetAPI())
        {
            case RendererAPI::API::None   : IK_CORE_ASSERT(false, "Render API not Supporting");
            case RendererAPI::API::OpenGL : return CreateRef<OpenGLRenderPass>(spec); break;
        }
        IK_CORE_ASSERT(false, "Unknown RendererAPI!");
        return nullptr;
    }

}
