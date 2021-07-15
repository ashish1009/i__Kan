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
#include <iKan/Renderer/Renderer.h>
#include <iKan/Renderer/RendererStats.h>
#include <iKan/Renderer/Material.h>
#include <iKan/Renderer/FrameBuffer.h>
#include <iKan/Renderer/RenderPass.h>

// TODO: Remove this from herez
#include <glad/glad.h>

namespace iKan {
        
    // ******************************************************************************
    // Stores the 2D Renderer information. All data and buffer pointers
    // ******************************************************************************
    struct Renderer2DData
    {
        // ******************************************************************************
        // Stores the vertex information of a Quad
        // ******************************************************************************
        struct QuadVertex
        {
            glm::vec3 Position;
            glm::vec4 Color;
            glm::vec2 TexCoord;

            float TexIndex;
            float TilingFactor;

            int32_t ObjectID;
        };

        // ******************************************************************************
        // Line vertex info
        // ******************************************************************************
        struct LineVertex
        {
            glm::vec3 Position;
            glm::vec4 Color;
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
        QuadVertex* QuadVertexBufferBase = nullptr;
        QuadVertex* QuadVertexBufferPtr  = nullptr;
        
        // array of textures for now 16 slots are possible
        std::array<Ref<Texture>, MaxTextureSlots> TextureSlots;
        uint32_t TextureSlotIndex = 1; /* 0 = white texture */
        
        // Basic vertex of quad
        glm::vec4 QuadVertexPositions[4];

        // Lines

        static const uint32_t MaxLines = 10000;
        static const uint32_t MaxLineVertices = MaxLines * 2;
        static const uint32_t MaxLineIndices = MaxLines * 6;

        Ref<VertexArray>    LineVertexArray;
        Ref<VertexBuffer>   LineVertexBuffer;
        Ref<IndexBuffer>    LineIndexBuffer;

        Ref<Shader> LineShader;

        uint32_t LineIndexCount = 0;
        LineVertex* LineVertexBufferBase = nullptr;
        LineVertex* LineVertexBufferPtr = nullptr;

        bool DepthTest = true;

        Renderer2DData() = default;
        ~Renderer2DData()
        {
            IK_CORE_WARN("Renderer2D Data instance Destryoyed and cleared the data");
            delete [] QuadVertexBufferBase;
        }
    };

    // ******************************************************************************
    // Stores the Scene Data Majorly Mesh. All data and buffer pointers
    // ******************************************************************************
    struct SceneRendererData
    {
        // ******************************************************************************
        // Stores the Scene Information
        // ******************************************************************************
        struct SceneInfo
        {
            SceneRenderer::Camera SceneCamera;
            Ref<MaterialInstance> SkyboxMaterial;
            Scene::Environment    SceneEnvironment;
            Scene::Light          ActiveLight;
        } SceneData;
        
        const Scene* ActiveScene = nullptr;

        Ref<Texture> BRDFLUT;
        Ref<Shader>  CompositeShader;

        Ref<RenderPass> GeoPass;
        Ref<RenderPass> CompositePass;

        struct DrawCommand
        {
            Ref<Mesh> Mesh;
            Ref<MaterialInstance> Material;
            glm::mat4 Transform;
        };
        std::vector<DrawCommand> DrawList;
        std::vector<DrawCommand> SelectedMeshDrawList;

        // Grid
        Ref<MaterialInstance> GridMaterial;
        Ref<MaterialInstance> OutlineMaterial;

        SceneRenderer::Options Options;
    };

    static Renderer2DData* s_Renderer2DData = new Renderer2DData();
    static SceneRendererData* s_SceneRendererData = new SceneRendererData();
    
    // ******************************************************************************
    // Initialise the 2D Data
    // ******************************************************************************
    void SceneRenderer::Init2DData()
    {
        IK_CORE_INFO("Initialising the Scene Renderer");
        
        s_Renderer2DData->QuadVertexArray  = VertexArray::Create();
        
        // Vertex Buffer and adding the layput
        s_Renderer2DData->QuadVertexBuffer = VertexBuffer::Create(s_Renderer2DData->MaxVertices * sizeof(Renderer2DData::QuadVertex));
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
        s_Renderer2DData->QuadVertexBufferBase = new Renderer2DData::QuadVertex[s_Renderer2DData->MaxVertices];
        
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

        // Lines
        {
            s_Renderer2DData->LineShader = Shader::Create("assets/shaders/Renderer2D_Line.glsl");

            s_Renderer2DData->LineVertexBuffer = VertexBuffer::Create(s_Renderer2DData->MaxLineVertices * sizeof(Renderer2DData::LineVertex));
            s_Renderer2DData->LineVertexBuffer->AddLayout({
                { ShaderDataType::Float3, "a_Position" },
                { ShaderDataType::Float4, "a_Color" }
            });

            s_Renderer2DData->LineVertexBufferBase = new Renderer2DData::LineVertex[s_Renderer2DData->MaxLineVertices];
            uint32_t* lineIndices = new uint32_t[s_Renderer2DData->MaxLineIndices];
            for (uint32_t i = 0; i < s_Renderer2DData->MaxLineIndices; i++)
                lineIndices[i] = i;

            s_Renderer2DData->LineIndexBuffer = IndexBuffer::Create(s_Renderer2DData->MaxLineIndices, lineIndices);
            delete[] lineIndices;

            s_Renderer2DData->LineVertexArray = VertexArray::Create();
            s_Renderer2DData->LineVertexArray->AddVertexBuffer(s_Renderer2DData->LineVertexBuffer);
            s_Renderer2DData->LineVertexArray->SetIndexBuffer(s_Renderer2DData->LineIndexBuffer);
        }
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

        s_Renderer2DData->LineIndexCount = 0;
        s_Renderer2DData->LineVertexBufferPtr = s_Renderer2DData->LineVertexBufferBase;

        StartBatch();
    }

    // ******************************************************************************
    // Begin the 2D Scene
    // ******************************************************************************
    void SceneRenderer::BeginScene(const iKan::Camera& camera, const glm::mat4& transform)
    {
        // Upload Camera View Projection Matris to shader
        glm::mat4 viewProj = camera.GetProjection() * glm::inverse(transform);

        auto textureShader = s_Renderer2DData->ShaderLibrary->Get("BatchRenderer2DShader");

        textureShader->Bind();
        textureShader->SetUniformMat4("u_ViewProjection", viewProj);

        s_Renderer2DData->LineIndexCount = 0;
        s_Renderer2DData->LineVertexBufferPtr = s_Renderer2DData->LineVertexBufferBase;

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
        if (s_Renderer2DData->QuadIndexCount == 0 || s_Renderer2DData->LineIndexCount == 0)
            return;

        // Render Quad
        uint32_t quadDataSize = (uint32_t)((uint8_t*)s_Renderer2DData->QuadVertexBufferPtr - (uint8_t*)s_Renderer2DData->QuadVertexBufferBase);
        s_Renderer2DData->QuadVertexBuffer->SetData(s_Renderer2DData->QuadVertexBufferBase, quadDataSize);
        
        // Bind textures
        for (uint32_t i = 0; i < s_Renderer2DData->TextureSlotIndex; i++)
        {
            if (i > 0)
                RendererStatistics::TextureCount++;

            s_Renderer2DData->TextureSlots[i]->Bind(i);
        }
        
        // Render the Quads
        Renderer::DrawIndexed(s_Renderer2DData->QuadVertexArray, s_Renderer2DData->QuadIndexCount);

        // Render line

        uint32_t lineDataSize = (uint32_t)((uint8_t*)s_Renderer2DData->LineVertexBufferPtr - (uint8_t*)s_Renderer2DData->LineVertexBufferBase);
        s_Renderer2DData->LineVertexBuffer->SetData(s_Renderer2DData->LineVertexBufferBase, lineDataSize);

        // Render the Lines
        Renderer::SetLineThickness(2.0f);
        Renderer::DrawIndexed(s_Renderer2DData->LineIndexCount, RendererAPI::PrimitiveType::Lines, s_Renderer2DData->DepthTest);
    }

    // ******************************************************************************
    // New batch
    // ******************************************************************************
    void SceneRenderer::NextBatch()
    {
        // if num Quad per Batch exceeds then Render the Scene and reset all parameters
        EndScene();

        // reset Quad Data
        s_Renderer2DData->QuadIndexCount = 0;
        s_Renderer2DData->QuadVertexBufferPtr = s_Renderer2DData->QuadVertexBufferBase;
        
        s_Renderer2DData->TextureSlotIndex = 1;

        // reset Line date
        s_Renderer2DData->LineIndexCount = 0;
        s_Renderer2DData->LineVertexBufferPtr = s_Renderer2DData->LineVertexBufferBase;
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
        if (s_Renderer2DData->QuadIndexCount >= Renderer2DData::MaxIndices)
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
                if (s_Renderer2DData->TextureSlotIndex >= Renderer2DData::MaxTextureSlots)
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

    // ******************************************************************************
    // Initialise the Scene Data
    // ******************************************************************************
    void SceneRenderer::DrawLine(const glm::vec3& p0, const glm::vec3& p1, const glm::vec4& color)
    {
        if (s_Renderer2DData->LineIndexCount >= Renderer2DData::MaxLineIndices)
            NextBatch();

        s_Renderer2DData->LineVertexBufferPtr->Position = p0;
        s_Renderer2DData->LineVertexBufferPtr->Color = color;
        s_Renderer2DData->LineVertexBufferPtr++;

        s_Renderer2DData->LineVertexBufferPtr->Position = p1;
        s_Renderer2DData->LineVertexBufferPtr->Color = color;
        s_Renderer2DData->LineVertexBufferPtr++;

        s_Renderer2DData->LineIndexCount += 2;

        RendererStatistics::LineCount++;
    }

    // ******************************************************************************
    // Initialise the Scene Data
    // ******************************************************************************
    void SceneRenderer::InitSceneData()
    {
        // Add the Geometry Render Pass
        Framebuffer::Specification framebufferSpec;

        framebufferSpec.Width   = 1280;
        framebufferSpec.Height  = 720;
        framebufferSpec.Samples = 8;

        framebufferSpec.ClearColor  = { 0.1f, 0.1f, 0.1f, 1.0f };
        framebufferSpec.Attachments = { Framebuffer::TextureSpecification::TextureFormat::RGBA8,
            Framebuffer::TextureSpecification::TextureFormat::DEPTH24STENCIL8,
            Framebuffer::TextureSpecification::TextureFormat::R32I };

        RenderPassSpecification geoRenderPassSpec;
        geoRenderPassSpec.TargetFramebuffer = Framebuffer::Create(framebufferSpec);

        s_SceneRendererData->GeoPass = RenderPass::Create(geoRenderPassSpec);

        // Add the Composit Render Pass
        framebufferSpec.Samples = -1;

        framebufferSpec.ClearColor  = { 0.5f, 0.1f, 0.1f, 1.0f };
        framebufferSpec.Attachments = { Framebuffer::TextureSpecification::TextureFormat::RGBA8};

        RenderPassSpecification compRenderPassSpec;
        compRenderPassSpec.TargetFramebuffer = Framebuffer::Create(framebufferSpec);

        s_SceneRendererData->CompositePass = RenderPass::Create(compRenderPassSpec);

        // Add the shaders
        s_SceneRendererData->CompositeShader = Shader::Create("assets/shaders/SceneComposite.glsl");
        s_SceneRendererData->BRDFLUT = Texture::Create("assets/textures/BRDF_LUT.tga");

        // Grid
        auto gridShader = Shader::Create("assets/shaders/Grid.glsl");
        s_SceneRendererData->GridMaterial = MaterialInstance::Create(Material::Create(gridShader));

        float gridScale = 16.025f, gridSize = 0.025f;
        s_SceneRendererData->GridMaterial->Set("u_Scale", gridScale);
        s_SceneRendererData->GridMaterial->Set("u_Res", gridSize);

        // Outline
        auto outlineShader = Shader::Create("assets/shaders/Outline.glsl");
        s_SceneRendererData->OutlineMaterial = MaterialInstance::Create(Material::Create(outlineShader));
        s_SceneRendererData->OutlineMaterial->SetFlag(MaterialFlag::DepthTest, false);
    }

    // ******************************************************************************
    // Update the size of frambuffer
    // ******************************************************************************
    void SceneRenderer::SetViewportSize(uint32_t width, uint32_t height)
    {
        s_SceneRendererData->GeoPass->GetSpecification().TargetFramebuffer->Resize(width, height);
        s_SceneRendererData->CompositePass->GetSpecification().TargetFramebuffer->Resize(width, height);
    }

    // ******************************************************************************
    // Begin the 3D Scene
    // ******************************************************************************
    void SceneRenderer::BeginMeshScene(const Scene* scene, const SceneRenderer::Camera& camera)
    {
        IK_CORE_ASSERT(!s_SceneRendererData->ActiveScene, "Secne already linked to data");

        s_SceneRendererData->ActiveScene = scene;
        s_SceneRendererData->SceneData.SceneCamera = camera;

        s_SceneRendererData->SceneData.SkyboxMaterial   = scene->m_Data.SkyboxMaterial;
        s_SceneRendererData->SceneData.SceneEnvironment = scene->m_Data.Environment;
        s_SceneRendererData->SceneData.ActiveLight      = scene->m_Data.Light;
    }

    // ******************************************************************************
    // Submit a mesh with matreial to the scene
    // ******************************************************************************
    void SceneRenderer::SubmitMesh(Ref<Mesh> mesh, const glm::mat4& transform, Ref<MaterialInstance> overrideMaterial)
    {
        // TODO: Culling, sorting, etc.
        s_SceneRendererData->DrawList.push_back({ mesh, overrideMaterial, transform });
    }

    // ******************************************************************************
    // Submit a mesh without material
    // ******************************************************************************
    void SceneRenderer::SubmitSelectedMesh(Ref<Mesh> mesh, const glm::mat4& transform)
    {
        s_SceneRendererData->SelectedMeshDrawList.push_back({ mesh, nullptr, transform });
    }

    // ******************************************************************************
    // Flush and draw the mesh
    // ******************************************************************************
    void SceneRenderer::FlushDrawMeshList()
    {
        IK_CORE_ASSERT(!s_SceneRendererData->ActiveScene, "Scene not created");

        GeometryPass();
        CompositePass();

        s_SceneRendererData->DrawList.clear();
        s_SceneRendererData->SelectedMeshDrawList.clear();
        s_SceneRendererData->SceneData = {};
    }

    // ******************************************************************************
    // Geometry Render Pass
    // ******************************************************************************
    void SceneRenderer::GeometryPass()
    {
        bool outline = s_SceneRendererData->SelectedMeshDrawList.size() > 0;

        if (outline)
        {
            Renderer::GetRenderer()->SetStencils();
        }

        Renderer::BeginRenderPass(s_SceneRendererData->GeoPass);

        if (outline)
        {
            Renderer::GetRenderer()->StencilMask(0);
        }

        auto viewProjection = s_SceneRendererData->SceneData.SceneCamera.Camera.GetProjection() * s_SceneRendererData->SceneData.SceneCamera.ViewMatrix;
        glm::vec3 cameraPosition = glm::inverse(s_SceneRendererData->SceneData.SceneCamera.ViewMatrix)[3];

        // Skybox
        auto skyboxShader = s_SceneRendererData->SceneData.SkyboxMaterial->GetShader();
        s_SceneRendererData->SceneData.SkyboxMaterial->Set("u_InverseVP", glm::inverse(viewProjection));
        Renderer::SubmitFullscreenQuad(s_SceneRendererData->SceneData.SkyboxMaterial);

        // Render entities
        for (auto& dc : s_SceneRendererData->DrawList)
        {
            auto baseMaterial = dc.Mesh->GetMaterial();
            baseMaterial->Set("u_ViewProjectionMatrix", viewProjection);
            baseMaterial->Set("u_CameraPosition", cameraPosition);

            // Environment (TODO: don't do this per mesh)
            baseMaterial->Set("u_EnvRadianceTex", s_SceneRendererData->SceneData.SceneEnvironment.RadianceMap);
            baseMaterial->Set("u_EnvIrradianceTex", s_SceneRendererData->SceneData.SceneEnvironment.IrradianceMap);
            baseMaterial->Set("u_BRDFLUTTexture", s_SceneRendererData->BRDFLUT);

            // Set lights (TODO: move to light environment and don't do per mesh)
            baseMaterial->Set("lights", s_SceneRendererData->SceneData.ActiveLight);

            auto overrideMaterial = nullptr; // dc.Material;
            Renderer::SubmitMesh(dc.Mesh, dc.Transform, overrideMaterial);
        }

        if (outline)
        {
            glStencilFunc(GL_ALWAYS, 1, 0xff);
            Renderer::StencilMask(0xff);
        }
        for (auto& dc : s_SceneRendererData->SelectedMeshDrawList)
        {
            auto baseMaterial = dc.Mesh->GetMaterial();
            baseMaterial->Set("u_ViewProjectionMatrix", viewProjection);
            baseMaterial->Set("u_CameraPosition", cameraPosition);

            // Environment (TODO: don't do this per mesh)
            baseMaterial->Set("u_EnvRadianceTex", s_SceneRendererData->SceneData.SceneEnvironment.RadianceMap);
            baseMaterial->Set("u_EnvIrradianceTex", s_SceneRendererData->SceneData.SceneEnvironment.IrradianceMap);
            baseMaterial->Set("u_BRDFLUTTexture", s_SceneRendererData->BRDFLUT);

            // Set lights (TODO: move to light environment and don't do per mesh)
            baseMaterial->Set("lights", s_SceneRendererData->SceneData.ActiveLight);

            auto overrideMaterial = nullptr; // dc.Material;
            Renderer::SubmitMesh(dc.Mesh, dc.Transform, overrideMaterial);
        }

        if (outline)
        {
            glStencilFunc(GL_NOTEQUAL, 1, 0xff);
            Renderer::StencilMask(0xff);

            glLineWidth(10);
            glEnable(GL_LINE_SMOOTH);
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            glDisable(GL_DEPTH_TEST);

            // Draw outline here
            s_SceneRendererData->OutlineMaterial->Set("u_ViewProjection", viewProjection);
            for (auto& dc : s_SceneRendererData->SelectedMeshDrawList)
            {
                Renderer::SubmitMesh(dc.Mesh, dc.Transform, s_SceneRendererData->OutlineMaterial);
            }

            glPointSize(10);
            glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);

            for (auto& dc : s_SceneRendererData->SelectedMeshDrawList)
            {
                Renderer::SubmitMesh(dc.Mesh, dc.Transform, s_SceneRendererData->OutlineMaterial);
            }

            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            glStencilMask(0xff);
            glStencilFunc(GL_ALWAYS, 1, 0xff);
            glEnable(GL_DEPTH_TEST);
        }
    }

    // ******************************************************************************
    //
    // ******************************************************************************
    void SceneRenderer::CompositePass()
    {
        Renderer::BeginRenderPass(s_SceneRendererData->CompositePass);
        s_SceneRendererData->CompositeShader->Bind();
        s_SceneRendererData->CompositeShader->SetUniformFloat1("u_Exposure", s_SceneRendererData->SceneData.SceneCamera.Camera.GetExposure());
        s_SceneRendererData->CompositeShader->SetUniformInt1("u_TextureSamples", s_SceneRendererData->GeoPass->GetSpecification().TargetFramebuffer->GetSpecification().Samples);
        s_SceneRendererData->GeoPass->GetSpecification().TargetFramebuffer->BindTexture();
        Renderer::SubmitFullscreenQuad(nullptr);
        Renderer::EndRenderPass();
    }

    // ******************************************************************************
    // End the 2D Scene
    // ******************************************************************************
    void SceneRenderer::EndMeshScene()
    {
        IK_CORE_ASSERT(s_SceneRendererData->ActiveScene, "Scene is not created");
        s_SceneRendererData->ActiveScene = nullptr;
        FlushDrawMeshList();
    }

}
