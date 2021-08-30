// ******************************************************************************
// File         : Renderer2D.CPP
// Description  : API Wrapper for rendering 2D at Client
// Project      : iKan : Renderer
//
// Created by Ashish on 05/08/21.
// Copyright © 2021 Ashish. All rights reserved.
// ******************************************************************************

#include "Renderer2D.h"
#include <iKan/Renderer/VertexArray.h>
#include <iKan/Renderer/Shader.h>
#include <iKan/Renderer/Renderer.h>
#include <iKan/Renderer/RendererStats.h>

namespace iKan {

    // ******************************************************************************
    // Stores the 2D Renderer information. All data and buffer pointers
    // ******************************************************************************
    struct RendererQuadData
    {
        // ******************************************************************************
        // Stores the vertex information of a Quad
        // ******************************************************************************
        struct Vertex
        {
            glm::vec3 Position;
            glm::vec4 Color;
            glm::vec2 TexCoord;

            float TexIndex;
            float TilingFactor;

            int32_t ObjectID;
        };

        // Consts to store limits of renderer
        static const uint32_t MaxQuads        = 20000;
        static const uint32_t MaxVertices     = MaxQuads * 4;
        static const uint32_t MaxIndices      = MaxQuads * 6;
        static const uint32_t MaxTextureSlots = 16;

        // Data storage for Rendering
        Ref<VertexArray>  QuadVertexArray;
        Ref<VertexBuffer> QuadVertexBuffer;
        Ref<Shader>       TextureShader;

        uint32_t QuadIndexCount = 0;

        // Pointer attribute of vertexes
        Vertex* QuadVertexBufferBase = nullptr;
        Vertex* QuadVertexBufferPtr  = nullptr;

        // array of textures for now 16 slots are possible
        std::array<Ref<Texture>, MaxTextureSlots> TextureSlots;
        uint32_t TextureSlotIndex = 1; /* 0 = white texture */

        // Basic vertex of quad
        glm::vec4 QuadVertexPositions[4];

        RendererQuadData() = default;
        ~RendererQuadData()
        {
            IK_CORE_WARN("Renderer2D Data instance Destryoyed and cleared the data");
            delete [] QuadVertexBufferBase;
        }
    };
    static RendererQuadData* s_Data = new RendererQuadData();

    // ******************************************************************************
    // Initialise the Scene Renderer
    // ******************************************************************************
    void Renderer2D::Init()
    {
        IK_CORE_INFO("Initialising the Renderer 2D");

        s_Data->QuadVertexArray  = VertexArray::Create();

        // Vertex Buffer and adding the layput
        s_Data->QuadVertexBuffer = VertexBuffer::Create(s_Data->MaxVertices * sizeof(RendererQuadData::Vertex));
        s_Data->QuadVertexBuffer->AddLayout ({
            { ShaderDataType::Float3, "a_Position" },
            { ShaderDataType::Float4, "a_Color" },
            { ShaderDataType::Float2, "a_TexCoord" },
            { ShaderDataType::Float,  "a_TexIndex" },
            { ShaderDataType::Float,  "a_TilingFactor" },
            { ShaderDataType::Int,    "a_ObjectID" }
        });
        s_Data->QuadVertexArray->AddVertexBuffer(s_Data->QuadVertexBuffer);

        // Allocating the memory for vertex Buffer Pointer
        s_Data->QuadVertexBufferBase = new RendererQuadData::Vertex[s_Data->MaxVertices];

        // Index Buffer
        uint32_t* quadIndices = new uint32_t[s_Data->MaxIndices];

        uint32_t offset = 0;
        for (uint32_t i = 0; i < s_Data->MaxIndices; i += 6)
        {
            quadIndices[i + 0] = offset + 0;
            quadIndices[i + 1] = offset + 1;
            quadIndices[i + 2] = offset + 2;

            quadIndices[i + 3] = offset + 2;
            quadIndices[i + 4] = offset + 3;
            quadIndices[i + 5] = offset + 0;

            offset += 4;
        }

        Ref<IndexBuffer> quadIB = IndexBuffer::Create(s_Data->MaxIndices, quadIndices);
        s_Data->QuadVertexArray->SetIndexBuffer(quadIB);
        delete[] quadIndices;

        // Creating white texture for colorful quads witout any texture or sprite
        uint32_t whiteTextureData = 0xffffffff;
        s_Data->TextureSlots[0]   = Texture::Create(1, 1, &whiteTextureData, sizeof(uint32_t));

        // Setting basic Vertex point of quad
        s_Data->QuadVertexPositions[0] = { -0.5f, -0.5f, 0.0f, 1.0f };
        s_Data->QuadVertexPositions[1] = {  0.5f, -0.5f, 0.0f, 1.0f };
        s_Data->QuadVertexPositions[2] = {  0.5f,  0.5f, 0.0f, 1.0f };
        s_Data->QuadVertexPositions[3] = { -0.5f,  0.5f, 0.0f, 1.0f };

        SetShaader("../../../iKan/assets/shaders/BatchRenderer2DShader.glsl");
    }

    // ******************************************************************************
    // Set the 2DD shader
    // ******************************************************************************
    void Renderer2D::SetShaader(const std::string &path)
    {
        IK_CORE_INFO("Set the 2D Renderer Shader {0}", path.c_str());

        // Creating array of Slots to store hem in shader
        int32_t samplers[s_Data->MaxTextureSlots];
        for (uint32_t i = 0; i < s_Data->MaxTextureSlots; i++)
            samplers[i] = i;

        // Creating Shader and storing all the slots
        s_Data->TextureShader = Shader::Create(path);
        s_Data->TextureShader->Bind();
        s_Data->TextureShader->SetIntArray("u_Textures", samplers, s_Data->MaxTextureSlots);
    }

    // ******************************************************************************
    // Shut down the Scene Renderer
    // ******************************************************************************
    void Renderer2D::Shutdown()
    {
        IK_CORE_WARN("Shutting down the Renderer 2D");
        delete s_Data;
    }

    // ******************************************************************************
    // Begin the 2D Scene
    // ******************************************************************************
    void Renderer2D::BeginScene(const glm::mat4& viewProj)
    {
        s_Data->TextureShader->Bind();
        s_Data->TextureShader->SetUniformMat4("u_ViewProjection", viewProj);

        StartBatch();
    }

    // ******************************************************************************
    // Start the batch Renderer
    // ******************************************************************************
    void Renderer2D::StartBatch()
    {
        s_Data->QuadIndexCount = 0;
        s_Data->QuadVertexBufferPtr = s_Data->QuadVertexBufferBase;
        s_Data->TextureSlotIndex = 1;
    }

    // ******************************************************************************
    // End the 2D Scene
    // ******************************************************************************
    void Renderer2D::EndScene()
    {
        Flush();
    }

    // ******************************************************************************
    // Flush the batch
    // ******************************************************************************
    void Renderer2D::Flush()
    {
        // Nothing to draw
        if (s_Data->QuadIndexCount == 0)
            return;

        uint32_t dataSize = (uint32_t)((uint8_t*)s_Data->QuadVertexBufferPtr - (uint8_t*)s_Data->QuadVertexBufferBase);
        s_Data->QuadVertexBuffer->SetData(s_Data->QuadVertexBufferBase, dataSize);

        // Bind textures
        for (uint32_t i = 0; i < s_Data->TextureSlotIndex; i++)
        {
            // Skipping i = 0 as 0 is slot for white texture
            if (i > 0)
                RendererStatistics::TextureCount++;

            s_Data->TextureSlots[i]->Bind(i);
        }

        // Render the Scene
        Renderer::DrawIndexed(s_Data->QuadVertexArray, s_Data->QuadIndexCount);
    }

    // ******************************************************************************
    // New batch
    // ******************************************************************************
    void Renderer2D::NextBatch()
    {
        // if num Quad per Batch exceeds then Render the Scene and reset all parameters
        EndScene();

        s_Data->QuadIndexCount = 0;
        s_Data->QuadVertexBufferPtr = s_Data->QuadVertexBufferBase;

        s_Data->TextureSlotIndex = 1;
    }

    // ******************************************************************************
    // Draq Color Quad
    // ******************************************************************************
    void Renderer2D::DrawQuad(const glm::mat4& transform, const glm::vec4& color, int32_t entID)
    {
        constexpr glm::vec2 textureCoords[] = { { 0.0f, 0.0f }, { 1.0f, 0.0f }, { 1.0f, 1.0f }, { 0.0f, 1.0f } };
        DrawTextureQuad(transform, nullptr, entID, textureCoords, 1.0f, color);
    }

    // ******************************************************************************
    // Draq Texture Quad
    // ******************************************************************************
    void Renderer2D::DrawQuad(const glm::mat4& transform, const Ref<Texture>& texture, int32_t entID, float tilingFactor, const glm::vec4& tintColor)
    {
        constexpr glm::vec2 textureCoords[] = { { 0.0f, 0.0f }, { 1.0f, 0.0f }, { 1.0f, 1.0f }, { 0.0f, 1.0f } };
        DrawTextureQuad(transform, texture, entID, textureCoords, tilingFactor, tintColor);
    }

    // ******************************************************************************
    // Draq Subtexutre Quad
    // ******************************************************************************
    void Renderer2D::DrawQuad(const glm::mat4& transform, const Ref<SubTexture>& subTexture, int32_t entID, float tilingFactor, const glm::vec4& tintColor)
    {
        const glm::vec2* textureCoords = subTexture->GetTexCoord();
        DrawTextureQuad(transform, subTexture->GetTexture(), entID, textureCoords, tilingFactor, tintColor);
    }

    void Renderer2D::DrawTextureQuad(const glm::mat4& transform, const Ref<Texture>& texture, int32_t entID, const glm::vec2* textureCoords, float tilingFactor, const glm::vec4& tintColor)
    {
        // If number of indices increase in batch then start new batch
        if (s_Data->QuadIndexCount >= RendererQuadData::MaxIndices)
        {
            IK_CORE_WARN("Starts the new batch as number of indices ({0}) increases in the previous batch", s_Data->QuadIndexCount);
            NextBatch();
        }

        float textureIndex = 0.0f;
        if (texture)
        {
            // Find if texture is already loaded in current batch
            for (int8_t i = 1; i < s_Data->TextureSlotIndex; i++)
            {
                if (*s_Data->TextureSlots[i] == *texture)
                {
                    // Found the current textue in the batch
                    textureIndex = (float)i;
                    break;
                }
            }

            // If current texture slot is not pre loaded then load the texture in proper slot
            if (textureIndex == 0.0f)
            {
                // If number of slots increases max then start new batch
                if (s_Data->TextureSlotIndex >= RendererQuadData::MaxTextureSlots)
                {
                    IK_CORE_WARN("Starts the new batch as number of texture slot ({0}) increases in the previous batch", s_Data->TextureSlotIndex);
                    NextBatch();
                }

                // Loading the current texture in the first free slot slot
                textureIndex = (float)s_Data->TextureSlotIndex;
                s_Data->TextureSlots[s_Data->TextureSlotIndex] = texture;
                s_Data->TextureSlotIndex++;
            }
        }

        constexpr size_t quadVertexCount = 4;
        for (size_t i = 0; i < quadVertexCount; i++)
        {
            s_Data->QuadVertexBufferPtr->Position     = transform * s_Data->QuadVertexPositions[i];
            s_Data->QuadVertexBufferPtr->Color        = tintColor;
            s_Data->QuadVertexBufferPtr->TexCoord     = textureCoords[i];
            s_Data->QuadVertexBufferPtr->TexIndex     = textureIndex;
            s_Data->QuadVertexBufferPtr->TilingFactor = tilingFactor;
            s_Data->QuadVertexBufferPtr->ObjectID     = entID;
            s_Data->QuadVertexBufferPtr++;
        }

        s_Data->QuadIndexCount += 6;

        RendererStatistics::VertexCount += 4;
        RendererStatistics::IndexCount += 6;
    }

}
