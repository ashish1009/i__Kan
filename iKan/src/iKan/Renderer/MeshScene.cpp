// ******************************************************************************
// File         : MeshScene.CPP
// Description  : API Wrapper for rendering Mesh at Client
// Project      : iKan : Renderer
//
// Created by Ashish on 06/08/21.
// Copyright © 2021 Ashish. All rights reserved.
// ******************************************************************************

#include "MeshScene.h"
#include <iKan/Scene/Scene.h>
#include <iKan/Renderer/SceneRenderer.h>
#include <iKan/Renderer/FrameBuffer.h>

namespace iKan {
    
    struct MeshSceneData
    {
        struct SceneInfo
        {
            const Scene* ActiveScene = nullptr;
            SceneRendererCamera SceneCamera;
        };
        SceneInfo SceneData;
        
        Ref<Framebuffer> GeometryFb;
        Ref<Framebuffer> CompositeFb;
    };
    static MeshSceneData s_Data;

    // ******************************************************************************
    // Initialise the Scene Renderer
    // ******************************************************************************
    void MeshScene::Init()
    {
        IK_CORE_INFO("Initialising the Renderer 3D");
        
        // Frame buffer Common specifications
        Framebuffer::Specification specs;
        specs.Width   = 1280;
        specs.Height  = 720;
        specs.Samples = 8;
        
        // Geomatry Framebuffer
        specs.ClearColor  = { 0.1f, 0.1f, 0.1f, 1.0f };
        specs.Attachments = { Framebuffer::TextureSpecification::TextureFormat::RGBA16F };
        s_Data.GeometryFb = Framebuffer::Create(specs);

        // Composite Framebuffer
        specs.ClearColor  = { 0.5f, 0.1f, 0.1f, 1.0f };
        specs.Attachments  = { Framebuffer::TextureSpecification::TextureFormat::RGBA8 };
        s_Data.CompositeFb = Framebuffer::Create(specs);
    }
    
    // ******************************************************************************
    // Update Mesh Scene on resize
    // ******************************************************************************
    void MeshScene::SetViewportSize(uint32_t width, uint32_t height)
    {
        s_Data.GeometryFb->Resize(width, height);
        s_Data.CompositeFb->Resize(width, height);
    }

    // ******************************************************************************
    // Shut down the Scene Renderer
    // ******************************************************************************
    void MeshScene::Shutdown()
    {
        IK_CORE_WARN("Shutting down the Renderer 3D");
    }

    // ******************************************************************************
    // Begin the 3D Scene
    // ******************************************************************************
    void MeshScene::BeginScene()
    {

    }

    // ******************************************************************************
    // End the 3D Scene
    // ******************************************************************************
    void MeshScene::EndScene()
    {

    }

}
