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

    // ******************************************************************************
    // Stores the Scene Renderer information.
    // ******************************************************************************
    struct SceneRendererData
    {
    public:
        // ******************************************************************************
        // Stores the Camera information bind to the active Scene
        // ******************************************************************************
        struct SceneCamera
        {
            iKan::Camera Camera;
            glm::mat4    ViewMatrix;

            SceneCamera() = default;
            ~SceneCamera() = default;
        };

    public:
        SceneCamera  Camera;
        const Scene* ActiveScene;

    public:
        SceneRendererData(const SceneRendererData&) = delete;
        SceneRendererData(SceneRendererData&&) = delete;

        SceneRendererData& operator=(const SceneRendererData&) = delete;
        SceneRendererData& operator=(SceneRendererData&&) = delete;

        ~SceneRendererData() = default;

        static SceneRendererData& Get()
        {
            static SceneRendererData data;
            return data;
        }

    private:
        SceneRendererData() = default;
    };

    // ******************************************************************************
    // Scene Renderer API
    // ******************************************************************************
    class SceneRenderer
    {
    public:
        static void Init();
        static void Shutdown();
        
        static void BeginScene(const Scene* scene, const SceneRendererData::SceneCamera& camera);
        static void EndScene();

        static const SceneRendererData& GetData();
    };
    
}
