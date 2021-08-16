// ******************************************************************************
// File         : SceneRenderer.h
// Description  : API Wrapper for rendering at Client
// Project      : iKan : Renderer
//
// Created by Ashish on 28/04/21.
// Copyright © 2021 Ashish. All rights reserved.
// ******************************************************************************

#pragma once

#include <iKan/Renderer/Camera.h>
#include <iKan/Scene/Scene.h>

namespace iKan {
    
    // ******************************************************************************
    // Stores the Camera information bind to the active Scene
    // ******************************************************************************
    struct SceneRendererCamera
    {
        iKan::Camera Camera;
        glm::mat4    ViewMatrix;
    };

    // ******************************************************************************
    // Scene Renderer API
    // ******************************************************************************
    class SceneRenderer
    {
    public:
        static void Init();
        static void Shutdown();
        
        static void BeginScene(const Scene* scene, const SceneRendererCamera& camera);
        static void EndScene();
        
        static void SetViewportSize(uint32_t width, uint32_t height);
    };
    
}
