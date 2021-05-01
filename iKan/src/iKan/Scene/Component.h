// ******************************************************************************
// File         : Component.h
// Project      : iKan : Scene
// Description  : Structures of various components
//
// Created on   : 29/04/21.
// ******************************************************************************

#pragma once

#include <iKan/Core/UUID.h>
#include <iKan/Scene/SceneCamera.h>

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
    
}
