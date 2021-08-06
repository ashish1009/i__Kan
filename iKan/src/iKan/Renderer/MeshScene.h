// ******************************************************************************
// File         : MeshScene.h
// Description  : API Wrapper for rendering Mesh at Client
// Project      : iKan : Renderer
//
// Created by Ashish on 06/08/21.
// Copyright © 2021 Ashish. All rights reserved.
// ******************************************************************************

#pragma once

#include <iKan/Editor/EditorCamera.h>
#include <iKan/Renderer/Texture.h>

namespace iKan {

    class MeshScene
    {
    public:
        static void Init();
        static void Shutdown();

        static void BeginScene();
        static void EndScene();
    };

}
