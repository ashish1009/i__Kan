// ******************************************************************************
// File         : ScriptableEntity.h
// Description  : Base class for each stribtable entity or native sctipt
// Project      : iKan : Scene
//
// Created by Ashish on 16/09/21.
// Copyright © 2021 Ashish. All rights reserved.
// ******************************************************************************

#pragma once

#include <iKan/Scene/Entity.h>
#include <iKan/Core/Events/Event.h>

namespace iKan {
    
    class Scene;
    
    // ******************************************************************************
    // Base class for each stribtable entity or native sctipt
    // ******************************************************************************
    class ScriptableEntity
    {
    public:
        ScriptableEntity(Ref<Scene>& scene)
        : m_ActiveScene(scene) { }
        
        virtual ~ScriptableEntity() = default;
        
        template<typename T>
        bool HasComponent() { return m_Entity.HasComponent<T>(); }
        
        template<typename T>
        T& GetComponent() { return m_Entity.GetComponent<T>(); }
        
    protected:
        virtual void OnCreate() {}
        virtual void OnUpdate(Timestep ts) {}
        virtual void OnDestroy() {}
        virtual void ImguiRenderer() {}
        virtual void OnEvent(Event& event) {}
        virtual void OnCollision(Entity& colloidedEntity) {}
        
    protected:
        bool m_Created   = false;
        bool m_Activated = false;
        
        Ref<Scene> m_ActiveScene;
        Entity m_Entity;
        
        friend class Scene;
    };
    
}
