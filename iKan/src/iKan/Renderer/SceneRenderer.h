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
#include <iKan/Renderer/Texture.h>
#include <iKan/Renderer/Shader.h>
#include <iKan/Renderer/Camera.h>
#include <iKan/Renderer/Mesh.h>
#include <iKan/Scene/Scene.h>

namespace iKan {

    // ******************************************************************************
    // Scene Renderer API
    // ******************************************************************************
    class SceneRenderer
    {
    public:
        // ******************************************************************************
        // Stores the Scene Camera component
        // ******************************************************************************
        struct Camera
        {
            iKan::Camera Camera;
            glm::mat4 ViewMatrix;
        };

        // ******************************************************************************
        // Flags
        // ******************************************************************************
        struct Options
        {
            bool ShowGrid = true;
            bool ShowBoundingBoxes = false;
        };

    public:
        static void Init();
        static void Shutdown();
        
        static void AddShaader(const std::string &path);
        
        static void BeginScene(const EditorCamera& camera);
        static void BeginScene(const iKan::Camera& camera, const glm::mat4& transform);
        static void EndScene();

        static void BeginMeshScene(const Scene* scene, const SceneRenderer::Camera& camera);
        static void EndMeshScene();

        static void SetViewportSize(uint32_t width, uint32_t height);
        static void SubmitMesh(Ref<Mesh> mesh, const glm::mat4& transform = glm::mat4(1.0f), Ref<MaterialInstance> overrideMaterial = nullptr);
        static void SubmitSelectedMesh(Ref<Mesh> mesh, const glm::mat4& transform = glm::mat4(1.0f));

        static void DrawLine(const glm::vec3& p0, const glm::vec3& p1, const glm::vec4& color = glm::vec4(1.0f));
        
        static Ref<ShaderLibrary> GetShaderLibrary();
        
        // Primitives
        static void DrawQuad(const glm::mat4& transform, const glm::vec4& color, int32_t entID);
        static void DrawQuad(const glm::mat4& transform, const Ref<Texture>& texture, int32_t entID, float tilingFactor = 1.0f, const glm::vec4& tintColor = glm::vec4(1.0f));
        static void DrawQuad(const glm::mat4& transform, const Ref<SubTexture>& texture, int32_t entID, float tilingFactor = 1.0f, const glm::vec4& tintColor = glm::vec4(1.0f));
        
    private:
        static void Init2DData();
        static void InitSceneData();
        static void Flush();
        static void StartBatch();
        static void NextBatch();

        static void FlushDrawMeshList();
        static void GeometryPass();
        static void CompositePass();

        static void DrawTextureQuad(const glm::mat4& transform, const Ref<Texture>& texture, int32_t entID, const glm::vec2* textureCoords, float tilingFactor = 1.0f, const glm::vec4& tintColor = glm::vec4(1.0f));
    };
    
}
