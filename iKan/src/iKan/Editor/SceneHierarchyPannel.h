// ******************************************************************************
// File         : SceneHierarchyPannel.ch
// Description  : Class for having property pannel of each entity
// Project      : iKan : Editor
//
// Created by Ashish on 02/05/21.
// Copyright © 2021 Ashish. All rights reserved.
// ******************************************************************************

#pragma once

#include <iKan/Scene/Entity.h>

namespace iKan {
    
    // ******************************************************************************
    // Scene Hierarchy Pannel class
    // ******************************************************************************
    class SceneHeirarchyPannel
    {
    private:
        const Ref<Texture> m_DefaultTexture = Texture::Create("../../../iKan/src/iKan/Editor/assets/resources/Default/NoTexture.png");

    public:
        SceneHeirarchyPannel();
        SceneHeirarchyPannel(const Ref<Scene>& scene);

        void SetContext(const Ref<Scene>& scene);
        void OnImguiender(bool* pIsOpen = nullptr);

        void SetSelectedEntity(Entity entity) { m_SelectedEntity = entity; }
        Entity GetSelectedEntity() const { return m_SelectedEntity; }

    public:
        bool isSceneHeirarchypanel = true;

    private:
        void Init(const Ref<Scene>& scene);
        
        void DrawEntityNode(Entity entity);
        void DrawComponents(Entity entity);
        void AddComponent();

    private:
        Ref<Scene> m_Context;
        Entity     m_SelectedEntity;
    };
    
}
