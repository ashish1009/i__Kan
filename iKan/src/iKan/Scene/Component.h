// ******************************************************************************
// File         : Component.h
// Description  : Structures of various components
// Project      : iKan : Scene
//
// Created by Ashish on 29/04/21.
// Copyright © 2021 Ashish. All rights reserved.
// ******************************************************************************

#pragma once

#include <iKan/Core/UUID.h>
#include <iKan/Scene/SceneCamera.h>
#include <iKan/Scene/Scene.h>
#include <iKan/Renderer/Texture.h>

namespace iKan {
    
    // ******************************************************************************
    // Stores the Universal unique ID for component
    // ******************************************************************************
    struct IDComponent
    {
        UUID ID = 0;
        
        ~IDComponent() = default;
        
        IDComponent() = default;
        IDComponent(const IDComponent&) = default;
        IDComponent(const UUID& Id)
        : ID(Id) { }
    };
    
    // ******************************************************************************
    // Stores the Tag or Title of component
    // ******************************************************************************
    struct TagComponent
    {
        std::string Tag = "Default Entity";
        
        ~TagComponent() = default;
        
        TagComponent() = default;
        TagComponent(const TagComponent&) = default;
        TagComponent(const std::string& tag)
        : Tag(tag) { }
    };
    
    // ******************************************************************************
    // Stores the Transform component
    // ******************************************************************************
    struct TransformComponent
    {
        glm::vec3 Translation   = { 0.0f, 0.0f, 0.0f };
        glm::vec3 Rotation      = { 0.0f, 0.0f, 0.0f };
        glm::vec3 Scale         = { 1.0f, 1.0f, 1.0f };
        
        ~TransformComponent() = default;
        
        TransformComponent() = default;
        TransformComponent(const TransformComponent&) = default;
        TransformComponent(const glm::vec3& translation)
        : Translation(translation) { }
        
        glm::mat4 GetTransform() const;
    };
    
    // ******************************************************************************
    // Stores the Camera Component
    // ******************************************************************************
    struct CameraComponent
    {
        SceneCamera Camera;
        bool FixedAspectRatio = false;

        ~CameraComponent() = default;
        
        CameraComponent() = default;
        CameraComponent(const CameraComponent&) = default;
        
        void MakePrimary();
        void MakeEditor();
        
        // ******************************************************************************
        // Return status of Primary and Editor
        // ******************************************************************************
        bool GetPrimary() const { return mPrimary; }
        bool GetEditor() const { return mEditor; }
        
    private:
        bool mPrimary = true;
        bool mEditor  = false;
    };
    
    // ******************************************************************************
    // Stores the Sprite Component for Texture, Sprite and color
    // ******************************************************************************
    struct SpriteRendererComponent
    {
        // ******************************************************************************
        // Structure store the refernece pointer of Texture and bool to
        // ******************************************************************************
        struct ImageTexture
        {
            bool Use = false;
            Ref<iKan::Texture> Component = nullptr;
        };

        // Variables
        glm::vec4       ColorComp  = glm::vec4(1.0f);
        Ref<SubTexture> SubTexComp = nullptr;
        ImageTexture    Texture;

        float TilingFactor = 1.0f;

        Ref<iKan::Texture> UploadTexture(const Ref<iKan::Texture>& texture);
        Ref<iKan::Texture> UploadTexture(const std::string& path);
        Ref<SubTexture> AddSubTexture();
        void RemoveSubTexture();
        void ResetAllComponents();

        ~SpriteRendererComponent() = default;

        SpriteRendererComponent() = default;
        SpriteRendererComponent(const SpriteRendererComponent&) = default;
        
        SpriteRendererComponent(const glm::vec4& color)
        : ColorComp(color), SubTexComp(nullptr) { }
        
        SpriteRendererComponent(const Ref<SubTexture>& subtexture)
        : ColorComp(glm::vec4(1.0f)), SubTexComp(subtexture) { }
        
        SpriteRendererComponent(const Ref<iKan::Texture>& texture)
        : ColorComp(glm::vec4(1.0f)), SubTexComp(nullptr) { }
    };
    
    // ******************************************************************************
    // Stores a flag to check if we want to show this entity on Scene Hierarchy
    // Pannel
    // ******************************************************************************
    struct SceneHierarchyPannelProp
    {
        bool IsProp = true;

        ~SceneHierarchyPannelProp() = default;

        SceneHierarchyPannelProp() = default;
        SceneHierarchyPannelProp(const SceneHierarchyPannelProp& other) = default;
        
        SceneHierarchyPannelProp(bool flag)
        : IsProp(flag) { }
    };

    // ******************************************************************************
    // 2D Box colloider compoennt. If added them 2D box is Rigid
    // ******************************************************************************
    struct AABBColloiderComponent
    {
        bool IsRigid = false;

        ~AABBColloiderComponent() = default;

        AABBColloiderComponent() = default;
        AABBColloiderComponent(const AABBColloiderComponent& other) = default;

        AABBColloiderComponent(bool flag)
        : IsRigid(flag) { }
    };
    
    // ******************************************************************************
    // Native Script component
    // ******************************************************************************
    class ScriptableEntity;
    struct NativeScriptComponent
    {
    public:
        std::vector<ScriptableEntity*> Scripts;
        
        template<typename T>
        void Bind()
        {
            Instance = static_cast<ScriptableEntity*>(new T());
            Scripts.emplace_back(Instance);
        }
        
    private:
        ScriptableEntity* Instance = nullptr;
    };

    // ******************************************************************************
    // RigidBody2DComponent
    // ******************************************************************************
    struct RigidBody2DComponent
    {
        enum class BodyType { Static = 0, Dynamic = 1, Kinematic = 2 };
        BodyType Type = BodyType::Static;
        bool FixedRotation = false;
        
        // Storage
        void* RuntimeBody = nullptr;
        
        ~RigidBody2DComponent() = default;
        
        RigidBody2DComponent() = default;
        RigidBody2DComponent(const RigidBody2DComponent& other) = default;
    };
    
    // ******************************************************************************
    // BoxColloiderComponent
    // ******************************************************************************
    struct BoxColloider2DComponent
    {
        glm::vec2 Offset = { 0.0f, 0.0f };
        glm::vec2 Size = { 0.5f, 0.5f };
        
        // TODO: Move to physics material
        float Density  = 1.0f;
        float Friction = 0.5f;
        float Restitution = 0.0f;
        float RestitutionThreshold = 0.5f;
        
        // Storage
        void* RuntimeFixture = nullptr;
        
        ~BoxColloider2DComponent() = default;
        
        BoxColloider2DComponent() = default;
        BoxColloider2DComponent(const BoxColloider2DComponent& other) = default;
    };
        
}
