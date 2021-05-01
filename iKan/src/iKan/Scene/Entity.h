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


        ~Entity() = default;
        
    private:
        entt::entity m_EntityHandle{ entt::null };
        Scene*       m_Scene;
    };
    
}
