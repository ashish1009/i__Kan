// ******************************************************************************
// File         : Entity.h
// Description  : Wrapper Entity Class to store EnTT
// Project      : iKan : Scene
//
// Created by Ashish on 28/04/21.
// Copyright © 2021 Ashish. All rights reserved.
// ******************************************************************************

#pragma once

#include <iKan/Scene/Scene.h>
#include <iKan/Scene/Component.h>

namespace iKan {
    
    class Entity
    {
    public:
        Entity() = default;
        Entity(const Entity& other) = default;
        
        ~Entity() = default;

        // ******************************************************************************
        // Entity constructure that takes entt and Scene pointer
        // ******************************************************************************
        Entity(entt::entity handle, Scene* scene)
        : m_EntityHandle(handle), m_Scene(scene)
        {
            
        }
        
        // ******************************************************************************
        // Add the component T if not added already to this entity
        // ******************************************************************************
        template<typename T, typename... Args>
        T& AddComponent(Args&&... args)
        {
            IK_CORE_ASSERT(!HasComponent<T>(), "Entity already has component!");
            T& component = m_Scene->m_Registry.emplace<T>(m_EntityHandle, std::forward<Args>(args)...);
            return component;
        }

        // ******************************************************************************
        // Get the T Component from Entity (If present)
        // ******************************************************************************
        template<typename T>
        T& GetComponent()
        {
            IK_CORE_ASSERT(HasComponent<T>(), "Entity does not have component!");
            return m_Scene->m_Registry.get<T>(m_EntityHandle);
        }
        
        // ******************************************************************************
        // Check the presence of T component in the Entity
        // ******************************************************************************
        template<typename T>
        bool HasComponent()
        {
            return m_Scene->m_Registry.has<T>(m_EntityHandle);
        }
        
        // ******************************************************************************
        // Remove the component T from entity (If present)
        // ******************************************************************************
        template<typename T>
        void RemoveComponent()
        {
            IK_CORE_ASSERT(HasComponent<T>(), "Entity does not have component!");
            m_Scene->m_Registry.remove<T>(m_EntityHandle);
        }
        
        UUID GetUUID() { return GetComponent<IDComponent>().ID; }
        
        operator entt::entity() const { return m_EntityHandle; }
        operator bool() const { return m_EntityHandle != entt::null; }
        operator uint32_t() const { return (uint32_t)m_EntityHandle; }

        bool operator ==(const Entity& other) { return m_EntityHandle == other.m_EntityHandle && m_Scene == other.m_Scene; }
        bool operator !=(const Entity& other) { return !(*this == other); }
        
        bool operator ==(const entt::entity& other) { return m_EntityHandle == other; }
        bool operator !=(const entt::entity& other) { return !(*this == other); }

    private:
        entt::entity m_EntityHandle{ entt::null };
        Scene*       m_Scene;
    };
    
}
