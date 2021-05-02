// ******************************************************************************
// File         : SceneHierarchyPannel.ch
// Project      : iKan
// Description  : Class for having property pannel of each entity
//
// Created      : on 02/05/21.
// ******************************************************************************

#pragma once

#include <iKan/Scene/Entity.h>

namespace iKan {
    
    // ******************************************************************************
    // Scene Hierarchy Pannel class
    // ******************************************************************************
    class SceneHeirarchyPannel
    {
    public:
        SceneHeirarchyPannel() = default;
        SceneHeirarchyPannel(const Ref<Scene>& scene);
        
        void SetContext(const Ref<Scene>& scene);
        void OnImguiender(bool* pIsOpen = nullptr);

        void SetSelectedEntity(Entity entity) { m_SelectionContext = entity; }
        Entity GetSelectedEntity() const { return m_SelectionContext; }
        
    private:
        void DrawEntityNode(Entity entity);
        void DrawComponents(Entity entity);

    private:
        Ref<Scene> m_Context;
        Entity m_SelectionContext;
    };
    
}
