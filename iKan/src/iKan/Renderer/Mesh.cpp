// ******************************************************************************
// File         : Mesh.cpp
// Description  : Mesh Renderer Class Implementation
// Projecte     : iKan : Renderer
//
// Created by Ashish on 05/07/21.
// Copyright © 2021 Ashish. All rights reserved.
// ******************************************************************************

#include "Mesh.h"

#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/Importer.hpp>
#include <assimp/DefaultLogger.hpp>
#include <assimp/LogStream.hpp>

#include <iKan/Renderer/SceneRenderer.h>
#include <iKan/Renderer/Buffers.h>

#define MESH_DEBUG_LOG 1

#if MESH_DEBUG_LOG
    #define IK_MESH_LOG(...) IK_CORE_TRACE(__VA_ARGS__)
#else
    #define IK_MESH_LOG(...)
#endif

namespace iKan {

    // ******************************************************************************
    // Flags set to load the Mesh
    // *****************************************************************************
    static const uint32_t s_MeshImportFlags =
                                                aiProcess_CalcTangentSpace |        // Create binormals/tangents just in case
                                                aiProcess_Triangulate |             // Make sure we're triangles
                                                aiProcess_SortByPType |             // Split meshes by primitive type
                                                aiProcess_GenNormals |              // Make sure we have legit normals
                                                aiProcess_GenUVCoords |             // Convert UVs if required
                                                aiProcess_OptimizeMeshes |          // Batch draws where possible
                                                aiProcess_ValidateDataStructure;    // Validation

    // ******************************************************************************
    // Convert aiMatrix to glm::mat4
    // *****************************************************************************
    static glm::mat4 Mat4FromAssimpMat4(const aiMatrix4x4& matrix)
    {
        glm::mat4 result;
        //the a,b,c,d in assimp is the row ; the 1,2,3,4 is the column
        result[0][0] = matrix.a1; result[1][0] = matrix.a2; result[2][0] = matrix.a3; result[3][0] = matrix.a4;
        result[0][1] = matrix.b1; result[1][1] = matrix.b2; result[2][1] = matrix.b3; result[3][1] = matrix.b4;
        result[0][2] = matrix.c1; result[1][2] = matrix.c2; result[2][2] = matrix.c3; result[3][2] = matrix.c4;
        result[0][3] = matrix.d1; result[1][3] = matrix.d2; result[2][3] = matrix.d3; result[3][3] = matrix.d4;
        return result;
    }

    // ******************************************************************************
    // Derived Logstream class for Assim Logger
    // *****************************************************************************
    struct LogStream : public Assimp::LogStream
    {
        // ******************************************************************************
        // Initialise the assimp logger
        // ******************************************************************************
        static void Initialize()
        {
            if (Assimp::DefaultLogger::isNullLogger())
            {
                Assimp::DefaultLogger::create("", Assimp::Logger::VERBOSE);
                Assimp::DefaultLogger::get()->attachStream(new LogStream, Assimp::Logger::Err | Assimp::Logger::Warn);
            }
        }

        // ******************************************************************************
        // Write the log (IK_LOG)
        // ******************************************************************************
        virtual void write(const char* message) override
        {
            IK_CORE_ERROR("Assimp error: {0}", message);
        }
    };

    // ******************************************************************************
    // Mesh constructor
    // ******************************************************************************
    Mesh::Mesh(const std::string &path)
    : m_Path(path)
    {
        LogStream::Initialize();

        IK_CORE_INFO("Loading Mesh: {0}", m_Path);

        m_Importer = CreateScope<Assimp::Importer>();

        m_Scene = m_Importer->ReadFile(m_Path, s_MeshImportFlags);
        if (!m_Scene || !m_Scene->HasMeshes())
            IK_CORE_ERROR("Failed to load mesh file: {0}", m_Path);

        m_IsAnimated       = m_Scene->mAnimations != nullptr;
        m_MeshShader       = m_IsAnimated ? SceneRenderer::GetShaderLibrary()->Get("BatchRenderer2DShader") : SceneRenderer::GetShaderLibrary()->Get("PBR_Static");
        m_BaseMaterial     = CreateRef<Material>(m_MeshShader);
        m_InverseTransform = glm::inverse(Mat4FromAssimpMat4(m_Scene->mRootNode->mTransformation));

        uint32_t vertexCount = 0;
        uint32_t indexCount = 0;

        m_Submeshes.reserve(m_Scene->mNumMeshes);
        for (size_t m = 0; m < m_Scene->mNumMeshes; m++)
        {
            aiMesh* mesh        = m_Scene->mMeshes[m];
            Submesh& submesh    = m_Submeshes.emplace_back();

            submesh.BaseVertex      = vertexCount;
            submesh.BaseIndex       = indexCount;
            submesh.MaterialIndex   = mesh->mMaterialIndex;
            submesh.IndexCount      = mesh->mNumFaces * 3;
            submesh.MeshName        = mesh->mName.C_Str();

            vertexCount += mesh->mNumVertices;
            indexCount  += submesh.IndexCount;

            IK_CORE_ASSERT(mesh->HasPositions(), "Meshes require positions.");
            IK_CORE_ASSERT(mesh->HasNormals(), "Meshes require normals.");

            // Vertices
            if (m_IsAnimated)
            {
                for (size_t i = 0; i < mesh->mNumVertices; i++)
                {
                    AnimatedVertex vertex;
                    vertex.Position = { mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z };
                    vertex.Normal   = { mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z };

                    if (mesh->HasTangentsAndBitangents())
                    {
                        vertex.Tangent  = { mesh->mTangents[i].x, mesh->mTangents[i].y, mesh->mTangents[i].z };
                        vertex.Binormal = { mesh->mBitangents[i].x, mesh->mBitangents[i].y, mesh->mBitangents[i].z };
                    }

                    if (mesh->HasTextureCoords(0))
                        vertex.Texcoord = { mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y };

                    m_AnimatedVertices.push_back(vertex);
                }
            } // if (m_IsAnimated)
            else
            {
                auto& aabb = submesh.BoundingBox;
                aabb.Min = { FLT_MAX, FLT_MAX, FLT_MAX };
                aabb.Max = { -FLT_MAX, -FLT_MAX, -FLT_MAX };
                for (size_t i = 0; i < mesh->mNumVertices; i++)
                {
                    Vertex vertex;
                    vertex.Position = { mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z };
                    vertex.Normal = { mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z };

                    aabb.Min.x = glm::min(vertex.Position.x, aabb.Min.x);
                    aabb.Min.y = glm::min(vertex.Position.y, aabb.Min.y);
                    aabb.Min.z = glm::min(vertex.Position.z, aabb.Min.z);
                    aabb.Max.x = glm::max(vertex.Position.x, aabb.Max.x);
                    aabb.Max.y = glm::max(vertex.Position.y, aabb.Max.y);
                    aabb.Max.z = glm::max(vertex.Position.z, aabb.Max.z);

                    if (mesh->HasTangentsAndBitangents())
                    {
                        vertex.Tangent  = { mesh->mTangents[i].x, mesh->mTangents[i].y, mesh->mTangents[i].z };
                        vertex.Binormal = { mesh->mBitangents[i].x, mesh->mBitangents[i].y, mesh->mBitangents[i].z };
                    }

                    if (mesh->HasTextureCoords(0))
                        vertex.Texcoord = { mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y };

                    m_StaticVertices.push_back(vertex);
                }
            } // else -> if (m_IsAnimated)

            // Indices
            for (size_t i = 0; i < mesh->mNumFaces; i++)
            {
                IK_CORE_ASSERT((mesh->mFaces[i].mNumIndices == 3), "Must have 3 indices.");
                Index index = { mesh->mFaces[i].mIndices[0], mesh->mFaces[i].mIndices[1], mesh->mFaces[i].mIndices[2] };
                m_Indices.push_back(index);

                if (!m_IsAnimated)
                    m_TriangleCache[(uint32_t)m].emplace_back(m_StaticVertices[index.V1 + submesh.BaseVertex], m_StaticVertices[index.V2 + submesh.BaseVertex], m_StaticVertices[index.V3 + submesh.BaseVertex]);
            }
        } // for (size_t m = 0; m < m_Scene->mNumMeshes; m++)

        TraverseNodes(m_Scene->mRootNode);

        // Bones
        if (m_IsAnimated)
        {
            for (size_t m = 0; m < m_Scene->mNumMeshes; m++)
            {
                aiMesh* mesh     = m_Scene->mMeshes[m];
                Submesh& submesh = m_Submeshes[m];

                for (size_t i = 0; i < mesh->mNumBones; i++)
                {
                    aiBone* bone = mesh->mBones[i];
                    std::string boneName(bone->mName.data);

                    int32_t boneIndex = 0;
                    if (m_BoneMapping.find(boneName) == m_BoneMapping.end())
                    {
                        // Allocate an index for a new bone
                        boneIndex = m_BoneCount;
                        m_BoneCount++;
                        BoneInfo bi;
                        m_BoneInfo.push_back(bi);
                        m_BoneInfo[boneIndex].BoneOffset = Mat4FromAssimpMat4(bone->mOffsetMatrix);
                        m_BoneMapping[boneName] = boneIndex;
                    }
                    else
                    {
                        IK_MESH_LOG("Found existing bone in map");
                        boneIndex = m_BoneMapping[boneName];
                    }

                    for (size_t j = 0; j < bone->mNumWeights; j++)
                    {
                        int32_t VertexID = submesh.BaseVertex + bone->mWeights[j].mVertexId;
                        float Weight     = bone->mWeights[j].mWeight;
                        m_AnimatedVertices[VertexID].AddBoneData(boneIndex, Weight);
                    }
                }
            } // for (size_t m = 0; m < m_Scene->mNumMeshes; m++)
        } // if (m_IsAnimated)

        // Materials
        if (m_Scene->HasMaterials())
        {
            IK_MESH_LOG("---- Materials - {0} ----", path);

            m_Textures.resize(m_Scene->mNumMaterials);
            m_Materials.resize(m_Scene->mNumMaterials);
            for (uint32_t i = 0; i < m_Scene->mNumMaterials; i++)
            {
                auto aiMaterial     = m_Scene->mMaterials[i];
                auto aiMaterialName = aiMaterial->GetName();

                auto mi = CreateRef<MaterialInstance>(m_BaseMaterial, aiMaterialName.data);
                m_Materials[i] = mi;

                IK_MESH_LOG("  {0} (Index = {1})", aiMaterialName.data, i);
                aiString aiTexPath;

                IK_MESH_LOG("    TextureCount = {0}", aiMaterial->GetTextureCount(aiTextureType_DIFFUSE));

                aiColor3D aiColor;
                aiMaterial->Get(AI_MATKEY_COLOR_DIFFUSE, aiColor);

                float shininess, metalness;
                if (aiMaterial->Get(AI_MATKEY_SHININESS, shininess) != aiReturn_SUCCESS)
                    shininess = 80.0f; // Default value

                if (aiMaterial->Get(AI_MATKEY_REFLECTIVITY, metalness) != aiReturn_SUCCESS)
                    metalness = 0.0f;

                float roughness = 1.0f - glm::sqrt(shininess / 100.0f);
                IK_MESH_LOG("    COLOR = {0}, {1}, {2}", aiColor.r, aiColor.g, aiColor.b);
                IK_MESH_LOG("    ROUGHNESS = {0}", roughness);

                bool hasAlbedoMap = aiMaterial->GetTexture(aiTextureType_DIFFUSE, 0, &aiTexPath) == AI_SUCCESS;
                if (hasAlbedoMap)
                {
                    // TODO: Temp - this should be handled by iKan's filesystem
                    std::filesystem::path path  = m_Path;
                    auto parentPath             = path.parent_path();

                    parentPath /= std::string(aiTexPath.data);
                    std::string texturePath = parentPath.string();

                    IK_MESH_LOG("    Albedo map path = {0}", texturePath);

                    auto texture = Texture::Create(texturePath);
                    if (texture->Uploaded())
                    {
                        m_Textures[i] = texture;
                        mi->Set("u_AlbedoTexture", m_Textures[i]);
                        mi->Set("u_AlbedoTexToggle", 1.0f);
                    }
                    else
                    {
                        IK_CORE_ERROR("Could not load texture: {0}", texturePath);
                        // Fallback to albedo color
                        mi->Set("u_AlbedoColor", glm::vec3{ aiColor.r, aiColor.g, aiColor.b });
                    }
                } // if (hasAlbedoMap)
                else
                {
                    mi->Set("u_AlbedoColor", glm::vec3 { aiColor.r, aiColor.g, aiColor.b });
                    IK_MESH_LOG("    No albedo map");
                }

                // Normal maps
                mi->Set("u_NormalTexToggle", 0.0f);
                if (aiMaterial->GetTexture(aiTextureType_NORMALS, 0, &aiTexPath) == AI_SUCCESS)
                {
                    // TODO: Temp - this should be handled by iKan's filesystem
                    std::filesystem::path path  = m_Path;
                    auto parentPath             = path.parent_path();

                    parentPath /= std::string(aiTexPath.data);
                    std::string texturePath = parentPath.string();

                    IK_MESH_LOG("    Normal map path = {0}", texturePath);

                    auto texture = Texture::Create(texturePath);
                    if (texture->Uploaded())
                    {
                        mi->Set("u_NormalTexture", texture);
                        mi->Set("u_NormalTexToggle", 1.0f);
                    }
                    else
                    {
                        IK_CORE_ERROR("    Could not load texture: {0}", texturePath);
                    }
                }
                else
                {
                    IK_MESH_LOG("    No normal map");
                }

                // Roughness map
                // mi->Set("u_Roughness", 1.0f);
                // mi->Set("u_RoughnessTexToggle", 0.0f);
                if (aiMaterial->GetTexture(aiTextureType_SHININESS, 0, &aiTexPath) == AI_SUCCESS)
                {
                    // TODO: Temp - this should be handled by iKan's filesystem
                    std::filesystem::path path = m_Path;
                    auto parentPath            = path.parent_path();

                    parentPath /= std::string(aiTexPath.data);
                    std::string texturePath = parentPath.string();

                    IK_MESH_LOG("    Roughness map path = {0}", texturePath);

                    auto texture = Texture::Create(texturePath);
                    if (texture->Uploaded())
                    {
                        mi->Set("u_RoughnessTexture", texture);
                        mi->Set("u_RoughnessTexToggle", 1.0f);
                    }
                    else
                    {
                        IK_CORE_ERROR("    Could not load texture: {0}", texturePath);
                    }
                }
                else
                {
                    IK_MESH_LOG("    No roughness map");
                    mi->Set("u_Roughness", roughness);
                }

                // Metalness map (or is it??)
                if (aiMaterial->Get("$raw.ReflectionFactor|file", aiPTI_String, 0, aiTexPath) == AI_SUCCESS)
                {
                    // TODO: Temp - this should be handled by iKan's filesystem
                    std::filesystem::path path = m_Path;
                    auto parentPath            = path.parent_path();

                    parentPath /= std::string(aiTexPath.data);
                    std::string texturePath = parentPath.string();

                    auto texture = Texture::Create(texturePath);
                    if (texture->Uploaded())
                    {
                        IK_MESH_LOG("    Metalness map path = {0}", texturePath);
                        mi->Set("u_MetalnessTexture", texture);
                        mi->Set("u_MetalnessTexToggle", 1.0f);
                    }
                    else
                    {
                        IK_CORE_ERROR("Could not load texture: {0}", texturePath);
                    }
                }
                else
                {
                    IK_MESH_LOG("    No metalness texture");
                    mi->Set("u_Metalness", metalness);
                }

                bool metalnessTextureFound = false;
                for (uint32_t i = 0; i < aiMaterial->mNumProperties; i++)
                {
                    auto prop = aiMaterial->mProperties[i];

                    IK_MESH_LOG("Material Property:");
                    IK_MESH_LOG("  Name = {0}", prop->mKey.data);

                    IK_MESH_LOG("  Value = {0}", *(float*)prop->mData);

                    switch (prop->mSemantic)
                    {
                        case aiTextureType_NONE:
                            IK_MESH_LOG("  Semantic = aiTextureType_NONE");
                            break;
                        case aiTextureType_DIFFUSE:
                            IK_MESH_LOG("  Semantic = aiTextureType_DIFFUSE");
                            break;
                        case aiTextureType_SPECULAR:
                            IK_MESH_LOG("  Semantic = aiTextureType_SPECULAR");
                            break;
                        case aiTextureType_AMBIENT:
                            IK_MESH_LOG("  Semantic = aiTextureType_AMBIENT");
                            break;
                        case aiTextureType_EMISSIVE:
                            IK_MESH_LOG("  Semantic = aiTextureType_EMISSIVE");
                            break;
                        case aiTextureType_HEIGHT:
                            IK_MESH_LOG("  Semantic = aiTextureType_HEIGHT");
                            break;
                        case aiTextureType_NORMALS:
                            IK_MESH_LOG("  Semantic = aiTextureType_NORMALS");
                            break;
                        case aiTextureType_SHININESS:
                            IK_MESH_LOG("  Semantic = aiTextureType_SHININESS");
                            break;
                        case aiTextureType_OPACITY:
                            IK_MESH_LOG("  Semantic = aiTextureType_OPACITY");
                            break;
                        case aiTextureType_DISPLACEMENT:
                            IK_MESH_LOG("  Semantic = aiTextureType_DISPLACEMENT");
                            break;
                        case aiTextureType_LIGHTMAP:
                            IK_MESH_LOG("  Semantic = aiTextureType_LIGHTMAP");
                            break;
                        case aiTextureType_REFLECTION:
                            IK_MESH_LOG("  Semantic = aiTextureType_REFLECTION");
                            break;
                        case aiTextureType_UNKNOWN:
                            IK_MESH_LOG("  Semantic = aiTextureType_UNKNOWN");
                            break;
                    }

                    if (prop->mType == aiPTI_String)
                    {
                        uint32_t strLength = *(uint32_t*)prop->mData;
                        std::string str(prop->mData + 4, strLength);

                        std::string key = prop->mKey.data;
                        if (key == "$raw.ReflectionFactor|file")
                        {
                            metalnessTextureFound = true;

                            // TODO: Temp - this should be handled by iKan's filesystem
                            std::filesystem::path path = m_Path;
                            auto parentPath            = path.parent_path();

                            parentPath /= str;
                            std::string texturePath = parentPath.string();

                            IK_MESH_LOG("    Metalness map path = {0}", texturePath);

                            auto texture = Texture::Create(texturePath);
                            if (texture->Uploaded())
                            {
                                mi->Set("u_MetalnessTexture", texture);
                                mi->Set("u_MetalnessTexToggle", 1.0f);
                            }
                            else
                            {
                                IK_CORE_ERROR("    Could not load texture: {0}", texturePath);
                                mi->Set("u_Metalness", metalness);
                                mi->Set("u_MetalnessTexToggle", 0.0f);
                            }
                            break;
                        }
                    } // if (prop->mType == aiPTI_String)
                } // for (uint32_t i = 0; i < aiMaterial->mNumProperties; i++)

                if (!metalnessTextureFound)
                {
                    IK_MESH_LOG("    No metalness map");

                    mi->Set("u_Metalness", metalness);
                    mi->Set("u_MetalnessTexToggle", 0.0f);
                }
            } // for (uint32_t i = 0; i < scene->mNumMaterials; i++)
            IK_MESH_LOG("------------------------");
        } // if (scene->HasMaterials())

        if (m_IsAnimated)
        {
            m_VertexBuffer = VertexBuffer::Create((uint32_t)(m_AnimatedVertices.size() * sizeof(AnimatedVertex)), (float*)m_AnimatedVertices.data());
            m_VertexBuffer->AddLayout({
                { ShaderDataType::Float3, "a_Position" },
                { ShaderDataType::Float3, "a_Normal" },
                { ShaderDataType::Float3, "a_Tangent" },
                { ShaderDataType::Float3, "a_Binormal" },
                { ShaderDataType::Float2, "a_TexCoord" },
                { ShaderDataType::Int4, "a_BoneIDs" },
                { ShaderDataType::Float4, "a_BoneWeights" },
            });
        }
        else
        {
            m_VertexBuffer = VertexBuffer::Create((uint32_t)(m_StaticVertices.size() * sizeof(Vertex)), (float*)m_StaticVertices.data());
            m_VertexBuffer->AddLayout({
                { ShaderDataType::Float3, "a_Position" },
                { ShaderDataType::Float3, "a_Normal" },
                { ShaderDataType::Float3, "a_Tangent" },
                { ShaderDataType::Float3, "a_Binormal" },
                { ShaderDataType::Float2, "a_TexCoord" },
            });
        }

        m_IndexBuffer = IndexBuffer::Create((uint32_t)(m_Indices.size() * sizeof(Index)), (uint32_t*)m_Indices.data());

        m_VertexArray = VertexArray::Create();
        m_VertexArray->AddVertexBuffer(m_VertexBuffer);
        m_VertexArray->SetIndexBuffer(m_IndexBuffer);
    }

    // ******************************************************************************
    // Mesh Destructor
    // ******************************************************************************
    Mesh::~Mesh()
    {
        IK_CORE_INFO("Mesh from path {0} Destroyed", m_Path);
    }

    // ******************************************************************************
    // Travense the node recursively. Update the submesh 
    // ******************************************************************************
    void Mesh::TraverseNodes(aiNode* node, const glm::mat4& parentTransform, uint32_t level)
    {
        glm::mat4 transform = parentTransform * Mat4FromAssimpMat4(node->mTransformation);
        for (uint32_t i = 0; i < node->mNumMeshes; i++)
        {
            uint32_t mesh     = node->mMeshes[i];
            auto& submesh     = m_Submeshes[mesh];
            submesh.NodeName  = node->mName.C_Str();
            submesh.Transform = transform;
        }

        for (uint32_t i = 0; i < node->mNumChildren; i++)
            TraverseNodes(node->mChildren[i], transform, level + 1);
    }

    // ******************************************************************************
    // Update the Scene for each frame
    // ******************************************************************************
    void Mesh::OnUpdate(Timestep ts)
    {
        if (m_IsAnimated)
        {
            if (m_AnimationPlaying)
            {
                m_WorldTime += ts;

                float ticksPerSecond = (float)(m_Scene->mAnimations[0]->mTicksPerSecond != 0 ? m_Scene->mAnimations[0]->mTicksPerSecond : 25.0f) * m_TimeMultiplier;
                m_AnimationTime += ts * ticksPerSecond;
                m_AnimationTime = fmod(m_AnimationTime, (float)m_Scene->mAnimations[0]->mDuration);
            }

            // TODO: We only need to recalc bones if rendering has been requested at the current animation frame
            BoneTransform(m_AnimationTime);
        }
    }

    // ******************************************************************************
    // ******************************************************************************
    void Mesh::BoneTransform(float time)
    {
        ReadNodeHierarchy(time, m_Scene->mRootNode, glm::mat4(1.0f));
        m_BoneTransforms.resize(m_BoneCount);
        
        for (size_t i = 0; i < m_BoneCount; i++)
            m_BoneTransforms[i] = m_BoneInfo[i].FinalTransformation;
    }

    // ******************************************************************************
    // ******************************************************************************
    void Mesh::ReadNodeHierarchy(float AnimationTime, const aiNode* pNode, const glm::mat4& parentTransform)
    {
        std::string name(pNode->mName.data);
        const aiAnimation* animation = m_Scene->mAnimations[0];
        glm::mat4 nodeTransform(Mat4FromAssimpMat4(pNode->mTransformation));
        const aiNodeAnim* nodeAnim = FindNodeAnim(animation, name);

        if (nodeAnim)
        {
            glm::vec3 translation = InterpolateTranslation(AnimationTime, nodeAnim);
            glm::mat4 translationMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(translation.x, translation.y, translation.z));

            glm::quat rotation = InterpolateRotation(AnimationTime, nodeAnim);
            glm::mat4 rotationMatrix = glm::toMat4(rotation);

            glm::vec3 scale = InterpolateScale(AnimationTime, nodeAnim);
            glm::mat4 scaleMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(scale.x, scale.y, scale.z));

            nodeTransform = translationMatrix * rotationMatrix * scaleMatrix;
        }

        glm::mat4 transform = parentTransform * nodeTransform;

        if (m_BoneMapping.find(name) != m_BoneMapping.end())
        {
            uint32_t BoneIndex = m_BoneMapping[name];
            m_BoneInfo[BoneIndex].FinalTransformation = m_InverseTransform * transform * m_BoneInfo[BoneIndex].BoneOffset;
        }

        for (uint32_t i = 0; i < pNode->mNumChildren; i++)
            ReadNodeHierarchy(AnimationTime, pNode->mChildren[i], transform);
    }

    // ******************************************************************************
    // ******************************************************************************
    const aiNodeAnim* Mesh::FindNodeAnim(const aiAnimation* animation, const std::string& nodeName)
    {
        for (uint32_t i = 0; i < animation->mNumChannels; i++)
        {
            const aiNodeAnim* nodeAnim = animation->mChannels[i];
            if (std::string(nodeAnim->mNodeName.data) == nodeName)
                return nodeAnim;
        }
        return nullptr;
    }

    // ******************************************************************************
    // ******************************************************************************
    glm::vec3 Mesh::InterpolateTranslation(float animationTime, const aiNodeAnim* nodeAnim)
    {
        if (nodeAnim->mNumPositionKeys == 1)
        {
            // No interpolation necessary for single value
            auto v = nodeAnim->mPositionKeys[0].mValue;
            return { v.x, v.y, v.z };
        }

        uint32_t PositionIndex      = FindPosition(animationTime, nodeAnim);
        uint32_t NextPositionIndex  = (PositionIndex + 1);

        IK_CORE_ASSERT((NextPositionIndex < nodeAnim->mNumPositionKeys), "");
        float DeltaTime = (float)(nodeAnim->mPositionKeys[NextPositionIndex].mTime - nodeAnim->mPositionKeys[PositionIndex].mTime);
        float Factor    = (animationTime - (float)nodeAnim->mPositionKeys[PositionIndex].mTime) / DeltaTime;

        IK_CORE_ASSERT((Factor <= 1.0f), "Factor must be below 1.0f");
        Factor = glm::clamp(Factor, 0.0f, 1.0f);

        const aiVector3D& Start = nodeAnim->mPositionKeys[PositionIndex].mValue;
        const aiVector3D& End   = nodeAnim->mPositionKeys[NextPositionIndex].mValue;

        aiVector3D Delta = End - Start;
        auto aiVec       = Start + Factor * Delta;
        return { aiVec.x, aiVec.y, aiVec.z };
    }

    // ******************************************************************************
    // ******************************************************************************
    glm::quat Mesh::InterpolateRotation(float animationTime, const aiNodeAnim* nodeAnim)
    {
        if (nodeAnim->mNumRotationKeys == 1)
        {
            // No interpolation necessary for single value
            auto v = nodeAnim->mRotationKeys[0].mValue;
            return glm::quat(v.w, v.x, v.y, v.z);
        }

        uint32_t RotationIndex = FindRotation(animationTime, nodeAnim);
        uint32_t NextRotationIndex = (RotationIndex + 1);

        IK_CORE_ASSERT((NextRotationIndex < nodeAnim->mNumRotationKeys), "");
        float DeltaTime = (float)(nodeAnim->mRotationKeys[NextRotationIndex].mTime - nodeAnim->mRotationKeys[RotationIndex].mTime);
        float Factor = (animationTime - (float)nodeAnim->mRotationKeys[RotationIndex].mTime) / DeltaTime;

        IK_CORE_ASSERT((Factor <= 1.0f), "Factor must be below 1.0f");
        Factor = glm::clamp(Factor, 0.0f, 1.0f);

        const aiQuaternion& StartRotationQ = nodeAnim->mRotationKeys[RotationIndex].mValue;
        const aiQuaternion& EndRotationQ = nodeAnim->mRotationKeys[NextRotationIndex].mValue;

        auto q = aiQuaternion();
        aiQuaternion::Interpolate(q, StartRotationQ, EndRotationQ, Factor);
        q = q.Normalize();
        return glm::quat(q.w, q.x, q.y, q.z);
    }

    // ******************************************************************************
    // ******************************************************************************
    glm::vec3 Mesh::InterpolateScale(float animationTime, const aiNodeAnim* nodeAnim)
    {
        if (nodeAnim->mNumScalingKeys == 1)
        {
            // No interpolation necessary for single value
            auto v = nodeAnim->mScalingKeys[0].mValue;
            return { v.x, v.y, v.z };
        }

        uint32_t index = FindScaling(animationTime, nodeAnim);
        uint32_t nextIndex = (index + 1);

        IK_CORE_ASSERT((nextIndex < nodeAnim->mNumScalingKeys), "");
        float deltaTime = (float)(nodeAnim->mScalingKeys[nextIndex].mTime - nodeAnim->mScalingKeys[index].mTime);
        float factor = (animationTime - (float)nodeAnim->mScalingKeys[index].mTime) / deltaTime;

        IK_CORE_ASSERT((factor <= 1.0f), "Factor must be below 1.0f");
        factor = glm::clamp(factor, 0.0f, 1.0f);

        const auto& start = nodeAnim->mScalingKeys[index].mValue;
        const auto& end = nodeAnim->mScalingKeys[nextIndex].mValue;
        auto delta = end - start;
        auto aiVec = start + factor * delta;
        return { aiVec.x, aiVec.y, aiVec.z };
    }

    // ******************************************************************************
    // ******************************************************************************
    uint32_t Mesh::FindPosition(float AnimationTime, const aiNodeAnim* pNodeAnim)
    {
        for (uint32_t i = 0; i < pNodeAnim->mNumPositionKeys - 1; i++)
        {
            if (AnimationTime < (float)pNodeAnim->mPositionKeys[i + 1].mTime)
                return i;
        }

        return 0;
    }

    // ******************************************************************************
    // ******************************************************************************
    uint32_t Mesh::FindRotation(float AnimationTime, const aiNodeAnim* pNodeAnim)
    {
        IK_CORE_ASSERT((pNodeAnim->mNumRotationKeys > 0), "");

        for (uint32_t i = 0; i < pNodeAnim->mNumRotationKeys - 1; i++)
        {
            if (AnimationTime < (float)pNodeAnim->mRotationKeys[i + 1].mTime)
                return i;
        }

        return 0;
    }

    // ******************************************************************************
    // ******************************************************************************
    uint32_t Mesh::FindScaling(float AnimationTime, const aiNodeAnim* pNodeAnim)
    {
        IK_CORE_ASSERT((pNodeAnim->mNumScalingKeys > 0), "");

        for (uint32_t i = 0; i < pNodeAnim->mNumScalingKeys - 1; i++)
        {
            if (AnimationTime < (float)pNodeAnim->mScalingKeys[i + 1].mTime)
                return i;
        }

        return 0;
    }

    // ******************************************************************************
    // ******************************************************************************
    void Mesh::DumpVertexBuffer()
    {
        // TODO: Convert to ImGui
        IK_MESH_LOG("------------------------------------------------------");
        IK_MESH_LOG("Vertex Buffer Dump");
        IK_MESH_LOG("Mesh: {0}", m_Path);
        if (m_IsAnimated)
        {
            for (size_t i = 0; i < m_AnimatedVertices.size(); i++)
            {
                IK_MESH_LOG("Vertex: {0}", i);
                IK_MESH_LOG("Position: {0}, {1}, {2}", m_AnimatedVertices[i].Position.x, m_AnimatedVertices[i].Position.y, m_AnimatedVertices[i].Position.z);
                IK_MESH_LOG("Normal: {0}, {1}, {2}", m_AnimatedVertices[i].Normal.x, m_AnimatedVertices[i].Normal.y, m_AnimatedVertices[i].Normal.z);
                IK_MESH_LOG("Binormal: {0}, {1}, {2}", m_AnimatedVertices[i].Binormal.x, m_AnimatedVertices[i].Binormal.y, m_AnimatedVertices[i].Binormal.z);
                IK_MESH_LOG("Tangent: {0}, {1}, {2}", m_AnimatedVertices[i].Tangent.x, m_AnimatedVertices[i].Tangent.y, m_AnimatedVertices[i].Tangent.z);
                IK_MESH_LOG("TexCoord: {0}, {1}", m_AnimatedVertices[i].Texcoord.x, m_AnimatedVertices[i].Texcoord.y);
                IK_MESH_LOG("--");
            }
        }
        else
        {
            for (size_t i = 0; i < m_StaticVertices.size(); i++)
            {
                IK_MESH_LOG("Vertex: {0}", i);
                IK_MESH_LOG("Position: {0}, {1}, {2}", m_StaticVertices[i].Position.x, m_StaticVertices[i].Position.y, m_StaticVertices[i].Position.z);
                IK_MESH_LOG("Normal: {0}, {1}, {2}", m_StaticVertices[i].Normal.x, m_StaticVertices[i].Normal.y, m_StaticVertices[i].Normal.z);
                IK_MESH_LOG("Binormal: {0}, {1}, {2}", m_StaticVertices[i].Binormal.x, m_StaticVertices[i].Binormal.y, m_StaticVertices[i].Binormal.z);
                IK_MESH_LOG("Tangent: {0}, {1}, {2}", m_StaticVertices[i].Tangent.x, m_StaticVertices[i].Tangent.y, m_StaticVertices[i].Tangent.z);
                IK_MESH_LOG("TexCoord: {0}, {1}", m_StaticVertices[i].Texcoord.x, m_StaticVertices[i].Texcoord.y);
                IK_MESH_LOG("--");
            }
        }
        IK_MESH_LOG("------------------------------------------------------");
    }
}
