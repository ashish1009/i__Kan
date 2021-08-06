// ******************************************************************************
// File         : SceneRenderer.h
// Description  : API Wrapper for rendering at Client
// Project      : iKan : Renderer
//
// Created by Ashish on 28/04/21.
// Copyright © 2021 Ashish. All rights reserved.
// ******************************************************************************

#pragma once

#include <iKan/Editor/EditorCamera.h>
#include <iKan/Scene/Scene.h>

namespace iKan {

    struct SceneRendererData;

    class SceneRenderer
    {
    public:
        // ******************************************************************************
        // Stores the Scene Renderer information.
        // ******************************************************************************
        struct Data
        {
        public:
            // ******************************************************************************
            // Stores the Camera information bind to the active Scene
            // ******************************************************************************
            struct CameraInfo
            {
                iKan::Camera Camera;
                glm::mat4    ViewMatrix;

                CameraInfo() = default;
                ~CameraInfo() = default;
            };

        public:
            CameraInfo Camera;
            Ref<Scene> ActiveScene;
        };

    public:
        static void Init();
        static void Shutdown();
        
        static void BeginScene(const SceneRenderer::Data::CameraInfo& camera);
        static void EndScene();

        static const Data& GetData();
    };
    
}
