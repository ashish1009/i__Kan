// ******************************************************************************
// File         : SceneRenderer.cpp
// Description  : API Wrapper for rendering at Client
// Project      : iKan : Renderer
//
// Created by Ashish on 28/04/21.
// Copyright © 2021 Ashish. All rights reserved.
// ******************************************************************************

#include "SceneRenderer.h"
#include <iKan/Renderer/Renderer2D.h>
#include <iKan/Renderer/MeshScene.h>

namespace iKan {

    // ******************************************************************************
    // Initialise the Scene Renderer
    // ******************************************************************************
    void SceneRenderer::Init()
    {
        IK_CORE_INFO("Initialising the Scene RendererS");
        Renderer2D::Init();
        MeshScene::Init();
    }

    // ******************************************************************************
    // Shut down the Scene Renderer
    // ******************************************************************************
    void SceneRenderer::Shutdown()
    {
        IK_CORE_WARN("Shutting down the Scene Renderer");
        Renderer2D::Shutdown();
        MeshScene::Shutdown();
    }
    
    // ******************************************************************************
    // Begin the Scene
    // ******************************************************************************
    void SceneRenderer::BeginScene(const Scene* scene, const SceneRendererCamera& camera)
    {
        Renderer2D::BeginScene(camera.ViewMatrix);
        MeshScene::BeginScene();
    }
    
    // ******************************************************************************
    // End the Scene
    // ******************************************************************************
    void SceneRenderer::EndScene()
    {
        Renderer2D::EndScene();
        MeshScene::EndScene();
    }
    
    // ******************************************************************************
    // Update Scene on resize
    // ******************************************************************************
    void SceneRenderer::SetViewportSize(uint32_t width, uint32_t height)
    {
        MeshScene::SetViewportSize(width, height);
    }

}
