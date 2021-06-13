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
        : ID(Id)
        {
             
        }
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
        : Tag(tag)
        {
            
        }
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
        : Translation(translation)
        {
            
        }
        
        // ******************************************************************************
        // Gets the transform Matrix from translation , scale and rotation
        // ******************************************************************************
        glm::mat4 GetTransform() const
        {
            glm::mat4 rotation = glm::toMat4(glm::quat(Rotation));
            return glm::translate(glm::mat4(1.0f), Translation) * rotation * glm::scale(glm::mat4(1.0f), Scale);
        }
    };
    
    // ******************************************************************************
    // Stores the Camera Component
    // ******************************************************************************
    struct CameraComponent
    {
        SceneCamera Camera;

        bool Primary = true;
        bool FixedAspectRatio = false;

        ~CameraComponent() = default;
        
        CameraComponent() = default;
        CameraComponent(const CameraComponent&) = default;
    };
    
    // ******************************************************************************
    // Stores the Sprite Component for Texture, Sprite and color
    // ******************************************************************************
    struct SpriteRendererComponent
    {
        glm::vec4       ColorComp   = glm::vec4(1.0f);
        Ref<SubTexture> SubTexComp  = nullptr;
        Ref<Texture>    TextureComp = nullptr;
        
        float TilingFactor = 1.0f;

        // ******************************************************************************
        // Upload new texture and reset the subtexture component
        // ******************************************************************************
        Ref<Texture> UploadTexture(const Ref<Texture>& texture)
        {
            // If texture is already created then delete the texture
            // (if shared with other entity then reduce the counter)
            if (TextureComp)
            {
                TextureComp.reset();
            }

            // If component is subtexture then overrite it with texture
            if (SubTexComp)
            {
                SubTexComp.reset();
            }

            SubTexComp  = nullptr;
            TextureComp = texture;

            // If texture is uploaded with invalid path So delete the texture
            if (!TextureComp->Uploaded())
            {
                TextureComp.reset();
            }

            return TextureComp;
        }

        // ******************************************************************************
        // Upload new texture and reset the subtexture component
        // ******************************************************************************
        Ref<Texture> UploadTexture(const std::string& path)
        {
            UploadTexture(Texture::Create(path));
            return TextureComp;
        }

        // ******************************************************************************
        // Upload new texture and reset the subtexture component
        // ******************************************************************************
        void ResetAllComponents()
        {
            // If texture is already created then delete the texture
            // (if shared with other entity then reduce the counter)
            if (TextureComp)
            {
                TextureComp.reset();
            }
            TextureComp = nullptr;

            // If texture is already created then delete the texture
            // (if shared with other entity then reduce the counter)
            if (SubTexComp)
            {
                SubTexComp.reset();
            }
            SubTexComp = nullptr;
        }

        ~SpriteRendererComponent() = default;

        SpriteRendererComponent() = default;
        SpriteRendererComponent(const SpriteRendererComponent&) = default;
        
        SpriteRendererComponent(const glm::vec4& color)
        : ColorComp(color), SubTexComp(nullptr), TextureComp(nullptr)
        {
            
        }
        
        SpriteRendererComponent(const Ref<SubTexture>& subtexture)
        : ColorComp(glm::vec4(1.0f)), SubTexComp(subtexture), TextureComp(nullptr)
        {
            
        }
        
        SpriteRendererComponent(const Ref<iKan::Texture>& texture)
        : ColorComp(glm::vec4(1.0f)), SubTexComp(nullptr), TextureComp(texture)
        {
            
        }
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
        : IsProp(flag)
        {
            
        }
    };

    // ******************************************************************************
    // 2D Box colloider compoennt. If added them 2D box is Rigid
    // ******************************************************************************
    struct BoxCollider2DComponent
    {
        bool IsRigid = false;

        ~BoxCollider2DComponent() = default;

        BoxCollider2DComponent() = default;
        BoxCollider2DComponent(const BoxCollider2DComponent& other) = default;

        BoxCollider2DComponent(bool flag)
        : IsRigid(flag)
        {

        }
    };

}
