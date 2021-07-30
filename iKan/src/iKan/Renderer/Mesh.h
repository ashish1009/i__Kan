// ******************************************************************************
// File         : Mesh.h
// Description  : Mesh Renderer Class
// Projecte     : iKan : Renderer
//
// Created by Ashish on 05/07/21.
// Copyright © 2021 Ashish. All rights reserved.
// ******************************************************************************

#pragma once

#include <iKan/Core/Math/AABB.h>

struct aiScene;

namespace Assimp {
    class Importer;
}

namespace iKan {

    // ******************************************************************************
    // Mesh Class Defination. This will store all the renderer component of Mesh
    // ******************************************************************************
    class Mesh
    {
    public:
        Mesh(const std::string& path);
        ~Mesh();

    private:
        std::string m_Path;
        Scope<Assimp::Importer> m_Importer;
        const aiScene* m_Scene;
    };

}
