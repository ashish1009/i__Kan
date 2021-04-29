// ******************************************************************************
// File         : SceneRenderer.cpp
// Project      : iKan : Renderer
// Description  : API Wrapper for rendering at Client
//
// Created on   : 28/04/21.
// ******************************************************************************

#include "SceneRenderer.h"
#include <iKan/Renderer/VertexArray.h>
#include <iKan/Renderer/Shader.h>
#include <iKan/Renderer/Texture.h>

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
        Ref<Texture>      WhiteTexture;
        
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
    void SceneRenderer::Init()
    {
        IK_CORE_INFO("Initialising the Scene Renderer");
        
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
        s_Data->WhiteTexture      = Texture::Create(1, 1, &whiteTextureData, sizeof(uint32_t));

        // Set the Texture slot 0 as White Texture
        s_Data->TextureSlots[0] = s_Data->WhiteTexture;

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
    void SceneRenderer::SetShaader(const std::string &path)
    {
        IK_CORE_INFO("Set the 2D Renderer Shader {0}", path.c_str());

        // Creating array of Slots to store hem in shader
        int32_t samplers[s_Data->MaxTextureSlots];
        for (uint32_t i = 0; i < s_Data->MaxTextureSlots; i++)
        {
            samplers[i] = i;
        }
        
        // Creating Shader and storing all the slots
        s_Data->TextureShader = Shader::Create(path);
        s_Data->TextureShader->Bind();
        s_Data->TextureShader->SetIntArray("u_Textures", samplers, s_Data->MaxTextureSlots);
    }
    
    // ******************************************************************************
    // Shut down the Scene Renderer
    // ******************************************************************************
    void SceneRenderer::Shutdown()
    {
        IK_CORE_WARN("Shutting down the Scene Renderer");
    }
    
}
