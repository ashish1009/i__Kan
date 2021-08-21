// ******************************************************************************
// File         : SceneSerializer.cpp
// Description  : Saves and stores the scene data
// Project      : iKan
//
// Created by Ashish on 14/06/21.
// Copyright © 2021 Ashish. All rights reserved.
// ******************************************************************************

#include "SceneSerializer.h"

#include <iKan/Scene/Entity.h>
#include <iKan/Scene/Component.h>

#pragma clang diagnostic ignored "-Wdocumentation"
#include <yaml-cpp/yaml.h>

namespace YAML {

    // ******************************************************************************
    // yml converstions vec2
    // ******************************************************************************
    template<>
    struct convert<glm::vec2>
    {
        static Node encode(const glm::vec2& rhs)
        {
            Node node;
            node.push_back(rhs.x);
            node.push_back(rhs.y);
            node.SetStyle(EmitterStyle::Flow);
            return node;
        }

        static bool decode(const Node& node, glm::vec2& rhs)
        {
            if (!node.IsSequence() || node.size() != 2)
                return false;

            rhs.x = node[0].as<float>();
            rhs.y = node[1].as<float>();
            return true;
        }
    };

    // ******************************************************************************
    // yml converstions vec3
    // ******************************************************************************
    template<>
    struct convert<glm::vec3>
    {
        static Node encode(const glm::vec3& rhs)
        {
            Node node;
            node.push_back(rhs.x);
            node.push_back(rhs.y);
            node.push_back(rhs.z);
            node.SetStyle(EmitterStyle::Flow);
            return node;
        }

        static bool decode(const Node& node, glm::vec3& rhs)
        {
            if (!node.IsSequence() || node.size() != 3)
                return false;

            rhs.x = node[0].as<float>();
            rhs.y = node[1].as<float>();
            rhs.z = node[2].as<float>();
            return true;
        }
    };

    // ******************************************************************************
    // yml converstions
    // ******************************************************************************
    template<>
    struct convert<glm::vec4>
    {
        static Node encode(const glm::vec4& rhs)
        {
            Node node;
            node.push_back(rhs.x);
            node.push_back(rhs.y);
            node.push_back(rhs.z);
            node.push_back(rhs.w);
            node.SetStyle(EmitterStyle::Flow);
            return node;
        }

        static bool decode(const Node& node, glm::vec4& rhs)
        {
            if (!node.IsSequence() || node.size() != 4)
                return false;

            rhs.x = node[0].as<float>();
            rhs.y = node[1].as<float>();
            rhs.z = node[2].as<float>();
            rhs.w = node[3].as<float>();
            return true;
        }
    };

}

namespace iKan {

    // ******************************************************************************
    // yml << operator for glm vec 2
    // ******************************************************************************
    YAML::Emitter& operator<<(YAML::Emitter& out, const glm::vec2& v)
    {
        out << YAML::Flow;
        out << YAML::BeginSeq << v.x << v.y << YAML::EndSeq;
        return out;
    }

    // ******************************************************************************
    // yml << operator for glm vec 3
    // ******************************************************************************
    YAML::Emitter& operator<<(YAML::Emitter& out, const glm::vec3& v)
    {
        out << YAML::Flow;
        out << YAML::BeginSeq << v.x << v.y << v.z << YAML::EndSeq;
        return out;
    }

    // ******************************************************************************
    // yml << operator for glm vec 4
    // ******************************************************************************
    YAML::Emitter& operator<<(YAML::Emitter& out, const glm::vec4& v)
    {
        out << YAML::Flow;
        out << YAML::BeginSeq << v.x << v.y << v.z << v.w << YAML::EndSeq;
        return out;
    }

    // ******************************************************************************
    // SceneSerializer constructor
    // ******************************************************************************
    SceneSerializer::SceneSerializer(const Ref<Scene>& scene)
    : m_Scene(scene)
    {
        IK_CORE_INFO("Serialising a scene ...");
    }
    
    // ******************************************************************************
    // SceneSerializer destructor
    // ******************************************************************************
    SceneSerializer::~SceneSerializer()
    {
        IK_CORE_INFO("Scene Serialised ...");
    }

    // ******************************************************************************
    // Serialize the entity. Extract the entity componenets from serialiser
    // ******************************************************************************
    static void SerializeEntity(YAML::Emitter& out, Entity entity)
    {
        UUID uuid = entity.GetComponent<IDComponent>().ID;

        out << YAML::BeginMap; // Entity
        out << YAML::Key << "Entity" << YAML::Value << uuid; // TODO: Entity ID goes here

        if (entity.HasComponent<TagComponent>())
        {
            out << YAML::Key << "TagComponent";
            out << YAML::BeginMap; // TagComponent

            auto& tag = entity.GetComponent<TagComponent>().Tag;
            out << YAML::Key << "Tag" << YAML::Value << tag;

            out << YAML::EndMap; // TagComponent
        }

        if (entity.HasComponent<TransformComponent>())
        {
            out << YAML::Key << "TransformComponent";
            out << YAML::BeginMap; // TransformComponent

            auto& tc = entity.GetComponent<TransformComponent>();
            out << YAML::Key << "Translation" << YAML::Value << tc.Translation;
            out << YAML::Key << "Rotation" << YAML::Value << tc.Rotation;
            out << YAML::Key << "Scale" << YAML::Value << tc.Scale;

            out << YAML::EndMap; // TransformComponent
        }

        if (entity.HasComponent<CameraComponent>())
        {
            out << YAML::Key << "CameraComponent";
            out << YAML::BeginMap; // CameraComponent

            auto& cameraComponent = entity.GetComponent<CameraComponent>();
            auto& camera = cameraComponent.Camera;

            out << YAML::Key << "Camera" << YAML::Value;
            out << YAML::BeginMap; // Camera
            out << YAML::Key << "ProjectionType" << YAML::Value << (int)camera.GetProjectionType();
            out << YAML::Key << "PerspectiveFOV" << YAML::Value << camera.GetPerspectiveFOV();
            out << YAML::Key << "PerspectiveNear" << YAML::Value << camera.GetPerspectiveNearClip();
            out << YAML::Key << "PerspectiveFar" << YAML::Value << camera.GetPerspectiveFarClip();
            out << YAML::Key << "OrthographicSize" << YAML::Value << camera.GetOrthographicSize();
            out << YAML::Key << "OrthographicNear" << YAML::Value << camera.GetOrthographicNearClip();
            out << YAML::Key << "OrthographicFar" << YAML::Value << camera.GetOrthographicFarClip();
            out << YAML::EndMap; // Camera

            out << YAML::Key << "Primary" << YAML::Value << cameraComponent.Primary;
            out << YAML::Key << "Editor" << YAML::Value << cameraComponent.Editor;
            out << YAML::Key << "FixedAspectRatio" << YAML::Value << cameraComponent.FixedAspectRatio;

            out << YAML::EndMap; // CameraComponent
        }

        if (entity.HasComponent<SpriteRendererComponent>())
        {
            out << YAML::Key << "SpriteRendererComponent";
            out << YAML::BeginMap; // SpriteRendererComponent

            auto& spriteRendererComponent = entity.GetComponent<SpriteRendererComponent>();
            out << YAML::Key << "Color" << YAML::Value << spriteRendererComponent.ColorComp;

            out << YAML::Key << "TilingFactor" << YAML::Value << spriteRendererComponent.TilingFactor;

            if (spriteRendererComponent.Texture.Component)
            {
                out << YAML::Key << "IsTexture" << YAML::Value << (bool)true;
                out << YAML::Key << "IsSubTexture" << YAML::Value << (bool)false;

                out << YAML::Key << "TexAssetUse" << YAML::Value << spriteRendererComponent.Texture.Use;
                out << YAML::Key << "TexAssetPath" << YAML::Value << spriteRendererComponent.Texture.Component->GetfilePath();
            }
            else if (spriteRendererComponent.SubTexComp)
            {
                out << YAML::Key << "IsTexture" << YAML::Value << (bool)false;
                out << YAML::Key << "IsSubTexture" << YAML::Value << (bool)true;

                out << YAML::Key << "TexAssetPath" << YAML::Value << spriteRendererComponent.SubTexComp->GetTexture()->GetfilePath();

                out << YAML::Key << "Coords" << YAML::Value << spriteRendererComponent.SubTexComp->GetCoords();
                out << YAML::Key << "SpriteSize" << YAML::Value << spriteRendererComponent.SubTexComp->GetSpriteSize();
                out << YAML::Key << "CellSize" << YAML::Value << spriteRendererComponent.SubTexComp->GetCellSize();
            }
            else
            {
                out << YAML::Key << "IsTexture" << YAML::Value << (bool)false;
                out << YAML::Key << "IsSubTexture" << YAML::Value << (bool)false;

                out << YAML::Key << "TexAssetPath" << YAML::Value << "";
            }

            // TODO: Add subtexture later
            out << YAML::EndMap; // SpriteRendererComponent
        }

        if (entity.HasComponent<SceneHierarchyPannelProp>())
        {
            out << YAML::Key << "SceneHierarchyPannelProp";
            out << YAML::BeginMap; // SceneHierarchyPannelProp

            auto& schComp = entity.GetComponent<SceneHierarchyPannelProp>();
            out << YAML::Key << "IsProp" << YAML::Value << schComp.IsProp;

            out << YAML::EndMap; // SceneHierarchyPannelProp

        }

        if (entity.HasComponent<BoxCollider2DComponent>())
        {
            out << YAML::Key << "BoxCollider2DComponent";
            out << YAML::BeginMap; // BoxCollider2DComponent

            auto& boxColComp = entity.GetComponent<BoxCollider2DComponent>();
            out << YAML::Key << "IsRigid" << YAML::Value << boxColComp.IsRigid;

            out << YAML::EndMap; // BoxCollider2DComponent

        }

        out << YAML::EndMap; // Entity
    }

    // ******************************************************************************
    // Serialize the Scene each enityt
    // ******************************************************************************
    void SceneSerializer::Serialize(const std::string& filepath)
    {
        YAML::Emitter out;
        out << YAML::BeginMap;
        out << YAML::Key << "Scene" << YAML::Value << "Untitled";
        out << YAML::Key << "Entities" << YAML::Value << YAML::BeginSeq;
        m_Scene->m_Registry.each([&](auto entityID)
                                 {
            Entity entity = { entityID, m_Scene.get() };
            if (!entity)
                return;

            SerializeEntity(out, entity);
        });
        out << YAML::EndSeq;
        out << YAML::EndMap;

        std::ofstream fout(filepath);
        fout << out.c_str();
    }

    // ******************************************************************************
    // Serialize the Scene Runtime
    // ******************************************************************************
    void SceneSerializer::SerializeRuntime(const std::string& filepath)
    {
        // Not implemented
        IK_CORE_ASSERT(false, "Not Implemented Yet");
    }

    // ******************************************************************************
    // Deserialize the Scene Runtime
    // ******************************************************************************
    bool SceneSerializer::Deserialize(const std::string& filepath)
    {
        auto& texMap = m_Scene->GetDataRef().TextureMap;

        YAML::Node data = YAML::LoadFile(filepath);

        if (!data["Scene"])
            return false;

        std::string sceneName = data["Scene"].as<std::string>();
        IK_CORE_TRACE("Deserializing scene '{0}'", sceneName);

        auto entities = data["Entities"];
        if (entities)
        {
            for (auto entity : entities)
            {
                uint64_t uuid = entity["Entity"].as<uint64_t>(); // TODO

                std::string name;
                auto tagComponent = entity["TagComponent"];
                if (tagComponent)
                    name = tagComponent["Tag"].as<std::string>();

                IK_CORE_TRACE("Deserialized entity with ID = {0}, name = {1}", uuid, name);

                Entity deserializedEntity = m_Scene->CreateEntity(name, uuid);

                auto transformComponent = entity["TransformComponent"];
                if (transformComponent)
                {
                    // Entities always have transforms
                    auto& tc = deserializedEntity.GetComponent<TransformComponent>();
                    tc.Translation = transformComponent["Translation"].as<glm::vec3>();
                    tc.Rotation = transformComponent["Rotation"].as<glm::vec3>();
                    tc.Scale = transformComponent["Scale"].as<glm::vec3>();

                    IK_CORE_INFO("  Entity Transform:");
                    IK_CORE_INFO("      Translation: {0}, {1}, {2}", tc.Translation.x, tc.Translation.y, tc.Translation.z);
                    IK_CORE_INFO("      Rotation: {0}, {1}, {2}", tc.Rotation.x, tc.Rotation.y, tc.Rotation.z);
                    IK_CORE_INFO("      Scale: {0}, {1}, {2}", tc.Scale.x, tc.Scale.y, tc.Scale.z);
                }

                auto cameraComponent = entity["CameraComponent"];
                if (cameraComponent)
                {
                    auto& cc = deserializedEntity.AddComponent<CameraComponent>();

                    auto cameraProps = cameraComponent["Camera"]; // TODO: It was reference
                    cc.Camera.SetProjectionType((SceneCamera::ProjectionType)cameraProps["ProjectionType"].as<int>());

                    cc.Camera.SetPerspectiveFOV(cameraProps["PerspectiveFOV"].as<float>());
                    cc.Camera.SetPerspectiveNearClip(cameraProps["PerspectiveNear"].as<float>());
                    cc.Camera.SetPerspectiveFarClip(cameraProps["PerspectiveFar"].as<float>());

                    cc.Camera.SetOrthographicSize(cameraProps["OrthographicSize"].as<float>());
                    cc.Camera.SetOrthographicNearClip(cameraProps["OrthographicNear"].as<float>());
                    cc.Camera.SetOrthographicFarClip(cameraProps["OrthographicFar"].as<float>());

                    cc.Primary = cameraComponent["Primary"].as<bool>();
                    cc.Editor = cameraComponent["Editor"].as<bool>();
                    cc.FixedAspectRatio = cameraComponent["FixedAspectRatio"].as<bool>();

                    IK_CORE_INFO("  Entity Camera:");
                    IK_CORE_INFO("      Primary: {0}", cc.Primary);
                    IK_CORE_INFO("      Editor: {0}", cc.Editor);
                    IK_CORE_INFO("      FixedAspectRatio: {0}", cc.FixedAspectRatio);
                    IK_CORE_INFO("          Projection Type: {0}", cc.Camera.GetProjectionType());

                    IK_CORE_INFO("              PerspectiveFOV: {0}", cc.Camera.GetPerspectiveFOV());
                    IK_CORE_INFO("              PerspectiveFar: {0}", cc.Camera.GetPerspectiveFarClip());
                    IK_CORE_INFO("              PerspectiveNear: {0}", cc.Camera.GetPerspectiveNearClip());

                    IK_CORE_INFO("              OrthographicSize: {0}", cc.Camera.GetOrthographicSize());
                    IK_CORE_INFO("              OrthographicNear: {0}", cc.Camera.GetOrthographicNearClip());
                    IK_CORE_INFO("              OrthographicFar: {0}",  cc.Camera.GetOrthographicFarClip());
                }

                auto spriteRendererComponent = entity["SpriteRendererComponent"];
                if (spriteRendererComponent)
                {
                    // TODO: Add for Texutre
                    auto& src = deserializedEntity.AddComponent<SpriteRendererComponent>();
                    src.ColorComp = spriteRendererComponent["Color"].as<glm::vec4>();

                    src.TilingFactor = spriteRendererComponent["TilingFactor"].as<float>();

                    bool isTexture    = spriteRendererComponent["IsTexture"].as<bool>();
                    bool isSubTexture = spriteRendererComponent["IsSubTexture"].as<bool>();

                    std::string texPath = spriteRendererComponent["TexAssetPath"].as<std::string>();
                    if (texPath != "")
                    {
                        Ref<Texture> texture = nullptr;
                        if (texMap.find(texPath) != texMap.end())
                        {
                            texture = texMap[texPath];
                        }
                        else
                        {
                            texture = Texture::Create(texPath);
                            texMap[texPath] = texture;
                        }

                        if (isTexture)
                        {
                            src.Texture.Component = texture;
                            src.Texture.Use = spriteRendererComponent["TexAssetUse"].as<bool>();
                        }
                        else if (isSubTexture)
                        {
                            glm::vec2 coords     = spriteRendererComponent["Coords"].as<glm::vec2>();
                            glm::vec2 spriteSize = spriteRendererComponent["SpriteSize"].as<glm::vec2>();
                            glm::vec2 cellSize   = spriteRendererComponent["CellSize"].as<glm::vec2>();

                            src.SubTexComp = SubTexture::CreateFromCoords(texture, coords, spriteSize, cellSize);
                        }
                        else
                        {
                            src.SubTexComp  = nullptr;
                            src.Texture.Component = nullptr;
                        }
                    }
                    else
                    {
                        src.SubTexComp  = nullptr;
                        src.Texture.Component = nullptr;
                    }

                    IK_CORE_INFO("  Entity Sprite:");

                    IK_CORE_INFO("      Color: {0}, {1}, {2}", src.ColorComp.x, src.ColorComp.y, src.ColorComp.z);
                    IK_CORE_INFO("      Texture Tiling Factor: {0}", src.TilingFactor);
                    IK_CORE_INFO("      Texture Asset Path: {0}", texPath);
                }

                auto schComp = entity["SceneHierarchyPannelProp"];
                if (schComp)
                {
                    auto& schc  = deserializedEntity.GetComponent<SceneHierarchyPannelProp>();
                    schc.IsProp = schComp["IsProp"].as<bool>();

                    IK_CORE_INFO("  Scene Hierarchy Pannel Property:");
                    IK_CORE_INFO("      Is Property pannel present : {0}", schc.IsProp);
                }

                auto boxColComp = entity["BoxCollider2DComponent"];
                if (boxColComp)
                {
                    auto& bcc   = deserializedEntity.GetComponent<BoxCollider2DComponent>();
                    bcc.IsRigid = boxColComp["IsRigid"].as<bool>();

                    IK_CORE_INFO("  BoxCollider 2D Component:");
                    IK_CORE_INFO("      Is Rigid object : {0}", bcc.IsRigid);
                }
            }
        }

        return true;
    }

    bool SceneSerializer::DeserializeRuntime(const std::string& filepath)
    {
        // Not implemented
        IK_CORE_ASSERT(false, "Not Implemented Yet");
        return false;
    }

}
