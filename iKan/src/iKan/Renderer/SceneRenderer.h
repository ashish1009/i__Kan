// ******************************************************************************
// File         : SceneRenderer.h
// Project      : iKan : Renderer
// Description  : API Wrapper for rendering at Client
//
// Created on   : 28/04/21.
// ******************************************************************************

#pragma once

#include <iKan/Editor/EditorCamera.h>
#include <iKan/Renderer/Texture.h>

namespace iKan {
    
    class SceneRenderer
    {
    public:
        static void Init();
        static void Shutdown();
        
        static void SetShaader(const std::string &path);
        
        static void BeginScene(const EditorCamera& camera);
        static void EndScene();
        
        // Primitives
        static void DrawQuad(const glm::mat4& transform, const glm::vec4& color, int32_t entID);
        static void DrawQuad(const glm::mat4& transform, const Ref<Texture>& texture, int32_t entID, float tilingFactor = 1.0f, const glm::vec4& tintColor = glm::vec4(1.0f));
        static void DrawQuad(const glm::mat4& transform, const Ref<SubTexture>& texture, int32_t entID, float tilingFactor = 1.0f, const glm::vec4& tintColor = glm::vec4(1.0f));
        
    private:
        static void Flush();
        static void StartBatch();
        static void NextBatch();
        
        static void DrawTextureQuad(const glm::mat4& transform, const Ref<Texture>& texture, int32_t entID, const glm::vec2* textureCoords, float tilingFactor = 1.0f, const glm::vec4& tintColor = glm::vec4(1.0f));
    };
    
}
