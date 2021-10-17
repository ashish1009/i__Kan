// ******************************************************************************
// File         : Component.cpp
// Description  : definations
// Project      : iKan : Scene
//
// Created by Ashish on 04/10/21.
// Copyright © 2021 Ashish. All rights reserved.
// ******************************************************************************

#include "Component.h"

namespace iKan {
    
    std::unordered_map<std::string, ScriptableEntity*> NativeScriptComponent::ScriptsMap;
    
    // ******************************************************************************
    // Gets the transform Matrix from translation , scale and rotation
    // ******************************************************************************
    glm::mat4 TransformComponent::GetTransform() const
    {
        glm::mat4 rotation = glm::toMat4(glm::quat(Rotation));
        return glm::translate(glm::mat4(1.0f), Translation) * rotation * glm::scale(glm::mat4(1.0f), Scale);
    }

    // ******************************************************************************
    // Make the Camera Primary
    // ******************************************************************************
    void CameraComponent::MakePrimary()
    {
        mPrimary = true;
        mEditor = false;
    }
    
    // ******************************************************************************
    // Make the Camera Editor
    // ******************************************************************************
    void CameraComponent::MakeEditor()
    {
        mPrimary = false;
        mEditor = true;
    }
    
    // ******************************************************************************
    // Upload new texture and reset the subtexture component
    // ******************************************************************************
    Ref<iKan::Texture> SpriteRendererComponent::UploadTexture(const Ref<iKan::Texture>& texture)
    {
        // If texture is already created then delete the texture
        // (if shared with other entity then reduce the counter)
        if (Texture.Component)
            Texture.Component.reset();
        
        // If component is subtexture then overrite it with texture
        if (SubTexComp)
            SubTexComp.reset();
        
        SubTexComp  = nullptr;
        
        // Image Texture component
        Texture.Component = texture;
        Texture.Use = true;
        
        // If texture is uploaded with invalid path So delete the texture
        if (!Texture.Component->Uploaded())
            Texture.Component.reset();
        
        return Texture.Component;
    }
    
    // ******************************************************************************
    // Upload new texture and reset the subtexture component
    // ******************************************************************************
    Ref<iKan::Texture> SpriteRendererComponent::UploadTexture(const std::string& path)
    {
        UploadTexture(iKan::Texture::Create(path));
        return Texture.Component;
    }
    
    // ******************************************************************************
    // Addthe subtexture compnent to the current entity
    // ******************************************************************************
    Ref<SubTexture> SpriteRendererComponent::AddSubTexture()
    {
        SubTexComp = SubTexture::CreateFromCoords(Texture.Component, glm::vec2(1.0f));
        
        // If texture is already created then delete the texture
        // (if shared with other entity then reduce the counter)
        if (Texture.Component)
            Texture.Component.reset();
        
        Texture.Component = nullptr;
        Texture.Use = false;
        
        return SubTexComp;
    }
    
    // ******************************************************************************
    // Remove subtexture compnent to the current entity
    // ******************************************************************************
    void SpriteRendererComponent::RemoveSubTexture()
    {
        Texture.Component = SubTexComp->GetTexture();
        Texture.Use = true;
        
        // If texture is already created then delete the texture
        // (if shared with other entity then reduce the counter)
        if (SubTexComp)
            SubTexComp.reset();
        
        SubTexComp = nullptr;
    }
    
    // ******************************************************************************
    // Upload new texture and reset the subtexture component
    // ******************************************************************************
    void SpriteRendererComponent::ResetAllComponents()
    {
        // If texture is already created then delete the texture
        // (if shared with other entity then reduce the counter)
        if (Texture.Component)
            Texture.Component.reset();
        
        Texture.Component = nullptr;
        Texture.Use = false;
        
        // If texture is already created then delete the texture
        // (if shared with other entity then reduce the counter)
        if (SubTexComp)
        {
            SubTexComp.reset();
        }
        SubTexComp = nullptr;
    }


}
