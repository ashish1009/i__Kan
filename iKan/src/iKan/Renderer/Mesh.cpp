// ******************************************************************************
// File         : Mesh.cpp
// Description  : Mesh Renderer Class Implementation
// Projecte     : iKan : Renderer
//
// Created by Ashish on 05/07/21.
// Copyright © 2021 Ashish. All rights reserved.
// ******************************************************************************

#include "Mesh.h"

#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/Importer.hpp>
#include <assimp/DefaultLogger.hpp>
#include <assimp/LogStream.hpp>

namespace iKan {

    // ******************************************************************************
    // Flags set to load the Mesh
    // *****************************************************************************
    static const uint32_t s_MeshImportFlags =
                                                aiProcess_CalcTangentSpace |        // Create binormals/tangents just in case
                                                aiProcess_Triangulate |             // Make sure we're triangles
                                                aiProcess_SortByPType |             // Split meshes by primitive type
                                                aiProcess_GenNormals |              // Make sure we have legit normals
                                                aiProcess_GenUVCoords |             // Convert UVs if required
                                                aiProcess_OptimizeMeshes |          // Batch draws where possible
                                                aiProcess_ValidateDataStructure;    // Validation


    // ******************************************************************************
    // Derived Logstream class for Assim Logger
    // *****************************************************************************
    struct LogStream : public Assimp::LogStream
    {
        // ******************************************************************************
        // Initialise the assimp logger
        // ******************************************************************************
        static void Initialize()
        {
            if (Assimp::DefaultLogger::isNullLogger())
            {
                Assimp::DefaultLogger::create("", Assimp::Logger::VERBOSE);
                Assimp::DefaultLogger::get()->attachStream(new LogStream, Assimp::Logger::Err | Assimp::Logger::Warn);
            }
        }

        // ******************************************************************************
        // Write the log (IK_LOG)
        // ******************************************************************************
        virtual void write(const char* message) override
        {
            IK_CORE_ERROR("Assimp error: {0}", message);
        }
    };

    // ******************************************************************************
    // Mesh constructor
    // ******************************************************************************
    Mesh::Mesh(const std::string &path)
    : m_Path(path)
    {
        LogStream::Initialize();

        IK_CORE_INFO("Loading Mesh: {0}", m_Path);

        m_Importer = CreateScope<Assimp::Importer>();

        m_Scene = m_Importer->ReadFile(m_Path, s_MeshImportFlags);
        if (!m_Scene || !m_Scene->HasMeshes())
            IK_CORE_ERROR("Failed to load mesh file: {0}", m_Path);
    }

    // ******************************************************************************
    // Mesh Destructor
    // ******************************************************************************
    Mesh::~Mesh()
    {
        IK_CORE_INFO("Mesh from path {0} Destroyed", m_Path);
    }

}
