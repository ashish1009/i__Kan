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

namespace iKan {

    // ******************************************************************************
    // Stores the Scene Camera component
    // ******************************************************************************
    struct SceneRendererCamera
    {
        iKan::Camera Camera;
        glm::mat4 ViewMatrix;
    };
    
    // ******************************************************************************
    // Scene Renderer API
    // ******************************************************************************
    class SceneRenderer
    {
    public:
        static void Init();
        static void Shutdown();
        
        static void AddShaader(const std::string &path);
        
        static void BeginScene(const EditorCamera& camera);
        static void BeginScene(const Camera& camera, const glm::mat4& transform);
        static void EndScene();

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
        
        static void DrawTextureQuad(const glm::mat4& transform, const Ref<Texture>& texture, int32_t entID, const glm::vec2* textureCoords, float tilingFactor = 1.0f, const glm::vec4& tintColor = glm::vec4(1.0f));
    };
    
}
