@@ -0,0 +1,42 @@
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

namespace iKan {
    
    // ******************************************************************************
    // Base class for each stribtable entity or native sctipt
    // ******************************************************************************
    class ScriptableEntity
    {
    public:
        virtual ~ScriptableEntity() = default;
        
        template<typename T>
        bool HasComponent() { return m_Entity.HasComponent<T>(); }
        
        template<typename T>
        T& GetComponent() { return m_Entity.GetComponent<T>(); }
        
    protected:
        virtual void OnCreate() { m_Created = true; }
        virtual void OnUpdate(TimeStep ts) {}
        virtual void OnDestroy() {}
        
    protected:
        bool m_Created = false;
        Entity m_Entity;
        
        friend class Scene;
    };
    
}
