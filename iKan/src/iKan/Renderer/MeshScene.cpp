// ******************************************************************************
// File         : MeshScene.CPP
// Description  : API Wrapper for rendering Mesh at Client
// Project      : iKan : Renderer
//
// Created by Ashish on 06/08/21.
// Copyright © 2021 Ashish. All rights reserved.
// ******************************************************************************

#include "MeshScene.h"

namespace iKan {

    // ******************************************************************************
    // Initialise the Scene Renderer
    // ******************************************************************************
    void MeshScene::Init()
    {
        IK_CORE_INFO("Initialising the Renderer 3D");

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
