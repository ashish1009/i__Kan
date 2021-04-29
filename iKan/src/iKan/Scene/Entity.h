// ******************************************************************************
// File         : Entity.h
// Project      : iKan : Scene
// Description  : Wrapper Entity Class to store EnTT
//
// Created on   : 28/04/21.
// ******************************************************************************

#pragma once

#include <iKan/Scene/Scene.h>

namespace iKan {
    
    class Entity
    {
    public:
        Entity() = default;
        Entity(const Entity& other) = default;
        
        Entity(entt::entity handle, Ref<Scene> scene)
        : m_EntityHandle(handle), m_Scene(scene)
        {
            
        }

        ~Entity() = default;
        
    private:
        entt::entity m_EntityHandle{ entt::null };
        Ref<Scene>   m_Scene;
    };
    
}
