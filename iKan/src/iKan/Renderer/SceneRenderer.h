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

namespace iKan {
    
    class SceneRenderer
    {
    public:
        static void Init();
        static void Shutdown();
        
        static void BeginScene(const EditorCamera& camera);
        static void BeginScene(const Camera& camera, const glm::mat4& transform);
        static void EndScene();
    };
    
}
