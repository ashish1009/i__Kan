// ******************************************************************************
// File         : Scene.h
// Description  : Class tp store and manage Scene
// Project      : iKan : Scene
//
// Created by Ashish on 28/04/21.
// Copyright © 2021 Ashish. All rights reserved.
// ******************************************************************************

#pragma once

#include <entt.hpp>
#include <iKan/Core/UUID.h>
#include <iKan/Editor/EditorCamera.h>
#include <iKan/Core/TimeStep.h>

namespace iKan {
    
    class Entity;
    class SceneHeirarchyPannel;
    class Scene
    {
    public:
        enum class BoxCollisionClass
        {
            Right   = BIT(0),
            Left    = BIT(1),
            Up      = BIT(2),
            Down    = BIT(3)
        };

    public:
        Scene();
        ~Scene();
        
        Entity CreateEntity(const std::string& name = "Unknown Entity", UUID uuid = UUID());
        void DestroyEntity(Entity entity);
        
        void OnUpdateEditor(Timestep ts, EditorCamera& camera);
        void OnViewportResize(uint32_t width, uint32_t height);
        void OnUpdateRuntime(Timestep ts);

        int32_t GetEntityIdFromPixels(int32_t mx, int32_t my);
        uint32_t GetNumEntities() const { return m_NumEntities; }
        int32_t OnBoxColloider(Entity& currEntity, float speed);

    private:
        Entity GetMainCameraEntity();
        void RenderSpriteComponent();

    private:
        uint32_t m_NumEntities = 0;

        // Container that contain all the entities
        entt::registry m_Registry;
        
        // Store the map of Entityes present in the Scene with their UUID
        std::unordered_map<UUID, Entity> m_EntityIDMap;
        
        uint32_t m_ViewportWidth = 1280.0f, m_ViewportHeight = 720.0f;
        
        friend class Entity;
        friend class SceneHeirarchyPannel;
    };
    
}
