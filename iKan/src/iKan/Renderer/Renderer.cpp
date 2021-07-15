// ******************************************************************************
// File         : Renderer.cpp
// Description  : Renderer API Visible to Core and client
// Project      : iKan : Renderer
//
// Created by Ashish on 16/04/21.
// Copyright © 2021 Ashish. All rights reserved.
// ******************************************************************************

#include "Renderer.h"
#include <iKan/Renderer/SceneRenderer.h>
#include <iKan/Renderer/RendererStats.h>
#include <iKan/Renderer/VertexArray.h>

namespace iKan {
 
    Scope<RendererAPI> Renderer::s_RendererAPI = RendererAPI::Create();

    // ******************************************************************************
    // Basic renderer Data
    // ******************************************************************************
    struct RendererData
    {
        Ref<RenderPass> m_ActiveRenderPass;

        Ref<VertexBuffer> m_FullscreenQuadVertexBuffer;
        Ref<IndexBuffer>  m_FullscreenQuadIndexBuffer;
        Ref<VertexArray>  m_FullscreenQuadVertexArray;

    };
    static RendererData s_Data;

    // ******************************************************************************
    // Initialise all renderer
    // ******************************************************************************
    void Renderer::Init()
    {
        IK_CORE_INFO("Initialising all Renderers");
        
        s_RendererAPI->Init();
        SceneRenderer::Init();

        // Create fullscreen quad
        float x = -1;
        float y = -1;
        float width = 2, height = 2;
        struct QuadVertex
        {
            glm::vec3 Position;
            glm::vec2 TexCoord;
        };

        QuadVertex* data = new QuadVertex[4];

        data[0].Position = glm::vec3(x, y, 0.1f);
        data[0].TexCoord = glm::vec2(0, 0);

        data[1].Position = glm::vec3(x + width, y, 0.1f);
        data[1].TexCoord = glm::vec2(1, 0);

        data[2].Position = glm::vec3(x + width, y + height, 0.1f);
        data[2].TexCoord = glm::vec2(1, 1);

        data[3].Position = glm::vec3(x, y + height, 0.1f);
        data[3].TexCoord = glm::vec2(0, 1);

        s_Data.m_FullscreenQuadVertexBuffer = VertexBuffer::Create(4 * sizeof(QuadVertex), (float*)data);
        s_Data.m_FullscreenQuadVertexBuffer->AddLayout({
            { ShaderDataType::Float3, "a_Position" },
            { ShaderDataType::Float2, "a_TexCoord" }
        });

        uint32_t indices[6] = { 0, 1, 2, 2, 3, 0, };
        s_Data.m_FullscreenQuadIndexBuffer = IndexBuffer::Create(6, indices);

        s_Data.m_FullscreenQuadVertexArray = VertexArray::Create();
        s_Data.m_FullscreenQuadVertexArray->AddVertexBuffer(s_Data.m_FullscreenQuadVertexBuffer);
        s_Data.m_FullscreenQuadVertexArray->SetIndexBuffer(s_Data.m_FullscreenQuadIndexBuffer);
    }
    
    // ******************************************************************************
    // IClear color and buffer
    // ******************************************************************************
    void Renderer::Clear(glm::vec4 color)
    {
        s_RendererAPI->Clear();
        s_RendererAPI->SetClearColor(color);
    }
    
    // ******************************************************************************
    // Set the view port
    // ******************************************************************************
    void Renderer::SetViewport(float width, float height)
    {
        s_RendererAPI->SetViewPort(width, height);
    }
    
    // ******************************************************************************
    // Draw commanr
    // ******************************************************************************
    void Renderer::DrawIndexed(uint32_t count)
    {
        RendererStatistics::DrawCalls++;
        s_RendererAPI->DrawIndexed(count);
    }
    
    // ******************************************************************************
    // Draw command
    // ******************************************************************************
    void Renderer::DrawIndexed(const Ref<VertexArray> &vertexArray, uint32_t count)
    {
        RendererStatistics::DrawCalls++;
        s_RendererAPI->DrawIndexed(vertexArray, count);
    }
    
    // ******************************************************************************
    // Shutdown all renderer
    // ******************************************************************************
    void Renderer::Shutdown()
    {
        SceneRenderer::Shutdown();
    }

    // ******************************************************************************
    // Draw command
    // ******************************************************************************
    void Renderer::DrawIndexed(uint32_t count, RendererAPI::PrimitiveType type, bool depthTest)
    {
        RendererStatistics::DrawCalls++;
        s_RendererAPI->DrawIndexed(count, type, depthTest);
    }

    // ******************************************************************************
    // begin the Render Pass and initialize the Framebuffer
    // ******************************************************************************
    void Renderer::BeginRenderPass(Ref<RenderPass> renderPass, bool clear)
    {
        IK_CORE_ASSERT(renderPass, "Render pass cannot be null!");

        // TODO: Convert all of this into a render command buffer
        s_Data.m_ActiveRenderPass = renderPass;

        renderPass->GetSpecification().TargetFramebuffer->Bind();
        if (clear)
        {
            const glm::vec4& clearColor = renderPass->GetSpecification().TargetFramebuffer->GetSpecification().ClearColor;
            s_RendererAPI->SetClearColor(clearColor);
        }
    }

    // ******************************************************************************
    // End the active Render Pass
    // ******************************************************************************
    void Renderer::EndRenderPass()
    {
        IK_CORE_ASSERT(s_Data.m_ActiveRenderPass, "No active render pass! Have you called Renderer::EndRenderPass twice?");
        s_Data.m_ActiveRenderPass->GetSpecification().TargetFramebuffer->Unbind();
        s_Data.m_ActiveRenderPass = nullptr;
    }

    // ******************************************************************************
    // ******************************************************************************
    void Renderer::SubmitQuad(Ref<MaterialInstance> material, const glm::mat4& transform)
    {
        bool depthTest = true;
        if (material)
        {
            material->Bind();
            depthTest = material->GetFlag(MaterialFlag::DepthTest);

            auto shader = material->GetShader();
            shader->SetUniformMat4("u_Transform", transform);
        }

        s_Data.m_FullscreenQuadVertexBuffer->Bind();
        s_Data.m_FullscreenQuadVertexArray->Bind();
        s_Data.m_FullscreenQuadIndexBuffer->Bind();
        Renderer::DrawIndexed(6, RendererAPI::PrimitiveType::Triangles, depthTest);
    }

    // ******************************************************************************
    // ******************************************************************************
    void Renderer::SubmitFullscreenQuad(Ref<MaterialInstance> material)
    {
        bool depthTest = true;
        if (material)
        {
            material->Bind();
            depthTest = material->GetFlag(MaterialFlag::DepthTest);
        }

        s_Data.m_FullscreenQuadVertexBuffer->Bind();
        s_Data.m_FullscreenQuadVertexArray->Bind();
        s_Data.m_FullscreenQuadIndexBuffer->Bind();
        Renderer::DrawIndexed(6, RendererAPI::PrimitiveType::Triangles, depthTest);
    }

    // ******************************************************************************
    // ******************************************************************************
    void Renderer::SubmitMesh(Ref<Mesh> mesh, const glm::mat4& transform, Ref<MaterialInstance> overrideMaterial)
    {
        // auto material = overrideMaterial ? overrideMaterial : mesh->GetMaterialInstance();
        // auto shader = material->GetShader();
        // TODO: Sort this out
        mesh->m_VertexBuffer->Bind();
        mesh->m_VertexArray->Bind();
        mesh->m_IndexBuffer->Bind();

        auto& materials = mesh->GetMaterials();
        for (Submesh& submesh : mesh->m_Submeshes)
        {
            // Material
            auto material = overrideMaterial ? overrideMaterial : materials[submesh.MaterialIndex];
            auto shader = material->GetShader();
            material->Bind();

            if (mesh->m_IsAnimated)
            {
                for (size_t i = 0; i < mesh->m_BoneTransforms.size(); i++)
                {
                    std::string uniformName = std::string("u_BoneTransforms[") + std::to_string(i) + std::string("]");
                    mesh->m_MeshShader->SetUniformMat4(uniformName, mesh->m_BoneTransforms[i]);
                }
            }
            shader->SetUniformMat4("u_Transform", transform * submesh.Transform);

            DrawElementBaseVertex(submesh.IndexCount, (void*)(sizeof(uint32_t) * submesh.BaseIndex), submesh.BaseVertex, material->GetFlag(MaterialFlag::DepthTest));
        }
    }

    // ******************************************************************************
    // ******************************************************************************
    void Renderer::DrawAABB(Ref<Mesh> mesh, const glm::mat4& transform, const glm::vec4& color)
    {
        for (Submesh& submesh : mesh->m_Submeshes)
        {
            auto& aabb = submesh.BoundingBox;
            auto aabbTransform = transform * submesh.Transform;
            DrawAABB(aabb, aabbTransform);
        }
    }

    // ******************************************************************************
    // ******************************************************************************
    void Renderer::DrawAABB(const AABB& aabb, const glm::mat4& transform, const glm::vec4& color /*= glm::vec4(1.0f)*/)
    {
        glm::vec4 min = { aabb.Min.x, aabb.Min.y, aabb.Min.z, 1.0f };
        glm::vec4 max = { aabb.Max.x, aabb.Max.y, aabb.Max.z, 1.0f };

        glm::vec4 corners[8] =
        {
            transform * glm::vec4 { aabb.Min.x, aabb.Min.y, aabb.Max.z, 1.0f },
            transform * glm::vec4 { aabb.Min.x, aabb.Max.y, aabb.Max.z, 1.0f },
            transform * glm::vec4 { aabb.Max.x, aabb.Max.y, aabb.Max.z, 1.0f },
            transform * glm::vec4 { aabb.Max.x, aabb.Min.y, aabb.Max.z, 1.0f },

            transform * glm::vec4 { aabb.Min.x, aabb.Min.y, aabb.Min.z, 1.0f },
            transform * glm::vec4 { aabb.Min.x, aabb.Max.y, aabb.Min.z, 1.0f },
            transform * glm::vec4 { aabb.Max.x, aabb.Max.y, aabb.Min.z, 1.0f },
            transform * glm::vec4 { aabb.Max.x, aabb.Min.y, aabb.Min.z, 1.0f }
        };

        for (uint32_t i = 0; i < 4; i++)
            SceneRenderer::DrawLine(corners[i], corners[(i + 1) % 4], color);

        for (uint32_t i = 0; i < 4; i++)
            SceneRenderer::DrawLine(corners[i + 4], corners[((i + 1) % 4) + 4], color);

        for (uint32_t i = 0; i < 4; i++)
            SceneRenderer::DrawLine(corners[i], corners[i + 4], color);
    }

    // ******************************************************************************
    // ******************************************************************************
    void Renderer::SetLineThickness(float thickness)
    {
        s_RendererAPI->SetLineThickness(thickness);
    }

    // ******************************************************************************
    // ******************************************************************************
    void Renderer::DrawElementBaseVertex(uint32_t indexCount, void* baseIndex, uint32_t baseVertex, bool depth)
    {
        s_RendererAPI->DrawElementBaseVertex(indexCount, baseIndex, baseVertex, depth);
    }

}
    
