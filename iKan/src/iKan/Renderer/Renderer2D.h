// ******************************************************************************
// File         : Renderer2D.h
// Description  : API Wrapper for rendering 2D at Client
// Project      : iKan : Renderer
//
// Created by Ashish on 05/08/21.
// Copyright © 2021 Ashish. All rights reserved.
// ******************************************************************************

#pragma once

#include <iKan/Editor/EditorCamera.h>
#include <iKan/Renderer/Texture.h>

namespace iKan {

    class Renderer2D
    {
    public:
        static void Init();
        static void Shutdown();

        static void SetShaader(const std::string &path);

        static void BeginScene(const glm::mat4& viewProj);
        static void EndScene();

        // Primitives
        static void DrawTextureQuad(const glm::mat4& transform, const Ref<Texture>& texture, int32_t entID, const glm::vec2* textureCoords, float tilingFactor = 1.0f, const glm::vec4& tintColor = glm::vec4(1.0f));
        static void DrawQuad(const glm::mat4& transform, const glm::vec4& color, int32_t entID);
        static void DrawQuad(const glm::mat4& transform, const Ref<Texture>& texture, int32_t entID, float tilingFactor = 1.0f, const glm::vec4& tintColor = glm::vec4(1.0f));
        static void DrawQuad(const glm::mat4& transform, const Ref<SubTexture>& texture, int32_t entID, float tilingFactor = 1.0f, const glm::vec4& tintColor = glm::vec4(1.0f));

        static void DrawCircle(const glm::mat4& transform, const glm::vec4& color, float thickness = 1.0f, float fade = 0.005, int32_t entID = -1);

    private:
        static void Flush();
        static void StartBatch();
        static void NextBatch();

    };

}
