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

namespace iKan {

    // ******************************************************************************
    // Initialise the Scene Renderer
    // ******************************************************************************
    void SceneRenderer::Init()
    {
        IK_CORE_INFO("Initialising the Scene RendererS");
        Renderer2D::Init();
    }

    // ******************************************************************************
    // Shut down the Scene Renderer
    // ******************************************************************************
    void SceneRenderer::Shutdown()
    {
        IK_CORE_WARN("Shutting down the Scene Renderer");
        Renderer2D::Shutdown();
    }
    
    // ******************************************************************************
    // Begin the 2D Scene
    // ******************************************************************************
    void SceneRenderer::BeginScene(const EditorCamera& camera)
    {
        Renderer2D::BeginScene(camera);
    }

    // ******************************************************************************
    // Begin the 2D Scene
    // ******************************************************************************
    void SceneRenderer::BeginScene(const Camera& camera, const glm::mat4& transform)
    {
        Renderer2D::BeginScene(camera, transform);
    }
    
    // ******************************************************************************
    // End the 2D Scene
    // ******************************************************************************
    void SceneRenderer::EndScene()
    {
        Renderer2D::EndScene();
    }

}
