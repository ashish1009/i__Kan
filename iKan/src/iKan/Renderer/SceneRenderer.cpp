// ******************************************************************************
// File         : SceneRenderer.cpp
// Description  : API Wrapper for rendering at Client
// Project      : iKan : Renderer
//
// Created by Ashish on 28/04/21.
// Copyright © 2021 Ashish. All rights reserved.
// ******************************************************************************

#include "SceneRenderer.h"
#include <iKan/Renderer/VertexArray.h>
#include <iKan/Renderer/Shader.h>
#include <iKan/Renderer/Renderer.h>
#include <iKan/Renderer/RendererStats.h>
#include <iKan/Renderer/Material.h>
#include <iKan/Renderer/Mesh.h>
#include <iKan/Scene/Scene.h>

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
        Ref<VertexArray>   QuadVertexArray;
        Ref<VertexBuffer>  QuadVertexBuffer;
        Ref<Texture>       WhiteTexture;
        Ref<ShaderLibrary> ShaderLibrary;
        
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

    // ******************************************************************************
    // Stores the Scene Data Majorly Mesh. All data and buffer pointers
    // ******************************************************************************
    struct SceneData
    {
        // ******************************************************************************
        // Stores the Scene Information
        // ******************************************************************************
        struct SceneInfo
        {
            SceneRendererCamera   SceneCamera;
            Ref<MaterialInstance> SkyboxMaterial;
        } SceneData;
        
        const Scene* ActiveScene = nullptr;

        struct DrawCommand
        {
            Ref<Mesh> Mesh;
            Ref<MaterialInstance> Material;
            glm::mat4 Transform;
        };
        std::vector<DrawCommand> DrawList;
    };

    static RendererQuadData* s_Renderer2DData = new RendererQuadData();
    static SceneData* s_SceneData = new SceneData();
    
    // ******************************************************************************
    // Initialise the 2D Data
    // ******************************************************************************
    void SceneRenderer::Init2DData()
    {
        IK_CORE_INFO("Initialising the Scene Renderer");
        
        s_Renderer2DData->QuadVertexArray  = VertexArray::Create();
        
        // Vertex Buffer and adding the layput
        s_Renderer2DData->QuadVertexBuffer = VertexBuffer::Create(s_Renderer2DData->MaxVertices * sizeof(RendererQuadData::Vertex));
        s_Renderer2DData->QuadVertexBuffer->AddLayout ({
            { ShaderDataType::Float3, "a_Position" },
            { ShaderDataType::Float4, "a_Color" },
            { ShaderDataType::Float2, "a_TexCoord" },
            { ShaderDataType::Float,  "a_TexIndex" },
            { ShaderDataType::Float,  "a_TilingFactor" },
            { ShaderDataType::Int,    "a_ObjectID" }
        });
        s_Renderer2DData->QuadVertexArray->AddVertexBuffer(s_Renderer2DData->QuadVertexBuffer);
        
        // Allocating the memory for vertex Buffer Pointer
        s_Renderer2DData->QuadVertexBufferBase = new RendererQuadData::Vertex[s_Renderer2DData->MaxVertices];
        
        // Index Buffer
        uint32_t* quadIndices = new uint32_t[s_Renderer2DData->MaxIndices];
        
        uint32_t offset = 0;
        for (uint32_t i = 0; i < s_Renderer2DData->MaxIndices; i += 6)
        {
            quadIndices[i + 0] = offset + 0;
            quadIndices[i + 1] = offset + 1;
            quadIndices[i + 2] = offset + 2;
            
            quadIndices[i + 3] = offset + 2;
            quadIndices[i + 4] = offset + 3;
            quadIndices[i + 5] = offset + 0;
            
            offset += 4;
        }
        
        Ref<IndexBuffer> quadIB = IndexBuffer::Create(s_Renderer2DData->MaxIndices, quadIndices);
        s_Renderer2DData->QuadVertexArray->SetIndexBuffer(quadIB);
        delete[] quadIndices;

        // Creating white texture for colorful quads witout any texture or sprite
        uint32_t whiteTextureData = 0xffffffff;
        s_Renderer2DData->WhiteTexture      = Texture::Create(1, 1, &whiteTextureData, sizeof(uint32_t));

        // Set the Texture slot 0 as White Texture
        s_Renderer2DData->TextureSlots[0] = s_Renderer2DData->WhiteTexture;

        // Setting basic Vertex point of quad
        s_Renderer2DData->QuadVertexPositions[0] = { -0.5f, -0.5f, 0.0f, 1.0f };
        s_Renderer2DData->QuadVertexPositions[1] = {  0.5f, -0.5f, 0.0f, 1.0f };
        s_Renderer2DData->QuadVertexPositions[2] = {  0.5f,  0.5f, 0.0f, 1.0f };
        s_Renderer2DData->QuadVertexPositions[3] = { -0.5f,  0.5f, 0.0f, 1.0f };

        s_Renderer2DData->ShaderLibrary = CreateRef<ShaderLibrary>();
        
        AddShaader("../../../iKan/assets/shaders/BatchRenderer2DShader.glsl");
        AddShaader("../../../iKan/assets/shaders/PBR_Static.glsl");
//        AddShaader("../../../iKan/assets/shaders/PBR_Anim.glsl");
    }

    // ******************************************************************************
    // Initialise the Scene Data
    // ******************************************************************************
    void SceneRenderer::InitSceneData()
    {
    }

    // ******************************************************************************
    // Initialise the All data
    // ******************************************************************************
    void SceneRenderer::Init()
    {
        Init2DData();
        InitSceneData();
    }
    
    // ******************************************************************************
    // Set the 2DD shader
    // ******************************************************************************
    void SceneRenderer::AddShaader(const std::string &path)
    {
        IK_CORE_INFO("Set the 2D Renderer Shader {0}", path.c_str());

        // Creating array of Slots to store hem in shader
        int32_t samplers[s_Renderer2DData->MaxTextureSlots];
        for (uint32_t i = 0; i < s_Renderer2DData->MaxTextureSlots; i++)
        {
            samplers[i] = i;
        }
        
        // Creating Shader and storing all the slots
        auto textureShader = s_Renderer2DData->ShaderLibrary->Load(path);

        textureShader->Bind();
        textureShader->SetIntArray("u_Textures", samplers, s_Renderer2DData->MaxTextureSlots);
    }
    
    // ******************************************************************************
    // Shut down the Scene Renderer
    // ******************************************************************************
    void SceneRenderer::Shutdown()
    {
        IK_CORE_WARN("Shutting down the Scene Renderer");
        delete s_Renderer2DData;
    }
    
    // ******************************************************************************
    // Begin the 2D Scene
    // ******************************************************************************
    void SceneRenderer::BeginScene(const EditorCamera& camera)
    {
        glm::mat4 viewProj = camera.GetViewProjection();

        auto textureShader = s_Renderer2DData->ShaderLibrary->Get("BatchRenderer2DShader");

        textureShader->Bind();
        textureShader->SetUniformMat4("u_ViewProjection", viewProj);

        StartBatch();
    }

    // ******************************************************************************
    // Begin the 2D Scene
    // ******************************************************************************
    void SceneRenderer::BeginScene(const Camera& camera, const glm::mat4& transform)
    {
        // Upload Camera View Projection Matris to shader
        glm::mat4 viewProj = camera.GetProjection() * glm::inverse(transform);

        auto textureShader = s_Renderer2DData->ShaderLibrary->Get("BatchRenderer2DShader");

        textureShader->Bind();
        textureShader->SetUniformMat4("u_ViewProjection", viewProj);

        StartBatch();
    }
    
    // ******************************************************************************
    // Start the batch Renderer
    // ******************************************************************************
    void SceneRenderer::StartBatch()
    {
        s_Renderer2DData->QuadIndexCount = 0;
        s_Renderer2DData->QuadVertexBufferPtr = s_Renderer2DData->QuadVertexBufferBase;
        
        s_Renderer2DData->TextureSlotIndex = 1;
    }
    
    // ******************************************************************************
    // End the 2D Scene
    // ******************************************************************************
    void SceneRenderer::EndScene()
    {
        Flush();
    }
    
    // ******************************************************************************
    // Flush the batch
    // ******************************************************************************
    void SceneRenderer::Flush()
    {
        // Nothing to draw
        if (s_Renderer2DData->QuadIndexCount == 0)
        {
            return;
        }
        
        uint32_t dataSize = (uint32_t)((uint8_t*)s_Renderer2DData->QuadVertexBufferPtr - (uint8_t*)s_Renderer2DData->QuadVertexBufferBase);
        s_Renderer2DData->QuadVertexBuffer->SetData(s_Renderer2DData->QuadVertexBufferBase, dataSize);
        
        // Bind textures
        for (uint32_t i = 0; i < s_Renderer2DData->TextureSlotIndex; i++)
        {
            if (i > 0)
            {
                // Skipping i = 0 as 0 is slot for white texture
                RendererStatistics::TextureCount++;
            }

            s_Renderer2DData->TextureSlots[i]->Bind(i);
        }
        
        // Render the Scene
        Renderer::DrawIndexed(s_Renderer2DData->QuadVertexArray, s_Renderer2DData->QuadIndexCount);
    }
    
    // ******************************************************************************
    // New batch
    // ******************************************************************************
    void SceneRenderer::NextBatch()
    {
        // if num Quad per Batch exceeds then Render the Scene and reset all parameters
        EndScene();
        
        s_Renderer2DData->QuadIndexCount = 0;
        s_Renderer2DData->QuadVertexBufferPtr = s_Renderer2DData->QuadVertexBufferBase;
        
        s_Renderer2DData->TextureSlotIndex = 1;
    }

    // ******************************************************************************
    // Get the shader library
    // ******************************************************************************
    Ref<ShaderLibrary> SceneRenderer::GetShaderLibrary()
    {
        return s_Renderer2DData->ShaderLibrary;
    }
    
    // ******************************************************************************
    // Draq Color Quad
    // ******************************************************************************
    void SceneRenderer::DrawQuad(const glm::mat4& transform, const glm::vec4& color, int32_t entID)
    {
        constexpr glm::vec2 textureCoords[] = { { 0.0f, 0.0f }, { 1.0f, 0.0f }, { 1.0f, 1.0f }, { 0.0f, 1.0f } };
        DrawTextureQuad(transform, nullptr, entID, textureCoords, 1.0f, color);
    }
    
    // ******************************************************************************
    // Draq Texture Quad
    // ******************************************************************************
    void SceneRenderer::DrawQuad(const glm::mat4& transform, const Ref<Texture>& texture, int32_t entID, float tilingFactor, const glm::vec4& tintColor)
    {
        constexpr glm::vec2 textureCoords[] = { { 0.0f, 0.0f }, { 1.0f, 0.0f }, { 1.0f, 1.0f }, { 0.0f, 1.0f } };
        DrawTextureQuad(transform, texture, entID, textureCoords, tilingFactor, tintColor);
    }
    
    // ******************************************************************************
    // Draq Subtexutre Quad
    // ******************************************************************************
    void SceneRenderer::DrawQuad(const glm::mat4& transform, const Ref<SubTexture>& subTexture, int32_t entID, float tilingFactor, const glm::vec4& tintColor)
    {
        const glm::vec2* textureCoords   = subTexture->GetTexCoord();
        DrawTextureQuad(transform, subTexture->GetTexture(), entID, textureCoords, tilingFactor, tintColor);
    }
    
    void SceneRenderer::DrawTextureQuad(const glm::mat4& transform, const Ref<Texture>& texture, int32_t entID, const glm::vec2* textureCoords, float tilingFactor, const glm::vec4& tintColor)
    {
        constexpr size_t quadVertexCount = 4;

        // If number of indices increase in batch then start new batch
        if (s_Renderer2DData->QuadIndexCount >= RendererQuadData::MaxIndices)
        {
            IK_CORE_WARN("Starts the new batch as number of indices ({0}) increases in the previous batch", s_Renderer2DData->QuadIndexCount);
            NextBatch();
        }
        
        float textureIndex = 0.0f;
        if (texture)
        {
            // Find if texture is already loaded in current batch
            for (int8_t i = 1; i < s_Renderer2DData->TextureSlotIndex; i++)
            {
                if (*s_Renderer2DData->TextureSlots[i] == *texture)
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
                if (s_Renderer2DData->TextureSlotIndex >= RendererQuadData::MaxTextureSlots)
                {
                    IK_CORE_WARN("Starts the new batch as number of texture slot ({0}) increases in the previous batch", s_Renderer2DData->TextureSlotIndex);
                    NextBatch();
                }

                // Loading the current texture in the first free slot slot
                textureIndex = (float)s_Renderer2DData->TextureSlotIndex;
                s_Renderer2DData->TextureSlots[s_Renderer2DData->TextureSlotIndex] = texture;
                s_Renderer2DData->TextureSlotIndex++;
            }
        }
        
        for (size_t i = 0; i < quadVertexCount; i++)
        {
            s_Renderer2DData->QuadVertexBufferPtr->Position     = transform * s_Renderer2DData->QuadVertexPositions[i];
            s_Renderer2DData->QuadVertexBufferPtr->Color        = tintColor;
            s_Renderer2DData->QuadVertexBufferPtr->TexCoord     = textureCoords[i];
            s_Renderer2DData->QuadVertexBufferPtr->TexIndex     = textureIndex;
            s_Renderer2DData->QuadVertexBufferPtr->TilingFactor = tilingFactor;
            s_Renderer2DData->QuadVertexBufferPtr->ObjectID     = entID;
            s_Renderer2DData->QuadVertexBufferPtr++;
        }
        
        s_Renderer2DData->QuadIndexCount += 6;

        RendererStatistics::VertexCount += 4;
        RendererStatistics::IndexCount += 6;
    }
    
}
