// ******************************************************************************
// File         : Mesh.h
// Description  : Mesh Renderer Class
// Projecte     : iKan : Renderer
//
// Created by Ashish on 05/07/21.
// Copyright © 2021 Ashish. All rights reserved.
// ******************************************************************************

#pragma once

#include <iKan/Core/Math/AABB.h>
#include <iKan/Core/TimeStep.h>
#include <iKan/Renderer/Shader.h>
#include <iKan/Renderer/Texture.h>
#include <iKan/Renderer/VertexArray.h>
#include <iKan/Renderer/Material.h>

struct aiScene;
struct aiNode;
struct aiAnimation;
struct aiNodeAnim;

namespace Assimp {
    class Importer;
}

namespace iKan {

    // ******************************************************************************
    // Vertex Info for Static Mesh
    // ******************************************************************************
    struct Vertex
    {
        glm::vec3 Position;
        glm::vec3 Normal;
        glm::vec3 Tangent;
        glm::vec3 Binormal;
        glm::vec2 Texcoord;
    };

    // ******************************************************************************
    // Vertex Info for animated Mesh
    // ******************************************************************************
    struct AnimatedVertex
    {
        glm::vec3 Position;
        glm::vec3 Normal;
        glm::vec3 Tangent;
        glm::vec3 Binormal;
        glm::vec2 Texcoord;

        uint32_t IDs[4] = { 0, 0,0, 0 };
        float Weights[4]{ 0.0f, 0.0f, 0.0f, 0.0f };

        void AddBoneData(uint32_t BoneID, float Weight)
        {
            for (size_t i = 0; i < 4; i++)
            {
                if (Weights[i] == 0.0)
                {
                    IDs[i] = BoneID;
                    Weights[i] = Weight;
                    return;
                }
            }

            // TODO: Keep top weights
            IK_CORE_WARN("Vertex has more than four bones/weights affecting it, extra data will be discarded (BoneID={0}, Weight={1})", BoneID, Weight);
        }
    };

    // ******************************************************************************
    // ******************************************************************************
    struct Index
    {
        uint32_t V1, V2, V3;
    };

    // ******************************************************************************
    // ******************************************************************************
    struct Triangle
    {
        Vertex V0, V1, V2;

        Triangle(const Vertex& v0, const Vertex& v1, const Vertex& v2)
        : V0(v0), V1(v1), V2(v2) {}
    };

    // ******************************************************************************
    // ******************************************************************************
    struct BoneInfo
    {
        glm::mat4 BoneOffset;
        glm::mat4 FinalTransformation;
    };

    // ******************************************************************************
    // Subcomponent of Mesh
    // ******************************************************************************
    class Submesh
    {
    public:
        uint32_t BaseVertex;
        uint32_t BaseIndex;
        uint32_t MaterialIndex;
        uint32_t IndexCount;

        glm::mat4 Transform;
        AABB BoundingBox;

        std::string NodeName, MeshName;
    };

    // ******************************************************************************
    // Mesh Class Defination. This will store all the renderer component of Mesh
    // ******************************************************************************
    class Mesh
    {
    public:
        Mesh(const std::string& path);
        ~Mesh();

        void OnUpdate(Timestep ts);
        void DumpVertexBuffer();

        std::vector<Submesh>& GetSubmeshes() { return m_Submeshes; }
        const std::vector<Submesh>& GetSubmeshes() const { return m_Submeshes; }

        Ref<Shader> GetMeshShader() { return m_MeshShader; }
        Ref<Material> GetMaterial() { return m_BaseMaterial; }
        std::vector<Ref<MaterialInstance>> GetMaterials() { return m_Materials; }
        const std::vector<Ref<Texture>>& GetTextures() const { return m_Textures; }
        const std::string& GetFilePath() const { return m_Path; }

        const std::vector<Triangle> GetTriangleCache(uint32_t index) const { return m_TriangleCache.at(index); }

    private:
        void TraverseNodes(aiNode* node, const glm::mat4& parentTransform = glm::mat4(1.0f), uint32_t level = 0);
        void BoneTransform(float time);
        void ReadNodeHierarchy(float AnimationTime, const aiNode* pNode, const glm::mat4& ParentTransform);
        const aiNodeAnim* FindNodeAnim(const aiAnimation* animation, const std::string& nodeName);

        uint32_t FindPosition(float AnimationTime, const aiNodeAnim* pNodeAnim);
        uint32_t FindRotation(float AnimationTime, const aiNodeAnim* pNodeAnim);
        uint32_t FindScaling(float AnimationTime, const aiNodeAnim* pNodeAnim);

        glm::vec3 InterpolateTranslation(float animationTime, const aiNodeAnim* nodeAnim);
        glm::quat InterpolateRotation(float animationTime, const aiNodeAnim* nodeAnim);
        glm::vec3 InterpolateScale(float animationTime, const aiNodeAnim* nodeAnim);

    private:
        std::string m_Path;
        Scope<Assimp::Importer> m_Importer;
        const aiScene* m_Scene;
        glm::mat4 m_InverseTransform;

        uint32_t m_BoneCount = 0;

        Ref<VertexArray>    m_VertexArray;
        Ref<VertexBuffer>   m_VertexBuffer;
        Ref<IndexBuffer>    m_IndexBuffer;

        std::vector<Submesh>        m_Submeshes;
        std::vector<Vertex>         m_StaticVertices;
        std::vector<AnimatedVertex> m_AnimatedVertices;
        std::vector<Index>          m_Indices;
        std::vector<BoneInfo>       m_BoneInfo;
        std::vector<glm::mat4>      m_BoneTransforms;
        
        std::unordered_map<std::string, uint32_t>           m_BoneMapping;
        std::unordered_map<uint32_t, std::vector<Triangle>> m_TriangleCache;

        // Materials
        Ref<Shader>   m_MeshShader;
        Ref<Material> m_BaseMaterial;

        std::vector<Ref<Texture>> m_Textures;
        std::vector<Ref<Texture>> m_NormalMaps;
        std::vector<Ref<MaterialInstance>> m_Materials;

        // Animation
        bool m_IsAnimated       = false;
        bool m_AnimationPlaying = true;

        float m_AnimationTime   = 0.0f;
        float m_WorldTime       = 0.0f;
        float m_TimeMultiplier  = 1.0f;
    };

}
