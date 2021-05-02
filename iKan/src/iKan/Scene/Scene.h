// ******************************************************************************
// File         : Scene.h
// Project      : iKan : Scene
// Description  : Class tp store and manage Scene
//
// Created on   : 28/04/21.
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
        Scene();
        ~Scene();
        
        Entity CreateEntity(const std::string& name = "Unknown Entity", UUID uuid = UUID());
        void DestroyEntity(Entity entity);
        
        void OnUpdateEditor(Timestep ts, EditorCamera& camera);
        void OnViewportResize(uint32_t width, uint32_t height);
        
    private:
        // Container that contain all the entities
        entt::registry m_Registry;
        
        // Store the map of Entityes present in the Scene with their UUID
        std::unordered_map<UUID, Entity> m_EntityIDMap;
        
        uint32_t m_ViewportWidth = 1280.0f, m_ViewportHeight = 720.0f;
        
        friend class Entity;
        friend class SceneHeirarchyPannel;
    };
    
}
